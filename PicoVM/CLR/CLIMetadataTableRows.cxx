#include <sstream>
#include <iomanip>

#include "CLIMetadataTableRows.hxx"

using namespace std;

// Read 16 or 32 bit index and get the utf8 string at this index.
void MetadataRowsReader::readString(vector<uint16_t>& result) {
    uint32_t offset = stringsIsLong ? reader.read_uint32() : reader.read_uint16();
    reader.read_utf8z(result, stringStreamOffset + offset, 0xffff);
};

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

string ModuleRow::str() {
    ostringstream ss;
    ss << "Module(" << endl
       << " generation=" << dec << generation << endl
       << " name=" << string(name.begin(), name.end()) << endl
       << " guid=" << guid << endl
       << ")" << endl;

       return ss.str();
}

// TODO: replace stubs with real constructors :D

TypeRefRow::TypeRefRow(MetadataRowsReader& mr) { /* stub */ }
TypeDefRow::TypeDefRow(MetadataRowsReader& mr) { /* stub */ }
FieldDefRow::FieldDefRow(MetadataRowsReader& mr) { /* stub */ }
MethodDefRow::MethodDefRow(MetadataRowsReader& mr) { /* stub */ }
ParamDefRow::ParamDefRow(MetadataRowsReader& mr) { /* stub */ }
InterfaceImplRow::InterfaceImplRow(MetadataRowsReader& mr) { /* stub */ }
MemberRefRow::MemberRefRow(MetadataRowsReader& mr) { /* stub */ }
ConstantRow::ConstantRow(MetadataRowsReader& mr) { /* stub */ }
CustomAttributeRow::CustomAttributeRow(MetadataRowsReader& mr) { /* stub */ }
FieldMarshalRow::FieldMarshalRow(MetadataRowsReader& mr) { /* stub */ }
DeclSecurityRow::DeclSecurityRow(MetadataRowsReader& mr) { /* stub */ }
ClassLayoutRow::ClassLayoutRow(MetadataRowsReader& mr) { /* stub */ }
FieldLayoutRow::FieldLayoutRow(MetadataRowsReader& mr) { /* stub */ }
EventMapRow::EventMapRow(MetadataRowsReader& mr) { /* stub */ }
EventRow::EventRow(MetadataRowsReader& mr) { /* stub */ }
PropertyMapRow::PropertyMapRow(MetadataRowsReader& mr) { /* stub */ }
PropertyRow::PropertyRow(MetadataRowsReader& mr) { /* stub */ }
MethodSemanticsRow::MethodSemanticsRow(MetadataRowsReader& mr) { /* stub */ }
MethodImplRow::MethodImplRow(MetadataRowsReader& mr) { /* stub */ }
ImplMapRow::ImplMapRow(MetadataRowsReader& mr) { /* stub */ }
FieldRVARow::FieldRVARow(MetadataRowsReader& mr) { /* stub */ }
AssemblyRow::AssemblyRow(MetadataRowsReader& mr) { /* stub */ }
AssemblyOSRow::AssemblyOSRow(MetadataRowsReader& mr) { /* stub */ }
AssemblyRefRow::AssemblyRefRow(MetadataRowsReader& mr) { /* stub */ }
AssemblyRefProcessorRow::AssemblyRefProcessorRow(MetadataRowsReader& mr) { /* stub */ }
AssemblyRefOSRow::AssemblyRefOSRow(MetadataRowsReader& mr) { /* stub */ }
FileRow::FileRow(MetadataRowsReader& mr) { /* stub */ }
ExportedTypeRow::ExportedTypeRow(MetadataRowsReader& mr) { /* stub */ }
ManifestResourceRow::ManifestResourceRow(MetadataRowsReader& mr) { /* stub */ }
NestedClassRow::NestedClassRow(MetadataRowsReader& mr) { /* stub */ }
GenericParamRow::GenericParamRow(MetadataRowsReader& mr) { /* stub */ }
MethodSpecRow::MethodSpecRow(MetadataRowsReader& mr) { /* stub */ }
GenericParamConstraintRow::GenericParamConstraintRow(MetadataRowsReader& mr) { /* stub */ }
