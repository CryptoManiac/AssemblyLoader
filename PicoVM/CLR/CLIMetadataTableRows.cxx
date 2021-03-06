#include <sstream>
#include <iomanip>

#include "EnumCasting.hxx"
#include "CLIMetadataTableRows.hxx"

using namespace std;

void MetadataRowsReader::Init(CLIMetadata& cliMetadata) {
    const auto metaHeaderOffset = cliMetadata.getStreamOffset({'#', '~'});

    stringStreamOffset = cliMetadata.getStreamOffset({'#', 'S', 't', 'r', 'i', 'n', 'g', 's'});
    guidStreamOffset = cliMetadata.getStreamOffset({'#', 'G', 'U', 'I', 'D'});
    blobStreamOffset = cliMetadata.getStreamOffset({'#', 'B', 'l', 'o', 'b'});

    // Encodes how wide indexes into the various heaps are. Default width is 16 bit, indexes can be either 16 or 32 bit long.
    //
    // 0x01 Size of #String stream is 32 bit.
    // 0x02 Size of #GUID stream is 32 bit.
    // 0x04 Size of #Blob stream is 32 bit.
    const auto heapSizes = reader[metaHeaderOffset + 6];

    // A 64-bit number that has a bit set for each table that is present in the assembly.
    const auto valid = reader.read_uint64(metaHeaderOffset + 8);

    metaDataOffset = metaHeaderOffset + 24;
    reader.seek(metaDataOffset);

    for (const auto& item : cliMetadataTableNames) {
        auto bit = item.first;
        if (((valid >> _u(bit)) & 1) != 0) {
            // Load table length record for existent and valid table.
            mapTableLength[bit] = reader.read_uint32();
        } else {
            mapTableLength[bit] = 0;
        }
    }

    stringsIsLong = (heapSizes & 0x01) != 0;
    guidIsLong = (heapSizes & 0x02) != 0;
    blobIsLong = (heapSizes & 0x04) != 0;
}

MetadataRowsReader::MetadataRowsReader(AssemblyReader& Reader, CLIMetadata& cliMetadata) : reader (Reader)
{
    Init(cliMetadata);
}

MetadataRowsReader& MetadataRowsReader::operator=(const MetadataRowsReader& other) {
    MetadataRowsReader(other).swap(*this);
    return *this;
}

void MetadataRowsReader::swap(MetadataRowsReader& other) noexcept {
    reader.swap(other.reader);
    mapTableLength.swap(other.mapTableLength);
    ::swap(stringsIsLong, other.stringsIsLong);
    ::swap(guidIsLong, other.guidIsLong);
    ::swap(blobIsLong, blobIsLong);
    ::swap(metaDataOffset, other.metaDataOffset);
    ::swap(stringStreamOffset, other.stringStreamOffset);
    ::swap(guidStreamOffset, other.guidStreamOffset);
    ::swap(blobStreamOffset, other.blobStreamOffset);
}


// Read 16 or 32 bit index and get the utf8 string at this index.
void MetadataRowsReader::readString(u16string& result) {
    uint32_t offset = stringsIsLong ? reader.read_uint32() : reader.read_uint16();
    reader.read_utf8z(result, stringStreamOffset + offset, 0xffff);
}

// Read 16 or 32 bit index, fill result by unique ID from this index.
void MetadataRowsReader::readGuid(Guid& result) {
    uint32_t index = guidIsLong ? reader.read_uint32() : reader.read_uint16();
    if (index != 0) {
        reader.read_guid(result, guidStreamOffset + ((index - 1) << 4));
    }
    // If index is zero then do nothing.
}

// Read 16 or 32 bit index, and fill the vector by binary data at this index.
void MetadataRowsReader::readBlob(vector<uint8_t>& result) {
    uint32_t index = blobIsLong ? reader.read_uint32() : reader.read_uint16();
    auto offset = blobStreamOffset + index;
    uint32_t length;
    // Get length of the following data stream
    auto read = reader.read_varsize(length, offset);
    // Read binary data into vector
    reader.read_bytes(result, offset + read, length);
}

