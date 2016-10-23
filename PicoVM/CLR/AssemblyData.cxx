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

AssemblyData::AssemblyData(string strFilePathName) {
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

AssemblyData::AssemblyData(const vector<uint8_t>& assembly_bytes) {
	// Check MZ header
	if (assembly_bytes[0] != 0x4d || assembly_bytes[1] != 0x5a) {
		throw runtime_error("There is no MZ header");
	}

	// Create reader and continue initialization process
	reader = AssemblyReader(assembly_bytes);
	InitAssembly();
}

void AssemblyData::InitAssembly() {
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
		}
		break;
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
		}
		break;
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
        CLIMetaData::CLIStream stream = {};
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

void AssemblyData::FillTables() {
    const auto metaHeaderOffset = cliMetadata.getStreamOffset({ '#', '~' });
    const auto stringStreamOffset = cliMetadata.getStreamOffset({ '#', 'S', 't', 'r', 'i', 'n', 'g', 's' });
    const auto usStreamOffset = cliMetadata.getStreamOffset({ '#', 'U', 'S' });
    const auto guidStreamOffset = cliMetadata.getStreamOffset({ '#', 'G', 'U', 'I', 'D' });
    const auto blobStreamOffset = cliMetadata.getStreamOffset({ '#', 'B', 'l', 'o', 'b' });

    // Encodes how wide indexes into the various heaps are. Default width is 16 bit, indexes can be either 16 or 32 bit long.
    //
    // 0x01 Size of #String stream is 32 bit.
    // 0x02 Size of #GUID stream is 32 bit.
    // 0x04 Size of #Blob stream is 32 bit.
    const auto heapSizes = reader[metaHeaderOffset + 6];

    // A 64-bit number that has a bit set for each table that is present in the assembly.
    const auto valid = reader.read_uint64(metaHeaderOffset + 8);
    // Similarly, tells which tables are sorted. However, I'd say that it doesn't make sanse for me.
    // http://stackoverflow.com/questions/3472752/what-is-a-sorted-table-in-net-metadata 
    const auto sorted = reader.read_uint64(metaHeaderOffset + 16);

    auto& r = reader; // reader can't be captured directly, so make a local reference
    
    // Read 16 or 32 bit index and get the utf8 string at this index.
    auto readString = [&r, heapSizes, stringStreamOffset](vector<uint16_t>& result) {
        uint32_t offset = (heapSizes & 0x01) != 0 ? r.read_uint32() : r.read_uint16();
        r.read_utf8z(result, stringStreamOffset + offset, 0xffff);
    };

    // Read 16 or 32 bit index, fill result by unique ID from this index.
    auto readGuid = [&r, heapSizes, guidStreamOffset](Guid& result) {
        uint32_t index = (heapSizes & 0x02) != 0 ? r.read_uint32() : r.read_uint16();
        if (index != 0) {
            r.read_guid(result, guidStreamOffset + ((index - 1) << 4));
        }
        // If index is zero then do nothing.
    };

    // Read 16 or 32 bit index, and fill the vector by binary data at this index.
    auto readBlob = [&r, heapSizes, blobStreamOffset](vector<uint8_t>& result) {
        uint32_t index = (heapSizes & 0x04) != 0 ? r.read_uint32() : r.read_uint16();
        auto offset = blobStreamOffset + index;
        uint32_t length;
        // Get length of the following data stream
        auto read = r.read_varsize(length, offset);
        // Read binary data into vector
        r.read_bytes(result, offset + read, length);
    };

    // Pop 16 or 32 bit index, read binary data and parse it as a signature.
    auto readSignature = [&r, &readBlob](vector<uint32_t>& result) {
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
    };

    auto metaDataOffset = metaHeaderOffset + 24;
    reader.seek(metaDataOffset);

    map<CLIMetadataTableItem, uint32_t> mapTableLength;

    for(const auto &item : cliMetadataTableNames) {
        auto bit = item.first;
        if (((valid >> _u(bit)) & 1) != 0) {
            // Load table length record for existent and valid table.
            mapTableLength[bit] = reader.read_uint32();
        }
        else {
            mapTableLength[bit] = 0;
        }
    }

    // Read row index.
    auto readRowIndex = [&r, &mapTableLength](CLIMetadataTableItem tableIndex)->uint32_t {
        // Using 32 bit addresses if table has more than 0xffff rows.
        return mapTableLength[tableIndex] >= 0xffff ? r.read_uint32() : r.read_uint16();
    };

    // Decode 
    auto readRowIndexChoice = [&r, &mapTableLength](const vector<CLIMetadataTableItem>& tables)->pair<uint32_t, CLIMetadataTableItem> {
        uint32_t max = 0;

        for (const auto& tableID : tables) {
            if (tableID != CLIMetadataTableItem::Unknown && mapTableLength.find(tableID) != mapTableLength.end() && max < mapTableLength[tableID]) {
                max = mapTableLength[tableID];
            }
        }

        uint32_t shift = 0, bit = 1;
        while (tables.size() > bit) {
            bit <<= 1;
            ++shift;
        }

        uint32_t index = (max << shift) >= 0xffff ? r.read_uint32() : r.read_uint16();

        return{ index, tables[index & (bit - 1)] };
    };

    {
        // Module table
        if (mapTableLength[CLIMetadataTableItem::Module] != 1) {
            throw runtime_error("Module table most contain one and only one row.");
        }

        // Module generation, currently it is set to zero.
        cliMetaDataTables.module.generation = r.read_uint16();

        // Module name.
        readString(cliMetaDataTables.module.name);

        // Unique identifier which is used to distinguish between two versions of the same module.
        readGuid(cliMetaDataTables.module.guid);

        // These two reserved fields are not presented in ModuleRow structure.
        // EncId (index into GUID heap, always set to zero).
        // EncBaseId (index into GUID heap, always set to zero).
        Guid tmp;
        readGuid(tmp); // encId 
        readGuid(tmp); // endBaseId 
    }

    // TypeRef
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::TypeRef]; ++n) {
        TypeRefRow row;

        // ResolutionScope coded index
        row.resolutionScope = readRowIndexChoice(resolutionScope);

        // Type name and namespace
        readString(row.typeName);
        readString(row.typeNamespace);
        cliMetaDataTables._TypeRef.push_back(row);
    }

    // TypeDef
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::TypeDef]; ++n) {
        TypeDefRow row;
        // 4-byte bit mask of type TypeAttributes
        row.flags = reader.read_uint32();
        // Type name and namespace
        readString(row.typeName);
        readString(row.typeNamespace);
        // TypeDefOrRef coded index into TypeDef, TypeRef or TypeSpec
        row.extendsType = readRowIndexChoice(typeDefOrRef);
        // Index into FieldDef table
        row.fieldList = readRowIndex(CLIMetadataTableItem::FieldDef);
        // Index into MethodDef table
        row.methodList = readRowIndex(CLIMetadataTableItem::MethodDef);
        cliMetaDataTables._TypeDef.push_back(row);
    }

    // FieldDef
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::FieldDef]; ++n) {
        FieldDefRow row;
        // 2-byte bit mask of type FieldAttributes
        row.flags = reader.read_uint16();
        readString(row.name);
        readSignature(row.signature);
        cliMetaDataTables._FieldDef.push_back(row);
    }

    // MethodDef
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::MethodDef]; ++n) {
        MethodDefRow row;
        row.rva = reader.read_uint32();
        // 2-byte bit mask of type MethodImplAttributes
        row.implFlags = reader.read_uint16();
        row.flags = reader.read_uint16();
        readString(row.name);
        readSignature(row.signature);
        // Index into the ParamDef table
        row.paramList = readRowIndex(CLIMetadataTableItem::ParamDef);
        cliMetaDataTables._MethodDef.push_back(row);
    }

    // ParamDef
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::ParamDef]; ++n) {
        ParamDefRow row;
        // 2-byte bit mask of type ParamAttributes
        row.flags = reader.read_uint16();
        row.sequence = reader.read_uint16();
        readString(row.name);
        cliMetaDataTables._ParamDef.push_back(row);
    }

    // InterfaceImpl
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::InterfaceImpl]; ++n) {
        InterfaceImplRow row;
        // Index into the TypeDef table
        row.classRef = readRowIndex(CLIMetadataTableItem::TypeDef);
        // TypeDefOrRef index into TypeDef, TypeRef or TypeSpec
        row.interfaceRef = readRowIndexChoice(typeDefOrRef);
        cliMetaDataTables._InterfaceImpl.push_back(row);
    }

    // MemberRef
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::MemberRef]; ++n) {
        MemberRefRow row;
        // MemberRefParent index into the TypeRef, ModuleRef, MethodDef, TypeSpec, or TypeDef tables
        row.classRef = readRowIndexChoice(memberRefParent);
        readString(row.name);
        readSignature(row.signature);
        cliMetaDataTables._MemberRef.push_back(row);
    }

    // Constant
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::Constant]; ++n) {
        ConstantRow row;
        row.type = reader.read_uint16();
        // HasConstant index into the ParamDef or FieldDef or Property table
        row.parent = readRowIndexChoice(hasConstant);
        readBlob(row.value);
        cliMetaDataTables._Constant.push_back(row);
    }

    // CustomAttribute
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::CustomAttribute]; ++n) {
        CustomAttributeRow row;
        // HasCustomAttribute index
        row.parent = readRowIndexChoice(hasCustomAttribute);
        // CustomAttributeType index
        row.type = readRowIndexChoice(customAttributeType);
        readBlob(row.value);
        cliMetaDataTables._CustomAttribute.push_back(row);
    }

    // FieldMarshal
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::FieldMarshal]; ++n) {
        FieldMarshalRow row;
        // HasFieldMarshal index
        row.parent = readRowIndexChoice(hasFieldMarshall);
        readBlob(row.nativeType);
        cliMetaDataTables._FieldMarshal.push_back(row);
    }

    // DeclSecurity    
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::DeclSecurity]; ++n) {
        DeclSecurityRow row;
        row.action = reader.read_uint16();
        row.parent = readRowIndexChoice(hasDeclSecurity);
        readBlob(row.permissionSet);
        cliMetaDataTables._DeclSecurity.push_back(row);
    }

    // ClassLayout
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::ClassLayout]; ++n) {
        ClassLayoutRow row;
        row.packingSize = reader.read_uint16();
        row.classSize = reader.read_uint32();
        row.parent = readRowIndex(CLIMetadataTableItem::TypeDef);
        cliMetaDataTables._ClassLayout.push_back(row);
    }

    // FieldLayout
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::FieldLayout]; ++n) {
        FieldLayoutRow row;
        row.offset = reader.read_uint32();
        row.parent = readRowIndex(CLIMetadataTableItem::FieldDef);
        cliMetaDataTables._FieldLayout.push_back(row);
    }

    // StandAloneSig
    // Each row represents a signature that isn't referenced by any other table. 
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::StandAloneSig]; ++n) {
        vector<uint32_t> signature;
        readSignature(signature);
        cliMetaDataTables._StandAloneSig.push_back(signature);
    }

    // EventMap
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::EventMap]; ++n) {
        EventMapRow row;
        row.parent = readRowIndex(CLIMetadataTableItem::TypeDef);
        row.eventList = readRowIndex(CLIMetadataTableItem::Event);
        cliMetaDataTables._EventMap.push_back(row);
    }

    // Event
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::Event]; ++n) {
        EventRow row;
        // 2-byte bit mask of type EventAttribute
        row.eventFlags = reader.read_uint16();
        readString(row.name);
        // TypeDefOrRef index
        row.eventType = readRowIndexChoice(typeDefOrRef);
        cliMetaDataTables._Event.push_back(row);
    }

    // PropertyMap
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::PropertyMap]; ++n) {
        PropertyMapRow row;
        row.parent = readRowIndex(CLIMetadataTableItem::TypeDef);
        row.propertyList = readRowIndex(CLIMetadataTableItem::Property);
        cliMetaDataTables._PropertyMap.push_back(row);
    }

    // Property
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::Property]; ++n) {
        PropertyRow row;
        // 2-byte bit mask of type PropertyAttributes
        row.flags = reader.read_uint16();
        readString(row.name);
        // A signature from the Blob heap
        readSignature(row.signature);
        cliMetaDataTables._Property.push_back(row);
    }

    // MethodSemantics 
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::MethodSemantics]; ++n) {
        MethodSemanticsRow row;
        // 2-byte bit mask of type MethodSemanticsAttributes
        row.semantics = reader.read_uint16();
        // Index into the MethodDef table
        row.method = readRowIndex(CLIMetadataTableItem::MethodDef);
        // HasSemantics index into the Event or Property table
        row.association = readRowIndexChoice(hasSemantics);
        cliMetaDataTables._MethodSemantics.push_back(row);
    }

    // MethodImpl
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::MethodImpl]; ++n) {
        MethodImplRow row;
        // Index into TypeDef table
        row.classRef = readRowIndex(CLIMetadataTableItem::TypeDef);
        // Index into MethodDef or MemberRef table
        row.methodBody = readRowIndexChoice(methodDefOrRef);
        row.methodDeclaration = readRowIndexChoice(methodDefOrRef);
        cliMetaDataTables._MethodImpl.push_back(row);
    }

    // ModuleRef
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::ModuleRef]; ++n) {
        vector<uint16_t> name;
        readString(name);
        cliMetaDataTables._ModuleRef.push_back(name);
    }

    // TypeSpec
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::TypeSpec]; ++n) {
        vector<uint32_t> signature;
        readSignature(signature);
        cliMetaDataTables._TypeSpec.push_back(signature);
    }

    // ImplMap
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::ImplMap]; ++n) {
        ImplMapRow row;
        // 2-byte bit mask of type PInvokeAttributes
        row.mappingFlags = reader.read_uint16();
        // MemberForwarded  index into the FieldDef or MethodDef table
        row.memberForwarded = readRowIndexChoice(memberForwarded);
        readString(row.importName);
        row.importScope = readRowIndex(CLIMetadataTableItem::ModuleRef);
        cliMetaDataTables._ImplMap.push_back(row);
    }

    // FieldRVA
    // The RVA in this table gives the location of the initial value for a Field.
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::FieldRVA]; ++n) {
        FieldRVARow row;
        row.rva = reader.read_uint32();
        // Index into FieldDef table
        row.field = readRowIndex(CLIMetadataTableItem::FieldDef);
        cliMetaDataTables._FieldRVA.push_back(row);
    }

    // Assembly
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::Assembly]; ++n) {
        AssemblyRow row;
        row.version.clear();

        // 4-byte constant of type AssemblyHashAlgorithm
        row.hashAlgId = reader.read_uint32();
        // MajorVersion
        row.version.push_back(reader.read_uint16());
        // MinorVersion
        row.version.push_back(reader.read_uint16());
        // BuildNumber
        row.version.push_back(reader.read_uint16());
        // RevisionNumber 
        row.version.push_back(reader.read_uint16());
        // 4-byte bit mask of type AssemblyFlags
        row.flags = reader.read_uint32();

        readBlob(row.publicKey);
        readString(row.name);
        readString(row.culture);
        cliMetaDataTables._Assembly.push_back(row);
    }

    // AssemblyProcessor
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::AssemblyProcessor]; ++n) {
        cliMetaDataTables._AssemblyProcessor.push_back(reader.read_uint32());
    }

    // AssemblyOS
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::AssemblyOS]; ++n) {
        AssemblyOSRow row = {};
        row.osPlatformID = reader.read_uint32();
        row.osMajorVersion = reader.read_uint32();
        row.osMinorVersion = reader.read_uint32();
        cliMetaDataTables._AssemblyOS.push_back(row);
    };

    // AssemblyRef
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::AssemblyRef]; ++n) {
        AssemblyRefRow row;
        row.version.clear();

        // MajorVersion
        row.version.push_back(reader.read_uint16());
        // MinorVersion
        row.version.push_back(reader.read_uint16());
        // BuildNumber
        row.version.push_back(reader.read_uint16());
        // RevisionNumber
        row.version.push_back(reader.read_uint16());
        // 4-byte bit mask of type AssemblyFlags
        row.flags = reader.read_uint32();

        readBlob(row.publicKeyOrToken);
        readString(row.name);
        readString(row.culture);
        readBlob(row.hashValue);
        cliMetaDataTables._AssemblyRef.push_back(row);
    }

    // AssemblyRefProcessor
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::AssemblyRefProcessor]; ++n) {
        AssemblyRefProcessorRow row;
        row.processor = reader.read_uint32();
        row.assemblyRef = readRowIndex(CLIMetadataTableItem::AssemblyRef);
        cliMetaDataTables._AssemblyRefProcessor.push_back(row);
    }

    // AssemblyRefOS
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::AssemblyRefOS]; ++n) {
        AssemblyRefOSRow row;
        row.osPlatformID = reader.read_uint32();
        row.osMajorVersion = reader.read_uint32();
        row.osMinorVersion = reader.read_uint32();
        row.assemblyRef = readRowIndex(CLIMetadataTableItem::AssemblyRef);
        cliMetaDataTables._AssemblyRefOS.push_back(row);
    }

    // File
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::File]; ++n) {
        FileRow row;
        // 4-byte bit mask of type FileAttributes
        row.flags = reader.read_uint32();
        readString(row.name);
        readBlob(row.hashValue);
        cliMetaDataTables._File.push_back(row);
    }

    // ExportedType 
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::ExportedType]; ++n) {
        ExportedTypeRow row;
        // 4-byte bit mask of type TypeAttributes
        row.flags = reader.read_uint32();
        // 4-byte index into a TypeDef table of another module in this Assembly
        row.typeDefId = reader.read_uint32();
        readString(row.typeName);
        readString(row.typeNamespace);
        row.implementation = readRowIndexChoice(implementation);
        cliMetaDataTables._ExportedType.push_back(row);
    }

    // ManifestResource
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::ManifestResource]; ++n) {
        ManifestResourceRow row;
        row.offset = reader.read_uint32();
        // 4-byte bit mask of type ManifestResourceAttributes
        row.flags = reader.read_uint32();
        readString(row.name);
        row.implementation = readRowIndexChoice(implementation);
        cliMetaDataTables._ManifestResource.push_back(row);
    }

    // NestedClass
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::NestedClass]; ++n) {
        NestedClassRow row;
        row.nestedClass = readRowIndex(CLIMetadataTableItem::TypeDef);
        row.enclosingClass = readRowIndex(CLIMetadataTableItem::TypeDef);
        cliMetaDataTables._NestedClass.push_back(row);
    }

    // GenericParam
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::GenericParam]; ++n) {
        GenericParamRow row;
        // 2-byte index of the generic parameter
        row.number = reader.read_uint16();
        // 2-byte bitmask of type GenericParamAttributes
        row.flags = reader.read_uint16();
        // TypeOrMethodDef index into the TypeDef or MethodDef table
        row.owner = readRowIndexChoice(typeOrMethodDef);
        readString(row.name);
        cliMetaDataTables._GenericParam.push_back(row);
    }

    // MethodSpec
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::MethodSpec]; ++n) {
        MethodSpecRow row;
        row.method = readRowIndexChoice(methodDefOrRef);
        // ^ MethodDefOrRef
        readSignature(row.instantiation);
        cliMetaDataTables._MethodSpec.push_back(row);
    }

    // GenericParamConstraint
    for (uint32_t n = 0; n < mapTableLength[CLIMetadataTableItem::GenericParamConstraint]; ++n) {
        GenericParamConstraintRow row;
        // Index into the GenericParam table
        row.owner = readRowIndex(CLIMetadataTableItem::GenericParam);
        row.constraint = readRowIndexChoice(typeDefOrRef);
        cliMetaDataTables._GenericParamConstraint.push_back(row);
    }
}

