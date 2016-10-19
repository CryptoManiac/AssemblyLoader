#ifndef __ASSEMBLYDATA_HXX__
#define __ASSEMBLYDATA_HXX__
#include <cinttypes>
#include <vector>
#include <algorithm>

#include "AssemblyReader.hxx"
#include "Property.hxx"
#include "CLIMetaTableRows.hxx"

class AssemblyData {
public:
    // Image header
	ImageFileHeader fileHeader;

    // Sections header
	std::vector<ImageSectionHeader> sections;

    // CLR header
    CLIHeader cliHeader;

    struct CLIMetaData {
        uint32_t cliMetadataOffset; // In memory only

        std::vector<uint16_t> version;
		uint16_t streamsCount;

        struct CLIStream {
			uint32_t offset;
			uint32_t size;
            std::vector<uint8_t> name;
        };

        std::vector<CLIStream> streams;
        uint32_t getStreamOffset(const std::vector<uint8_t>& name) const;
    } cliMetadata;

    struct CLIMetaDataTables {
        ModuleRow module;

    } cliMetaDataTables;

	AssemblyData(std::string strFilePathName);
	AssemblyData(const std::vector<uint8_t>& assembly_bytes);
	uint32_t getDataOffset(uint32_t address) const;

private:
	AssemblyReader reader;
	void InitAssembly(); // called from constructor
    void FillTables();
};

#endif
