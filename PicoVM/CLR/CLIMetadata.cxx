#include <limits>
#include "CLIMetadata.hxx"

using namespace std;

// Get physical offset of metadata stream from the beginning of file.
uint32_t CLIMetadata::getStreamOffset(const vector<uint8_t>& name) const
{
    for (const auto& stream : streams) {
        const auto& streamName = stream.name;
        if (streamName.size() == name.size() && equal(begin(streamName), end(streamName), begin(name))) {
            return cliMetadataOffset + stream.offset;
        }
    }

    return numeric_limits<uint32_t>::max();
}