// Get physical offset from the beginning of file.
uint32_t AssemblyData::getDataOffset (uint32_t address) const {
    for (const auto &section : sections) {
        if (section.virtualAddress <= address && address < section.virtualAddress + section.virtualSize) {
            return section.pointerToRawData + (address - section.virtualAddress);
        }
    }

    // It looks like we weren't able to find anything
    return std::numeric_limits<uint32_t>::max();
}

// Get physical offset of metadata stream from the beginning of file.
uint32_t AssemblyData::CLIMetaData::getStreamOffset(const vector<uint8_t>& name) const {
    for (const auto& stream : streams) {
        const auto& streamName = stream.name;
        if (streamName.size() == name.size() && equal(begin(streamName), end(streamName), begin(name))) {
            return cliMetadataOffset + stream.offset;
        }
    }

    return std::numeric_limits<uint32_t>::max();
};

// Get method information 
void AssemblyData::getMethodBody(uint32_t index, MethodBody& methodBody) {
    using bflags = MethodBodyFlags;
    using eflags = ExceptionFlags;

    methodBody.methodDef = cliMetaDataTables._MethodDef[index - 1];
    auto offset = getDataOffset(methodBody.methodDef.rva);
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
            } else if ((sectionHeader  & _u(eflags::FatFormat)) != 0) {
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
