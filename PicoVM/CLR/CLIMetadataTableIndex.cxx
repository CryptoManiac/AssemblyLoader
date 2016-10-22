#include "CLIMetadataTableIndex.hxx"
#include <algorithm>

using namespace std;

CLIMetadataTableItem& operator++(CLIMetadataTableItem& value)
{
    auto it = cliMetadataTableNames.find(value);
    if ((*it).first == CLIMetadataTableItem::Unknown || it == cliMetadataTableNames.cend()) {
        // Overflow
        return value = CLIMetadataTableItem::Module;
    }

    return value = (*next(it, 1)).first;
}

string getTableName(const CLIMetadataTableItem& value)
{
    auto it = cliMetadataTableNames.find(value);
    if (it == cliMetadataTableNames.cend()) {
        throw runtime_error("Incorrect table index");
    }

    return (*it).second;
}
