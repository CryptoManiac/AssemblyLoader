#include <limits>
#include "CLIMetadata.hxx"

using namespace std;

// Get physical offset of metadata stream from the beginning of file.
uint32_t CLIMetadata::getStreamOffset(const string& name) const
{
    for (const auto& stream : streams) {
        const auto& streamName = stream.name;
        if (streamName.size() == name.size() && equal(begin(streamName), end(streamName), begin(name))) {
            return cliMetadataOffset + stream.offset;
        }
    }

    return numeric_limits<uint32_t>::max();
}

CLIMetadata::~CLIMetadata() noexcept
{
// This routine fix next warning
// PicoVM/CLR/CLIMetadata.hxx:8:8: warning: inlining failed in call to ‘CLIMetadata::~CLIMetadata() noexcept’: call is unlikely and code size would grow [-Winline]
// struct CLIMetadata {
//        ^
// cc1plus: warning: called from here [-Winline]
}