// Pop 16 or 32 bit index, read binary data and parse it as a signature.
void MetadataRowsReader::readSignature(vector<uint32_t>& result) {
     vector<uint8_t> buffer;
     readBlob(buffer);

    // Signature is presented by a set of variable length 
    //   integers, we're simply reading these numbers consequently.
    uint32_t offset = 0;
    while (offset < buffer.size()) {
        uint32_t value;
        uint32_t read = AssemblyReader::read_varsize(value, buffer, offset);
        result.push_back(value);
        offset += read;
    }
}

// Read row index.
uint32_t MetadataRowsReader::readRowIndex(CLIMetadataTableItem tableIndex) {
    // Using 32 bit addresses if table has more than 0xffff rows.
    return mapTableLength[tableIndex] >= 0xffff ? reader.read_uint32() : reader.read_uint16();
}

// Decode polymorphic index
pair<uint32_t, CLIMetadataTableItem> MetadataRowsReader::readRowIndexChoice(const vector<CLIMetadataTableItem>& tables) {
    uint32_t max = 0;

    for (const auto& tableID : tables) {
        const auto& mapItem = mapTableLength.find(tableID);
        if (tableID != CLIMetadataTableItem::Unknown && mapItem != mapTableLength.end() && max < (*mapItem).second) {
            max = (*mapItem).second;
        }
    }

    uint32_t shift = 0, bit = 1;
    while (tables.size() > bit) {
        bit <<= 1;
        ++shift;
    }

    uint32_t index = (max << shift) >= 0xffff ? reader.read_uint32() : reader.read_uint16();

    return{ index, tables[index & (bit - 1)] };
}

ModuleRow::ModuleRow(MetadataRowsReader& mr) {
    // Module generation, currently it is set to zero.
    generation = mr.reader.read_uint16();

    // Module name.
    mr.readString(name);

    // Unique identifier which is used to distinguish between two versions of the same module.
    mr.readGuid(guid);

    // These two reserved fields are not presented in ModuleRow structure.
    // EncId (index into GUID heap, always set to zero).
    // EncBaseId (index into GUID heap, always set to zero).
    Guid tmp;
    mr.readGuid(tmp); // encId
    mr.readGuid(tmp); // endBaseId
}

string ModuleRow::str() const {
    ostringstream ss;
    ss << "Module(" << endl
       << " generation=" << dec << generation << endl
       << " name=" << string(name.begin(), name.end()) << endl
       << " guid=" << guid << endl
       << ")" << endl;

       return ss.str();
}

ModuleRow::~ModuleRow() noexcept
{
}

TypeRefRow::TypeRefRow(MetadataRowsReader& mr) {
    // ResolutionScope coded index
    resolutionScope = mr.readRowIndexChoice(resolutionScopeIndex);

    // Type name and namespace
    mr.readString(typeName);
    mr.readString(typeNamespace);
}

TypeRefRow::~TypeRefRow() noexcept
{
}

TypeDefRow::TypeDefRow(MetadataRowsReader& mr) {
    // 4-byte bit mask of type TypeAttributes
    flags = mr.reader.read_uint32();
    // Type name and namespace
    mr.readString(typeName);
    mr.readString(typeNamespace);
    // TypeDefOrRef coded index into TypeDef, TypeRef or TypeSpec
    extendsType = mr.readRowIndexChoice(typeDefOrRef);
    // Index into FieldDef table
    fieldList = mr.readRowIndex(CLIMetadataTableItem::FieldDef);
    // Index into MethodDef table
    methodList = mr.readRowIndex(CLIMetadataTableItem::MethodDef);
}

TypeDefRow::~TypeDefRow() noexcept
{
}

FieldDefRow::FieldDefRow(MetadataRowsReader& mr) {
    // 2-byte bit mask of type FieldAttributes
    flags = mr.reader.read_uint16();
    mr.readString(name);
    mr.readSignature(signature);
}

FieldDefRow::~FieldDefRow() noexcept
{
}

MethodDefRow::MethodDefRow(MetadataRowsReader& mr) {
    rva = mr.reader.read_uint32();
    // 2-byte bit mask of type MethodImplAttributes
    implFlags = mr.reader.read_uint16();
    flags = mr.reader.read_uint16();
    mr.readString(name);
    mr.readSignature(signature);
    // Index into the ParamDef table
    paramList = mr.readRowIndex(CLIMetadataTableItem::ParamDef);
}

