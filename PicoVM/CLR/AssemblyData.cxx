#include <fstream>
#include <iterator>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>
#include <limits>

#include "AssemblyData.hxx"
#include "Formatting.hxx"

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
    uint32_t peOffset = reader.read_uint32(0x3c);
	if (reader.read_uint32(peOffset) != 0x4550) {
        // PE header should begin with PE\0\0 magic value.
		throw runtime_error("Invalid PE header");
	}

    // PE standard identifier
	uint16_t standard = reader.read_uint16(peOffset + 24);
	uint32_t cliHeaderRVA = 0;

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
            // CLI header address
            cliHeaderRVA = header.optionalHeader.nt.directories[ImageDirectoryType::cliHeader].rva;
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
            // CLI header address
			cliHeaderRVA = header.optionalHeader.nt.directories[ImageDirectoryType::cliHeader].rva;
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
        uint32_t cliHeaderOffset = getDataOffset(cliHeaderRVA);
        if (cliHeaderOffset == numeric_limits<uint32_t>::max()) {
            throw runtime_error("CLI header could not be found.");
        }

        if (reader.read_uint32(cliHeaderOffset) != sizeof(CLIHeader)) {
            // CLI header size must be identical to one which is defined by CLIHeader structure.
            throw runtime_error("CLI header size is invalid.");
        }

        // Load CLI header
        reader.read_cliheader(cliHeader, cliHeaderOffset);

        if ((cliHeader.flags & CLIHeaderFlags::nativeEntryPoint) != 0) {
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
    uint32_t versionLength = reader.read_uint32(cliMetadata.cliMetadataOffset + 12);
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

    const auto heapSizes = reader[metaHeaderOffset + 6];
    const auto valid = reader.read_uint64(metaHeaderOffset + 8);
    const auto sorted = reader.read_uint64(metaHeaderOffset + 16);

    auto& r = reader; // reader can't be captured directly, so make a local reference
    
    // Read 16 or 32 bit index and get the utf8 string at this index.
    auto readString = [&r, heapSizes, stringStreamOffset](vector<uint16_t>& result) {
        uint32_t offset = (heapSizes & 0x01) != 0 ? r.read_uint32() : r.read_uint16();
        r.read_utf8z(result, stringStreamOffset + offset, 0xffff);
    };

    // Read 16 or 32 bit index, fill result by unique ID from this index.
    auto readGuid = [&r, heapSizes, guidStreamOffset](vector<uint8_t>& result) {
        uint32_t index = (heapSizes & 0x02) != 0 ? r.read_uint32() : r.read_uint16();
        if (index != 0) {
            r.read_guid(result, guidStreamOffset + ((index - 1) << 4));
        }
        // If index is zero then do nothing.
    };

    // Read 16 or 32 bit index, and fill the vector by binary data at this index.
    auto readBlob = [&r, heapSizes, blobStreamOffset](vector<uint8_t>& result) {
        uint32_t index = (heapSizes & 0x04) != 0 ? r.read_uint32() : r.read_uint16();
        uint32_t offset = blobStreamOffset + index;
        uint32_t length;
        // Get length of the following data stream
        ptrdiff_t read = r.read_varsize(length, offset);
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
            ptrdiff_t read = AssemblyReader::read_varsize(value, buffer, offset);
            result.push_back(value);
            offset += read;
        }
    };

    auto metaDataOffset = metaHeaderOffset + 24;
    reader.seek(metaDataOffset);

    map<CliMetadataTableIndex, uint32_t> mapTableLength;

    for (CliMetadataTableIndex bit = Module; bit <= GenericParamConstraint; ++bit) {
        bool isSet = ((valid >> bit) & 1) != 0;
        if (isSet) {
            // Load table length record for existent and valid table.
            mapTableLength[bit] = reader.read_uint32();
            cout << getTableName(bit) << " " << dec << mapTableLength[bit] << endl;
        }
        else {
            mapTableLength[bit] = 0;
            cout << getTableName(bit) << " " << dec << mapTableLength[bit] << endl;
        }
    }

    // Read row index.
    auto readRowIndex = [&r, &mapTableLength](CliMetadataTableIndex tableIndex)->uint32_t {
        // Using 32 bit addresses if table has more than 0xffff rows.
        return mapTableLength[tableIndex] >= 0xffff ? r.read_uint32() : r.read_uint16();
    };

    auto readRowIndexChoice = [&r, &mapTableLength](const vector<CliMetadataTableIndex>& tables)->pair<uint32_t, CliMetadataTableIndex> {
        uint32_t max = 0;

        for (const auto& tableID : tables) {
            if (tableID != Unknown && mapTableLength.find(tableID) != mapTableLength.end() && max < mapTableLength[tableID]) {
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
        if (mapTableLength[Module] != 1) {
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
        vector<uint8_t> tmp;
        readGuid(tmp); // encId 
        readGuid(tmp); // endBaseId 
    }

    {
        // TypeRef
        const vector<CliMetadataTableIndex> scope = { Module, ModuleRef, AssemblyRef, TypeRef };
        for (uint32_t n = 0; n < mapTableLength[TypeRef]; ++n) {
            TypeRefRow row;

            // ResolutionScope coded index
            row.resolutionScope = readRowIndexChoice(scope);

            // Type name and namespace
            readString(row.typeName);
            readString(row.typeNamespace);
            cliMetaDataTables._TypeRef.push_back(row);
        }
    }

    {
        // TypeDef
        const vector<CliMetadataTableIndex> scope = { TypeDef, TypeRef, TypeSpec };
        for (uint32_t n = 0; n < mapTableLength[TypeDef]; ++n) {
            TypeDefRow row;
            // 4-byte bit mask of type TypeAttributes
            row.flags = reader.read_uint32();
            // Type name and namespace
            readString(row.typeName);
            readString(row.typeNamespace);
            // TypeDefOrRef coded index into TypeDef, TypeRef or TypeSpec
            row.extendsType = readRowIndexChoice(scope);
            // Index into Field table
            row.fieldList = readRowIndex(Field);
            // Index into MethodDef table
            row.methodList = readRowIndex(MethodDef);
            cliMetaDataTables._TypeDef.push_back(row);
        }
    }

    // Field
    for (uint32_t n = 0; n < mapTableLength[Field]; ++n) {
        FieldRow row;
        // 2-byte bit mask of type FieldAttributes
        row.flags = reader.read_uint16();
        readString(row.name);
        readSignature(row.signature);
        cliMetaDataTables._Field.push_back(row);
    }

    // MethodDef
    for (uint32_t n = 0; n < mapTableLength[MethodDef]; ++n) {
        MethodDefRow row;
        row.rva = reader.read_uint32();
        // 2-byte bit mask of type MethodImplAttributes
        row.implFlags = reader.read_uint16();
        row.flags = reader.read_uint16();
        readString(row.name);
        readSignature(row.signature);
        // Index into the Param table
        row.paramList = readRowIndex(Param);
        cliMetaDataTables._MethodDef.push_back(row);
    }

    // Param
    for (uint32_t n = 0; n < mapTableLength[Param]; ++n) {
        ParamRow row;
        // 2-byte bit mask of type ParamAttributes
        row.flags = reader.read_uint16();
        row.sequence = reader.read_uint16();
        readString(row.name);
        cliMetaDataTables._Param.push_back(row);
    }

    {
        // InterfaceImpl
        const vector<CliMetadataTableIndex> scope = { TypeDef, TypeRef, TypeSpec };
        for (uint32_t n = 0; n < mapTableLength[InterfaceImpl]; ++n) {
            InterfaceImplRow row;
            // Index into the TypeDef table
            row.classRef = readRowIndex(TypeDef);
            // TypeDefOrRef index into TypeDef, TypeRef or TypeSpec
            row.interfaceRef = readRowIndexChoice(scope);
            cliMetaDataTables._InterfaceImpl.push_back(row);
        }
    }

    {
        // MemberRef
        const vector<CliMetadataTableIndex> scope = { TypeDef, TypeRef, ModuleRef, MethodDef, TypeSpec };
        for (uint32_t n = 0; n < mapTableLength[MemberRef]; ++n) {
            MemberRefRow row;
            // MemberRefParent index into the TypeRef, ModuleRef, MethodDef, TypeSpec, or TypeDef tables
            row.classRef = readRowIndexChoice(scope);
            readString(row.name);
            readSignature(row.signature);
            cliMetaDataTables._MemberRef.push_back(row);
        }
    }

    {
        // Constant
        const vector<CliMetadataTableIndex> scope = { Field, Param, Property };
        for (uint32_t n = 0; n < mapTableLength[Constant]; ++n) {
            ConstantRow row;
            row.type = reader.read_uint16();
            // HasConstant index into the Param or Field or Property table
            row.parent = readRowIndexChoice(scope);
            readBlob(row.value);
            cliMetaDataTables._Constant.push_back(row);
        }
    }

    {
        // CustomAttribute
        const vector<CliMetadataTableIndex> parent = {
            MethodDef, Field, TypeRef, TypeDef, Param, InterfaceImpl, MemberRef, Module, Unknown /* FIXME: ??? was Permission */,
            Property, Event, StandAloneSig, ModuleRef, TypeSpec, Assembly, AssemblyRef, File, ExportedType, ManifestResource
        };
        vector<CliMetadataTableIndex> type = { Unknown, Unknown, MethodDef, MemberRef, Unknown };
        for (uint32_t n = 0; n < mapTableLength[CustomAttribute]; ++n) {
            CustomAttributeRow row;
            // HasCustomAttribute index
            row.parent = readRowIndexChoice(parent);
            // CustomAttributeType index
            row.type = readRowIndexChoice(type);
            readBlob(row.value);
            cliMetaDataTables._CustomAttribute.push_back(row);
        };
    }

    {
        // FieldMarshal
        const vector<CliMetadataTableIndex> parent = { Field, Param };
        for (uint32_t n = 0; n < mapTableLength[FieldMarshal]; ++n) {
            FieldMarshalRow row;
            // HasFieldMarshal index
            row.parent = readRowIndexChoice(parent);
            readBlob(row.nativeType);
            cliMetaDataTables._FieldMarshal.push_back(row);
        }
    }

    // DeclSecurity
    const vector<CliMetadataTableIndex> parent = { TypeDef, MethodDef, Assembly };
    for (uint32_t n = 0; n < mapTableLength[DeclSecurity]; ++n) {
        DeclSecurityRow row;
        row.action = reader.read_uint16();
        row.parent = readRowIndexChoice(parent);
        readBlob(row.permissionSet);
        cliMetaDataTables._DeclSecurity.push_back(row);
    }

    // ClassLayout
    for (uint32_t n = 0; n < mapTableLength[DeclSecurity]; ++n) {
        ClassLayoutRow row;
        row.packingSize = reader.read_uint16();
        row.classSize = reader.read_uint32();
        row.parent = readRowIndex(TypeDef);
        cliMetaDataTables._ClassLayout.push_back(row);
    }

    // FieldLayout
    for (uint32_t n = 0; n < mapTableLength[FieldLayout]; ++n) {
        FieldLayoutRow row;
        row.offset = reader.read_uint32();
        row.parent = readRowIndex(Field);
        cliMetaDataTables._FieldLayout.push_back(row);
    }

    // StandAloneSig
    // Each row represents a signature that isn't referenced by any other table. 
    for (uint32_t n = 0; n < mapTableLength[StandAloneSig]; ++n) {
        vector<uint32_t> signature;
        readSignature(signature);
        cliMetaDataTables._StandAloneSig.push_back(signature);
    }

    // EventMap
    for (uint32_t n = 0; n < mapTableLength[EventMap]; ++n) {
        EventMapRow row;
        row.parent = readRowIndex(TypeDef);
        row.eventList = readRowIndex(Event);
        cliMetaDataTables._EventMap.push_back(row);
    }

    {
        // Event
        const vector<CliMetadataTableIndex> scope = { TypeDef, TypeRef, TypeSpec };
        for (uint32_t n = 0; n < mapTableLength[Event]; ++n) {
            EventRow row;
            // 2-byte bit mask of type EventAttribute
            row.eventFlags = reader.read_uint16();
            readString(row.name);
            // TypeDefOrRef index
            row.eventType = readRowIndexChoice(scope);
            cliMetaDataTables._Event.push_back(row);
        }
    }

    // PropertyMap
    for (uint32_t n = 0; n < mapTableLength[PropertyMap]; ++n) {
        PropertyMapRow row;
        row.parent = readRowIndex(TypeDef);
        row.propertyList = readRowIndex(Property);
        cliMetaDataTables._PropertyMap.push_back(row);
    }

    // Property
    for (uint32_t n = 0; n < mapTableLength[Property]; ++n) {
        PropertyRow row;
        // 2-byte bit mask of type PropertyAttributes
        row.flags = reader.read_uint16();
        readString(row.name);
        // A signature from the Blob heap
        readSignature(row.signature);
        cliMetaDataTables._Property.push_back(row);
    }

    {
        // MethodSemantics
        const vector<CliMetadataTableIndex> scope = { Event, Property };
        for (uint32_t n = 0; n < mapTableLength[MethodSemantics]; ++n) {
            MethodSemanticsRow row;
            // 2-byte bit mask of type MethodSemanticsAttributes
            row.semantics = reader.read_uint16();
            // Index into the MethodDef table
            row.method = readRowIndex(MethodDef);
            // HasSemantics index into the Event or Property table
            row.association = readRowIndexChoice(scope);
            cliMetaDataTables._MethodSemantics.push_back(row);
        }
    }

    {
        // MethodImpl
        const vector<CliMetadataTableIndex> body = { MethodDef, MemberRef };
        const vector<CliMetadataTableIndex> declaration = { MethodDef, MemberRef };
        for (uint32_t n = 0; n < mapTableLength[MethodImpl]; ++n) {
            MethodImplRow row;
            // Index into TypeDef table
            row.classRef = readRowIndex(TypeDef);
            // Index into MethodDef or MemberRef table
            row.methodBody = readRowIndexChoice(body);
            row.methodDeclaration = readRowIndexChoice(declaration);
            // ^ MethodDefOrRef
            cliMetaDataTables._MethodImpl.push_back(row);
        }
    }

    // ModuleRef
    for (uint32_t n = 0; n < mapTableLength[ModuleRef]; ++n) {
        vector<uint16_t> name;
        readString(name);
        cliMetaDataTables._ModuleRef.push_back(name);
    }

    // TypeSpec
    for (uint32_t n = 0; n < mapTableLength[TypeSpec]; ++n) {
        vector<uint32_t> signature;
        readSignature(signature);
        cliMetaDataTables._TypeSpec.push_back(signature);
    }

    {
        // ImplMap
        const vector<CliMetadataTableIndex> scope = { Field, MethodDef };
        for (uint32_t n = 0; n < mapTableLength[ImplMap]; ++n) {
            ImplMapRow row;
            // 2-byte bit mask of type PInvokeAttributes
            row.mappingFlags = reader.read_uint16();
            // MemberForwarded  index into the Field or MethodDef table
            row.memberForwarded = readRowIndexChoice(scope);
            readString(row.importName);
            row.importScope = readRowIndex(ModuleRef);
            cliMetaDataTables._ImplMap.push_back(row);
        }
    }

    // FieldRVA
    // The RVA in this table gives the location of the initial value for a Field.
    for (uint32_t n = 0; n < mapTableLength[FieldRVA]; ++n) {
        FieldRVARow row;
        row.rva = reader.read_uint32();
        // Index into Field table
        row.field = readRowIndex(Field);
        cliMetaDataTables._FieldRVA.push_back(row);
    }

    // Assembly
    for (uint32_t n = 0; n < mapTableLength[Assembly]; ++n) {
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
    for (uint32_t n = 0; n < mapTableLength[AssemblyProcessor]; ++n) {
        cliMetaDataTables._AssemblyProcessor.push_back(reader.read_uint32());
    }

    // AssemblyOS
    for (uint32_t n = 0; n < mapTableLength[AssemblyOS]; ++n) {
        AssemblyOSRow row = {};
        row.osPlatformID = reader.read_uint32();
        row.osMajorVersion = reader.read_uint32();
        row.osMinorVersion = reader.read_uint32();
        cliMetaDataTables._AssemblyOS.push_back(row);
    };

    // AssemblyRef
    for (uint32_t n = 0; n < mapTableLength[AssemblyRef]; ++n) {
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
    for (uint32_t n = 0; n < mapTableLength[AssemblyRefProcessor]; ++n) {
        AssemblyRefProcessorRow row;
        row.processor = reader.read_uint32();
        row.assemblyRef = readRowIndex(AssemblyRef);
        cliMetaDataTables._AssemblyRefProcessor.push_back(row);
    }

    // AssemblyRefOS
    for (uint32_t n = 0; n < mapTableLength[AssemblyRefOS]; ++n) {
        AssemblyRefOSRow row;
        row.osPlatformID = reader.read_uint32();
        row.osMajorVersion = reader.read_uint32();
        row.osMinorVersion = reader.read_uint32();
        row.assemblyRef = readRowIndex(AssemblyRef);
        cliMetaDataTables._AssemblyRefOS.push_back(row);
    }

    // File
    for (uint32_t n = 0; n < mapTableLength[File]; ++n) {
        FileRow row;
        // 4-byte bit mask of type FileAttributes
        row.flags = reader.read_uint32();
        readString(row.name);
        readBlob(row.hashValue);
        cliMetaDataTables._File.push_back(row);
    }

    {
        // ExportedType
        const vector<CliMetadataTableIndex> scope = { File, AssemblyRef /*nl*/, ExportedType };
        for (uint32_t n = 0; n < mapTableLength[ExportedType]; ++n) {
            ExportedTypeRow row;
            // 4-byte bit mask of type TypeAttributes
            row.flags = reader.read_uint32();
            // 4-byte index into a TypeDef table of another module in this Assembly
            row.typeDefId = reader.read_uint32();
            readString(row.typeName);
            readString(row.typeNamespace);
            row.implementation = readRowIndexChoice(scope);
            // ^ Implementation
            cliMetaDataTables._ExportedType.push_back(row);
        }
    }

    {
        // ManifestResource
        const vector<CliMetadataTableIndex> scope = { File, AssemblyRef, ExportedType /*nl*/ };
        for (uint32_t n = 0; n < mapTableLength[ManifestResource]; ++n) {
            ManifestResourceRow row;
            row.offset = reader.read_uint32();
            // 4-byte bit mask of type ManifestResourceAttributes
            row.flags = reader.read_uint32();
            readString(row.name);
            row.implementation = readRowIndexChoice(scope);
            // ^ Implementation
            cliMetaDataTables._ManifestResource.push_back(row);
        }
    }

    // NestedClass
    for (uint32_t n = 0; n < mapTableLength[NestedClass]; ++n) {
        NestedClassRow row;
        row.nestedClass = readRowIndex(TypeDef);
        row.enclosingClass = readRowIndex(TypeDef);
        cliMetaDataTables._NestedClass.push_back(row);
    }

    {
        // GenericParam
        const vector<CliMetadataTableIndex> scope = { TypeDef, MethodDef };
        for (uint32_t n = 0; n < mapTableLength[GenericParam]; ++n) {
            GenericParamRow row;
            // 2-byte index of the generic parameter
            row.number = reader.read_uint16();
            // 2-byte bitmask of type GenericParamAttributes
            row.flags = reader.read_uint16();
            // TypeOrMethodDef index into the TypeDef or MethodDef table
            row.owner = readRowIndexChoice(scope);
            readString(row.name);
            cliMetaDataTables._GenericParam.push_back(row);
        }
    }

    {
        // MethodSpec
        const vector<CliMetadataTableIndex> scope = { MethodDef, MemberRef };
        for (uint32_t n = 0; n < mapTableLength[MethodSpec]; ++n) {
            MethodSpecRow row;
            row.method = readRowIndexChoice(scope);
            // ^ MethodDefOrRef
            readSignature(row.instantiation);
            cliMetaDataTables._MethodSpec.push_back(row);
        }
    }

    {
        // GenericParamConstraint
        const vector<CliMetadataTableIndex> scope = { TypeDef, TypeRef, TypeSpec };
        for (uint32_t n = 0; n < mapTableLength[GenericParamConstraint]; ++n) {
            GenericParamConstraintRow row;
            // Index into the GenericParam table
            row.owner = readRowIndex(GenericParam);
            row.constraint = readRowIndexChoice(scope);
            cliMetaDataTables._GenericParamConstraint.push_back(row);
        }
    }
}

// Get physical offset from the beginning of file.
uint32_t AssemblyData::getDataOffset (uint32_t address) const {
    for (uint32_t n = 0; n < fileHeader.sectionsCount; ++n) {
        if (sections[n].virtualAddress <= address && address < sections[n].virtualAddress + sections[n].virtualSize) {
            return sections[n].pointerToRawData + (address - sections[n].virtualAddress);
        }
    }

    // It looks like we weren't able to find anything
    return std::numeric_limits<uint32_t>::max();
}

uint32_t AssemblyData::CLIMetaData::getStreamOffset(const vector<uint8_t>& name) const {
    for (uint32_t i = 0; i < streamsCount; ++i) {
        const vector<uint8_t>& streamName = streams[i].name;
        if (streamName.size() == name.size() && equal(begin(streamName), end(streamName), begin(name))) {
            return cliMetadataOffset + streams[i].offset;
        }
    }

    return std::numeric_limits<uint32_t>::max();
};

