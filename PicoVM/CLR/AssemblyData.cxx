#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>
#include <map>
#include <limits>

#include "AssemblyData.hxx"
#include "Formatting.hxx"
#include "EnumCasting.hxx"

using namespace std;

AssemblyData::CLIMetaDataTables::~CLIMetaDataTables() noexcept
{
}

AssemblyData::AssemblyData(string strFilePathName)
{
    ifstream AssemblyStream(strFilePathName, ios_base::in | ios_base::binary | ios_base::ate);
    if (!AssemblyStream.is_open()) {
        throw runtime_error("Unable to open assembly file");
    }

    vector<uint8_t> file_bytes;
    file_bytes.reserve(AssemblyStream.tellg());
    AssemblyStream.seekg(ios_base::beg);
    file_bytes.assign((istreambuf_iterator<char>(AssemblyStream)), istreambuf_iterator<char>());

    // Check MZ header
    if (file_bytes[0] != 0x4d || file_bytes[1] != 0x5a) {
        throw runtime_error("There is no MZ header");
    }

    // Create reader and continue initialization process
    reader = AssemblyReader(file_bytes);
    InitAssembly();
}

AssemblyData::AssemblyData(const vector<uint8_t>& assembly_bytes)
{
    // Check MZ header
    if (assembly_bytes[0] != 0x4d || assembly_bytes[1] != 0x5a) {
        throw runtime_error("There is no MZ header");
    }

    // Create reader and continue initialization process
    reader = AssemblyReader(assembly_bytes);
    InitAssembly();
}

AssemblyData& AssemblyData::operator=(const AssemblyData& other) {
    AssemblyData(other).swap(*this);
    return *this;
}

void AssemblyData::swap(AssemblyData& other) noexcept {
    sections.swap(other.sections);
    ::swap(cliHeader, other.cliHeader);
    ::swap(cliMetadata, other.cliMetadata);
    ::swap(cliMetaDataTables, other.cliMetaDataTables);
    ::swap(fileHeader, other.fileHeader);
    reader.swap(other.reader);
}

void AssemblyData::InitAssembly()
{
    // PE headers
    auto peOffset = reader.read_uint32(0x3c);
    if (reader.read_uint32(peOffset) != 0x4550) {
        // PE header should begin with PE\0\0 magic value.
        throw runtime_error("Invalid PE header");
    }

    // PE standard identifier
    auto standard = reader.read_uint16(peOffset + 24);
    ImageDataDirectory cliDirectory;

    switch (standard) {
    // CLR has a support for two variations of PE format, PE32 and PE32+.

    case 0x010B: {
        // PE32 header
        ImageNTHeader32 header;
        reader.read_ntheader32(header, peOffset);
        if (header.optionalHeader.nt.numberOfRvaAndSizes != ImageOptionalDirectoriesNumber32) {
            throw runtime_error("Optional header is invalid.");
        }
        fileHeader = header.fileHeader;
        // CLI directory
        cliDirectory = header.optionalHeader.nt.directories[_u(ImageDirectoryType::cliHeader)];
        peOffset += sizeof(ImageNTHeader32);
    } break;
    case 0x020B: {
        // PE32+ header
        ImageNTHeader64 header;
        reader.read_ntheader64(header, peOffset);
        if (header.optionalHeader.nt.numberOfRvaAndSizes != ImageOptionalDirectoriesNumber64) {
            throw runtime_error("Optional header is invalid.");
        }
        fileHeader = header.fileHeader;
        // CLI directory
        cliDirectory = header.optionalHeader.nt.directories[_u(ImageDirectoryType::cliHeader)];
        peOffset += sizeof(ImageNTHeader64);
    } break;
    default:
        throw runtime_error("Unsupported PE format");
    }

    // Section Headers
    for (uint32_t i = 0; i < fileHeader.sectionsCount; ++i) {
        ImageSectionHeader header;
        reader.read_sectionheader(header, peOffset);
        sections.push_back(header);
        peOffset += sizeof(ImageSectionHeader);
    }

    // CLI Header
    {
        auto cliHeaderOffset = getDataOffset(cliDirectory.rva);
        if (cliHeaderOffset == numeric_limits<uint32_t>::max()) {
            throw runtime_error("CLI header could not be found.");
        }

        if (cliDirectory.size != sizeof(CLIHeader) || reader.read_uint32(cliHeaderOffset) != sizeof(CLIHeader)) {
            // CLI header size must be identical to one which is defined by CLIHeader structure.
            throw runtime_error("CLI header size is invalid.");
        }

        // Load CLI header
        reader.read_cliheader(cliHeader, cliHeaderOffset);

        if ((cliHeader.flags & _u(CLIHeaderFlags::nativeEntryPoint)) != 0) {
            // Reject header if there are any native-specific flags specified.
            throw runtime_error("Only pure IL assemblies are supported.");
        }
    }

    // CLI MetaData section
    cliMetadata.cliMetadataOffset = getDataOffset(cliHeader.metaData.rva);
    if (reader.read_uint32(cliMetadata.cliMetadataOffset) != 0x424A5342) {
        throw runtime_error("Invalid CLI metadata.");
    }

    // CLI version data string
    auto versionLength = reader.read_uint32(cliMetadata.cliMetadataOffset + 12);
    cliMetadata.version.reserve(versionLength);
    reader.read_utf8z(cliMetadata.version, cliMetadata.cliMetadataOffset + 16, versionLength);

    // Stream headers
    cliMetadata.streamsCount = reader.read_uint16(cliMetadata.cliMetadataOffset + versionLength + 18);
    uint32_t streamsOffset = cliMetadata.cliMetadataOffset + versionLength + 20;
    for (uint32_t i = 0; i < cliMetadata.streamsCount; ++i) {
        CLIMetadata::CLIStream stream = {};
        // Stream offset, relative to the beginning of metadata header
        stream.offset = reader.read_uint32(streamsOffset);
        // Stream length
        stream.size = reader.read_uint32(streamsOffset + 4);
        // Stream name
        reader.read_asciiz(stream.name, streamsOffset + 8, 32);
        cliMetadata.streams.push_back(stream);
        streamsOffset += 8 + ((stream.name.size() + 4) & 0xFC);
    }

    // Load metadata tables
    FillTables();
}

