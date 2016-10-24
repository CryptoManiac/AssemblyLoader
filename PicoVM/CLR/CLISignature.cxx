#include "CLISignature.hxx"

using namespace std;

ArrayShape::ArrayShape(vector<uint32_t>::const_iterator& it) {
    type = static_cast<CLIElementType>(*(it++));
    rank = *(it++);

    auto numSizes = *(it++);
    for (uint32_t n = 0; n < numSizes; ++n) {
        hiBounds.push_back(*(it++));
    }

    auto numLoBounds = *(it++);
    for (uint32_t n = 0; n < numLoBounds; ++n) {
        loBounds.push_back(*(it++));
    }
}
