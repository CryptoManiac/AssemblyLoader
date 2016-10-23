#include <sstream>
#include <iomanip>

#include "CLIMetadataTableRows.hxx"

using namespace std;

string ModuleRow::str() {
    ostringstream ss;
    ss << "Module(" << endl
       << " generation=" << dec << generation << endl
       << " name=" << string(name.begin(), name.end()) << endl
       << " guid=" << guid << endl
       << ")" << endl;

       return ss.str();
}