MethodDefRow::~MethodDefRow() noexcept
{
}

ParamDefRow::ParamDefRow(MetadataRowsReader& mr) {
    // 2-byte bit mask of type ParamAttributes
    flags = mr.reader.read_uint16();
    sequence = mr.reader.read_uint16();
    mr.readString(name);
}

InterfaceImplRow::InterfaceImplRow(MetadataRowsReader& mr) {
    // Index into the TypeDef table
    classRef = mr.readRowIndex(CLIMetadataTableItem::TypeDef);
    // TypeDefOrRef index into TypeDef, TypeRef or TypeSpec
    interfaceRef = mr.readRowIndexChoice(typeDefOrRef);
}

MemberRefRow::MemberRefRow(MetadataRowsReader& mr) {
    // MemberRefParent index into the TypeRef, ModuleRef, MethodDef, TypeSpec, or TypeDef tables
    classRef = mr.readRowIndexChoice(memberRefParent);
    mr.readString(name);
    mr.readSignature(signature);
}

MemberRefRow::~MemberRefRow() noexcept
{
}

ConstantRow::ConstantRow(MetadataRowsReader& mr) {
    type = mr.reader.read_uint16();
    // HasConstant index into the ParamDef or FieldDef or Property table
    parent = mr.readRowIndexChoice(hasConstant);
    mr.readBlob(value);
}

CustomAttributeRow::CustomAttributeRow(MetadataRowsReader& mr) {
    // HasCustomAttribute index
    parent = mr.readRowIndexChoice(hasCustomAttribute);
    // CustomAttributeType index
    type = mr.readRowIndexChoice(customAttributeType);
    mr.readBlob(value);
}

FieldMarshalRow::FieldMarshalRow(MetadataRowsReader& mr) {
    // HasFieldMarshal index
    parent = mr.readRowIndexChoice(hasFieldMarshall);
    mr.readBlob(nativeType);
}

DeclSecurityRow::DeclSecurityRow(MetadataRowsReader& mr) {
    action = mr.reader.read_uint16();
    parent = mr.readRowIndexChoice(hasDeclSecurity);
    mr.readBlob(permissionSet);
}

ClassLayoutRow::ClassLayoutRow(MetadataRowsReader& mr) {
    packingSize = mr.reader.read_uint16();
    classSize = mr.reader.read_uint32();
    parent = mr.readRowIndex(CLIMetadataTableItem::TypeDef);
}

FieldLayoutRow::FieldLayoutRow(MetadataRowsReader& mr) {
    offset = mr.reader.read_uint32();
    parent = mr.readRowIndex(CLIMetadataTableItem::FieldDef);
}

EventMapRow::EventMapRow(MetadataRowsReader& mr) {
    parent = mr.readRowIndex(CLIMetadataTableItem::TypeDef);
    eventList = mr.readRowIndex(CLIMetadataTableItem::Event);
}

EventRow::EventRow(MetadataRowsReader& mr) {
    // 2-byte bit mask of type EventAttribute
    eventFlags = mr.reader.read_uint16();
    mr.readString(name);
    // TypeDefOrRef index
    eventType = mr.readRowIndexChoice(typeDefOrRef);
}

PropertyMapRow::PropertyMapRow(MetadataRowsReader& mr) {
    parent = mr.readRowIndex(CLIMetadataTableItem::TypeDef);
    propertyList = mr.readRowIndex(CLIMetadataTableItem::Property);
}

PropertyRow::PropertyRow(MetadataRowsReader& mr) {
    // 2-byte bit mask of type PropertyAttributes
    flags = mr.reader.read_uint16();
    mr.readString(name);
    // A signature from the Blob heap
    mr.readSignature(signature);
}

PropertyRow::~PropertyRow() noexcept
{
}