void AssemblyData::FillTables()
{
    // Reader helper class. Basically it's a wrapper around AssemblyReader, which provides a set of methods used by row constructors.
    MetadataRowsReader mr(reader, cliMetadata);

    // Verify Module table
    if (mr.mapTableLength[CLIMetadataTableItem::Module] != 1) {
        throw runtime_error("Module table must contain one and only one row.");
    }

    // Load module information.
    FillTable<ModuleRow>(mr, cliMetaDataTables._Module);

    // TypeRef
    FillTable<TypeRefRow>(mr, cliMetaDataTables._TypeRef);

    // TypeDef
    FillTable<TypeDefRow>(mr, cliMetaDataTables._TypeDef);

    // FieldDef
    FillTable<FieldDefRow>(mr, cliMetaDataTables._FieldDef);

    // MethodDef
    FillTable<MethodDefRow>(mr, cliMetaDataTables._MethodDef);

    // ParamDef
    FillTable<ParamDefRow>(mr, cliMetaDataTables._ParamDef);

    // InterfaceImpl
    FillTable<InterfaceImplRow>(mr, cliMetaDataTables._InterfaceImpl);

    // MemberRef
    FillTable<MemberRefRow>(mr, cliMetaDataTables._MemberRef);

    // Constant
    FillTable<ConstantRow>(mr, cliMetaDataTables._Constant);

    // CustomAttribute
    FillTable<CustomAttributeRow>(mr, cliMetaDataTables._CustomAttribute);

    // FieldMarshal
    FillTable<FieldMarshalRow>(mr, cliMetaDataTables._FieldMarshal);

    // DeclSecurity
    FillTable<DeclSecurityRow>(mr, cliMetaDataTables._DeclSecurity);

    // ClassLayout
    FillTable<ClassLayoutRow>(mr, cliMetaDataTables._ClassLayout);

    // FieldLayout
    FillTable<FieldLayoutRow>(mr, cliMetaDataTables._FieldLayout);

    // StandAloneSig
    // Each row represents a signature that isn't referenced by any other table.
    cliMetaDataTables._StandAloneSig.reserve(mr.mapTableLength[CLIMetadataTableItem::StandAloneSig]);
    for (uint32_t n = 0; n < mr.mapTableLength[CLIMetadataTableItem::StandAloneSig]; ++n) {
        vector<uint32_t> signature;
        mr.readSignature(signature);
        cliMetaDataTables._StandAloneSig.push_back(signature);
    }

    // EventMap
    FillTable<EventMapRow>(mr, cliMetaDataTables._EventMap);

    // Event
    FillTable<EventRow>(mr, cliMetaDataTables._Event);

    // PropertyMap
    FillTable<PropertyMapRow>(mr, cliMetaDataTables._PropertyMap);

    // Property
    FillTable<PropertyRow>(mr, cliMetaDataTables._Property);

    // MethodSemantics
    FillTable<MethodSemanticsRow>(mr, cliMetaDataTables._MethodSemantics);

    // MethodImpl
    FillTable<MethodImplRow>(mr, cliMetaDataTables._MethodImpl);

    // ModuleRef
    cliMetaDataTables._ModuleRef.reserve(mr.mapTableLength[CLIMetadataTableItem::ModuleRef]);
    for (uint32_t n = 0; n < mr.mapTableLength[CLIMetadataTableItem::ModuleRef]; ++n) {
        u16string name;
        mr.readString(name);
        cliMetaDataTables._ModuleRef.push_back(name);
    }

    // TypeSpec
    cliMetaDataTables._TypeSpec.reserve(mr.mapTableLength[CLIMetadataTableItem::TypeSpec]);
    for (uint32_t n = 0; n < mr.mapTableLength[CLIMetadataTableItem::TypeSpec]; ++n) {
        vector<uint32_t> signature;
        mr.readSignature(signature);
        cliMetaDataTables._TypeSpec.push_back(signature);
    }

    // ImplMap
    FillTable<ImplMapRow>(mr, cliMetaDataTables._ImplMap);

    // FieldRVA
    FillTable<FieldRVARow>(mr, cliMetaDataTables._FieldRVA);

    // Assembly
    FillTable<AssemblyRow>(mr, cliMetaDataTables._Assembly);

    // AssemblyProcessor
    for (uint32_t n = 0; n < mr.mapTableLength[CLIMetadataTableItem::AssemblyProcessor]; ++n) {
        cliMetaDataTables._AssemblyProcessor.push_back(reader.read_uint32());
    }

    // AssemblyOS
    FillTable<AssemblyOSRow>(mr, cliMetaDataTables._AssemblyOS);

    // AssemblyRef
    FillTable<AssemblyRefRow>(mr, cliMetaDataTables._AssemblyRef);

    // AssemblyRefProcessor
    FillTable<AssemblyRefProcessorRow>(mr, cliMetaDataTables._AssemblyRefProcessor);

    // AssemblyRefOS
    FillTable<AssemblyRefOSRow>(mr, cliMetaDataTables._AssemblyRefOS);

    // File
    FillTable<FileRow>(mr, cliMetaDataTables._File);

    // ExportedType
    FillTable<ExportedTypeRow>(mr, cliMetaDataTables._ExportedType);

    // ManifestResource
    FillTable<ManifestResourceRow>(mr, cliMetaDataTables._ManifestResource);

    // NestedClass
    FillTable<NestedClassRow>(mr, cliMetaDataTables._NestedClass);

    // GenericParam
    FillTable<GenericParamRow>(mr, cliMetaDataTables._GenericParam);

    // MethodSpec
    FillTable<MethodSpecRow>(mr, cliMetaDataTables._MethodSpec);

    // GenericParamConstraint
    FillTable<GenericParamConstraintRow>(mr, cliMetaDataTables._GenericParamConstraint);

    // Load method bodies.
    for (uint32_t n = 0; n < mr.mapTableLength[CLIMetadataTableItem::MethodDef]; ++n) {
        loadMethodBody(n);
    }
}

