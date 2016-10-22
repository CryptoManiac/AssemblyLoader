#include <algorithm>
#include "CLIElementTypes.hxx"

using namespace std;

string getTypeName(const CLIElementType &elementType) {
    auto it = cliElementTypeNames.find(elementType);
    if (it == cliElementTypeNames.cend()) {
        throw runtime_error("Incorrect type");
    }

    return (*it).second;
}