MethodSemanticsRow::MethodSemanticsRow(MetadataRowsReader& mr) {
    // 2-byte bit mask of type MethodSemanticsAttributes
    semantics = mr.reader.read_uint16();
    // Index into the MethodDef table
    method = mr.readRowIndex(CLIMetadataTableItem::MethodDef);
    // HasSemantics index into the Event or Property table
    association = mr.readRowIndexChoice(hasSemantics);
}

MethodImplRow::MethodImplRow(MetadataRowsReader& mr) {
    // Index into TypeDef table
    classRef = mr.readRowIndex(CLIMetadataTableItem::TypeDef);
    // Index into MethodDef or MemberRef table
    methodBody = mr.readRowIndexChoice(methodDefOrRef);
    methodDeclaration = mr.readRowIndexChoice(methodDefOrRef);
}

ImplMapRow::ImplMapRow(MetadataRowsReader& mr) {
    // 2-byte bit mask of type PInvokeAttributes
    mappingFlags = mr.reader.read_uint16();
    // MemberForwarded  index into the FieldDef or MethodDef table
    memberForwarded = mr.readRowIndexChoice(memberForwardedIndex);
    mr.readString(importName);
    importScope = mr.readRowIndex(CLIMetadataTableItem::ModuleRef);
}

FieldRVARow::FieldRVARow(MetadataRowsReader& mr) {
    // The RVA in this table gives the location of the initial value for a Field.
    rva = mr.reader.read_uint32();
    // Index into FieldDef table
    field = mr.readRowIndex(CLIMetadataTableItem::FieldDef);
}

AssemblyRow::AssemblyRow(MetadataRowsReader& mr) {
    version.clear();
    // 4-byte constant of type AssemblyHashAlgorithm
    hashAlgId = mr.reader.read_uint32();
    // MajorVersion
    version.push_back(mr.reader.read_uint16());
    // MinorVersion
    version.push_back(mr.reader.read_uint16());
    // BuildNumber
    version.push_back(mr.reader.read_uint16());
    // RevisionNumber 
    version.push_back(mr.reader.read_uint16());
    // 4-byte bit mask of type AssemblyFlags
    flags = mr.reader.read_uint32();

    mr.readBlob(publicKey);
    mr.readString(name);
    mr.readString(culture);
}

AssemblyRow::~AssemblyRow() noexcept
{
}

AssemblyOSRow::AssemblyOSRow(MetadataRowsReader& mr) {
    osPlatformID = mr.reader.read_uint32();
    osMajorVersion = mr.reader.read_uint32();
    osMinorVersion = mr.reader.read_uint32();
}

AssemblyRefRow::AssemblyRefRow(MetadataRowsReader& mr) {
    version.clear();

    // MajorVersion
    version.push_back(mr.reader.read_uint16());
    // MinorVersion
    version.push_back(mr.reader.read_uint16());
    // BuildNumber
    version.push_back(mr.reader.read_uint16());
    // RevisionNumber
    version.push_back(mr.reader.read_uint16());
    // 4-byte bit mask of type AssemblyFlags
    flags = mr.reader.read_uint32();

    mr.readBlob(publicKeyOrToken);
    mr.readString(name);
    mr.readString(culture);
    mr.readBlob(hashValue);
}

AssemblyRefRow::~AssemblyRefRow() noexcept
{
}

AssemblyRefProcessorRow::AssemblyRefProcessorRow(MetadataRowsReader& mr) {
    processor = mr.reader.read_uint32();
    assemblyRef = mr.readRowIndex(CLIMetadataTableItem::AssemblyRef);
}

AssemblyRefOSRow::AssemblyRefOSRow(MetadataRowsReader& mr) {
    osPlatformID = mr.reader.read_uint32();
    osMajorVersion = mr.reader.read_uint32();
    osMinorVersion = mr.reader.read_uint32();
    assemblyRef = mr.readRowIndex(CLIMetadataTableItem::AssemblyRef);
}

FileRow::FileRow(MetadataRowsReader& mr) {
    // 4-byte bit mask of type FileAttributes
    flags = mr.reader.read_uint32();
    mr.readString(name);
    mr.readBlob(hashValue);
}

FileRow::~FileRow() noexcept
{
}