// Get physical offset from the beginning of file.
uint32_t AssemblyData::getDataOffset(uint32_t address) const
{
    for (const auto& section : sections) {
        if (section.virtualAddress <= address && address < section.virtualAddress + section.virtualSize) {
            return section.pointerToRawData + (address - section.virtualAddress);
        }
    }

    // It looks like we weren't able to find anything
    return numeric_limits<uint32_t>::max();
}

const MethodDefRow& AssemblyData::getMethodDef(uint32_t token) const
{
    return cliMetaDataTables._MethodDef[(token & 0xFFFFFF) - 1];
}


// Get method information
void AssemblyData::loadMethodBody(uint32_t index)
{
    using bflags = MethodBodyFlags;
    using eflags = ExceptionFlags;

    MethodDefRow& methodDef = cliMetaDataTables._MethodDef[index];
    MethodBody& methodBody = methodDef.methodBody;

    if (methodDef.rva == 0) {
        // There is no code to search for, it looks like we have a virtual or PInvoke method here.
        return;
    }

    auto offset = getDataOffset(methodDef.rva);
    auto format = bflags(reader[offset] & 0x03);
    reader.seek(offset);

    if (format == bflags::TinyFormat) {
        // "For a method to have its IL instructions formatted in a tiny format, the following must be true:
        // * No local variables exist.
        // * No exceptions exist.
        // * No extra data sections exist.
        // * The operand stack cannot be longer than eight entries.
        // * The method is less than 64 bytes.
        // If these conditions are true, then a method can be coded tiny. The other 6 bits of the first byte contain the size of the method. IL instructions start with the next byte."
        //
        // - p.125 of ".NET Common Language Runtime Unleashed" by Kevin Burton
        //
        methodBody.maxStack = 8;
        auto length = reader.read_uint8() >> 2;
        reader.read_bytes(methodBody.data, length);
    } else if (format == bflags::FatFormat) {
        // "If any of the conditions specified for a tiny format are not true, then the method uses a
        // fat format. The fat format header has the following structure:
        // typedef struct IMAGE_COR_ILMETHOD_FAT
        // {
        //      unsigned Flags : 12;
        //      unsigned Size : 4;
        //      unsigned MaxStack : 16;
        //      DWORD CodeSize;
        //      mdSignature LocalVarSigTok;
        // } IMAGE_COR_ILMETHOD_FAT;
        // Other than indicating that this is a fat format, two additional flags exist: one flag indi-
        // cates that the local variables should be initialized, and another indicates that additional
        // sections of code follow the instruction block."
        //
        // - p.125 of ".NET Common Language Runtime Unleashed" by Kevin Burton
        //
        auto flags = reader.read_uint16();
        auto maxStack = reader.read_uint16();
        auto codeSize = reader.read_uint32();
        auto localVarSigTok = reader.read_uint32();

        // Check if there are local variable signatures present.
        if (localVarSigTok != 0) {
            auto localTable = static_cast<CLIMetadataTableItem>(localVarSigTok >> 24);
            if (localTable != CLIMetadataTableItem::StandAloneSig) {
                throw runtime_error("Invalid localVarSigTok value.");
            }

            methodBody.localVarSigs = cliMetaDataTables._StandAloneSig[(localVarSigTok & 0x00FFFFFF) - 1];
        }

        methodBody.maxStack = maxStack;
        reader.read_bytes(methodBody.data, codeSize);

        if ((flags & _u(bflags::MoreSects)) != 0) {
            reader.seek(reader.tell() + ((codeSize + 3) & ~3) - codeSize);
            auto sectionHeader = reader.read_uint32();
            if ((sectionHeader & _u(eflags::MoreSects)) != 0 || (sectionHeader & _u(eflags::EHTable)) == 0) {
                // Formally, section could be used for any kind of purposes. However, currently it's not used for anything except storing the information about exception blocks.
                throw runtime_error("Section format is not supported");
            } else if ((sectionHeader & _u(eflags::FatFormat)) != 0) {
                // Fat section: 32-bit block and handler offsets, 32-bit block and handler length fields.
                auto count = ((sectionHeader >> 8) - 4) / 24;
                for (uint32_t i = 0; i < count; i++) {
                    ExceptionClause clause;
                    clause.flags = reader.read_uint32();
                    clause.tryOffset = reader.read_uint32();
                    clause.tryLength = reader.read_uint32();
                    clause.handlerOffset = reader.read_uint32();
                    clause.handlerLength = reader.read_uint32();
                    clause.classTokenOrFilterOffset = reader.read_uint32();
                    methodBody.exceptions.push_back(clause);
                }
            } else {
                // Tiny section: 16-bit block and handler offsets, 8-bit block and handler length fields.
                auto count = (((sectionHeader >> 8) & 0xFF) - 4) / 12;
                for (uint32_t i = 0; i < count; i++) {
                    ExceptionClause clause;
                    clause.flags = reader.read_uint16();
                    clause.tryOffset = reader.read_uint16();
                    clause.tryLength = reader.read_uint8();
                    clause.handlerOffset = reader.read_uint16();
                    clause.handlerLength = reader.read_uint8();
                    clause.classTokenOrFilterOffset = reader.read_uint32();
                    methodBody.exceptions.push_back(clause);
                }
            }
        }

        methodBody.initLocals = ((flags & _u(bflags::InitLocals)) != 0);
    } else {
        throw runtime_error("Invalid body format.");
    }
}

size_t AssemblyData::getMethodCount() const {
    return cliMetaDataTables._MethodDef.size();
}

const Guid& AssemblyData::getGUID() const {
    return cliMetaDataTables._Module[0].guid;
}

const u16string& AssemblyData::getName() const {
    return cliMetaDataTables._Assembly[0].name;
} 

const vector<AssemblyRefRow>& AssemblyData::getAssemblyRef() const {
    return cliMetaDataTables._AssemblyRef;
}

const vector<uint16_t>& AssemblyData::getVersion() const {
    return cliMetaDataTables._Assembly[0].version;
}
