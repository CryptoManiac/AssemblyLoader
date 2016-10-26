#ifndef __CLIMETADATA_HXX__
#define __CLIMETADATA_HXX__

#include <vector>
#include <cstdint>

struct CLIMetadata {
    uint32_t cliMetadataOffset; // In memory only

    std::wstring version;
    uint16_t streamsCount = 0;

    struct CLIStream {
        uint32_t offset = 0;
        uint32_t size = 0;
        std::string name;
    };

    std::vector<CLIStream> streams;
    uint32_t getStreamOffset(const std::string& name) const;
};

#endif