ExportedTypeRow::ExportedTypeRow(MetadataRowsReader& mr) {
    // 4-byte bit mask of type TypeAttributes
    flags = mr.reader.read_uint32();
    // 4-byte index into a TypeDef table of another module in this Assembly
    typeDefId = mr.reader.read_uint32();
    mr.readString(typeName);
    mr.readString(typeNamespace);
    implementation = mr.readRowIndexChoice(implementationIndex);
}

ExportedTypeRow::~ExportedTypeRow() noexcept
{
}

ManifestResourceRow::ManifestResourceRow(MetadataRowsReader& mr) {
    offset = mr.reader.read_uint32();
    // 4-byte bit mask of type ManifestResourceAttributes
    flags = mr.reader.read_uint32();
    mr.readString(name);
    implementation = mr.readRowIndexChoice(implementationIndex);
}

NestedClassRow::NestedClassRow(MetadataRowsReader& mr) {
    nestedClass = mr.readRowIndex(CLIMetadataTableItem::TypeDef);
    enclosingClass = mr.readRowIndex(CLIMetadataTableItem::TypeDef);
}

GenericParamRow::GenericParamRow(MetadataRowsReader& mr) {
    // 2-byte index of the generic parameter
    number = mr.reader.read_uint16();
    // 2-byte bitmask of type GenericParamAttributes
    flags = mr.reader.read_uint16();
    // TypeOrMethodDef index into the TypeDef or MethodDef table
    owner = mr.readRowIndexChoice(typeOrMethodDef);
    mr.readString(name);
}

MethodSpecRow::MethodSpecRow(MetadataRowsReader& mr) {
    method = mr.readRowIndexChoice(methodDefOrRef);
    mr.readSignature(instantiation);
}

GenericParamConstraintRow::GenericParamConstraintRow(MetadataRowsReader& mr) {
    // Index into the GenericParam table
    owner = mr.readRowIndex(CLIMetadataTableItem::GenericParam);
    constraint = mr.readRowIndexChoice(typeDefOrRef);
}

// Table identifiers
const CLIMetadataTableItem ModuleRow::tableID;
const CLIMetadataTableItem TypeRefRow::tableID;
const CLIMetadataTableItem TypeDefRow::tableID;
const CLIMetadataTableItem FieldDefRow::tableID;
const CLIMetadataTableItem MethodDefRow::tableID;
const CLIMetadataTableItem ParamDefRow::tableID;
const CLIMetadataTableItem InterfaceImplRow::tableID;
const CLIMetadataTableItem MemberRefRow::tableID;
const CLIMetadataTableItem ConstantRow::tableID;
const CLIMetadataTableItem CustomAttributeRow::tableID;
const CLIMetadataTableItem FieldMarshalRow::tableID;
const CLIMetadataTableItem DeclSecurityRow::tableID;
const CLIMetadataTableItem ClassLayoutRow::tableID;
const CLIMetadataTableItem FieldLayoutRow::tableID;
const CLIMetadataTableItem EventMapRow::tableID;
const CLIMetadataTableItem EventRow::tableID;
const CLIMetadataTableItem PropertyMapRow::tableID;
const CLIMetadataTableItem PropertyRow::tableID;
const CLIMetadataTableItem MethodSemanticsRow::tableID;
const CLIMetadataTableItem MethodImplRow::tableID;
const CLIMetadataTableItem ImplMapRow::tableID;
const CLIMetadataTableItem FieldRVARow::tableID;
const CLIMetadataTableItem AssemblyRow::tableID;
const CLIMetadataTableItem AssemblyOSRow::tableID;
const CLIMetadataTableItem AssemblyRefRow::tableID;
const CLIMetadataTableItem AssemblyRefProcessorRow::tableID;
const CLIMetadataTableItem AssemblyRefOSRow::tableID;
const CLIMetadataTableItem FileRow::tableID;
const CLIMetadataTableItem ExportedTypeRow::tableID;
const CLIMetadataTableItem ManifestResourceRow::tableID;
const CLIMetadataTableItem NestedClassRow::tableID;
const CLIMetadataTableItem GenericParamRow::tableID;
const CLIMetadataTableItem MethodSpecRow::tableID;
const CLIMetadataTableItem GenericParamConstraintRow::tableID;
