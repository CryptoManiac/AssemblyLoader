#ifndef __CLIMETADATA_HXX__
#define __CLIMETADATA_HXX__

#include <vector>
#include <cstdint>
#include <string>

struct CLIMetadata {
    struct CLIStream {
        std::string name;
        uint32_t offset = 0;
        uint32_t size = 0;
    };
    std::u16string version;
    std::vector<CLIStream> streams;

    uint32_t cliMetadataOffset; // In memory only
    uint16_t streamsCount = 0;

    uint32_t getStreamOffset(const std::string& name) const;

    ~CLIMetadata() noexcept;
};

#endif
