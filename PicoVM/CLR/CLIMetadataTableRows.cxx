#include <sstream>
#include <iomanip>

#include "CLIMetadataTableRows.hxx"

using namespace std;

std::string ModuleRow::toString() {
    ostringstream ss;
    ss << "Module(" << endl
       << " generation=" << dec << generation << endl
       << " name=" << string(name.begin(), name.end()) << endl
       << " guid=" << guid.toString() << endl
       << ")" << endl;

       return ss.str();
}

std::string TypeRefRow::toString() {
    
}
