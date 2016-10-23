#include "CLIMetadataTableIndex.hxx"
#include <algorithm>

using namespace std;

string getTableName(const CLIMetadataTableItem& value)
{
    auto it = cliMetadataTableNames.find(value);
    if (it == cliMetadataTableNames.cend()) {
        throw runtime_error("Incorrect table index");
    }

    return (*it).second;
}
