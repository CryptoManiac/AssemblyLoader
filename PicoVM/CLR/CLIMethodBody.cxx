#include "CLIMethodBody.hxx"
#include "HexStr.hxx"
#include <sstream>

using namespace std;

string ExceptionClause::toString() const {
    stringstream ss;
    ss << "ExceptionClause(tryOffset=" << hex << tryOffset << " tryLength=" << dec << tryLength << " handlerOffset=" << hex << handlerOffset << " handlerLength=" << dec << handlerLength << " classTokenOrFilterOffset=" << hex << classTokenOrFilterOffset << ")";
    return ss.str();
}

std::string MethodBody::toString(bool fPrintBody) const {
    stringstream ss;
    ss << "MethodBody(" << endl
       << " maxStack=" << dec << maxStack << endl
       << " localVarSigTok=" << hex << localVarSigTok << endl
       << " initLocals=" << dec << initLocals << endl;

    if (exceptions.size() != 0) {
        ss << " Exceptions(" << endl;
        for(uint32_t n = 0; n < exceptions.size(); ++n) {
            ss << "   " << exceptions[n].toString() << endl;
        }
        ss << " )" << endl;
    }

    if (fPrintBody) {
        ss << " Body(" << endl 
           << "  " << HexStr(data.cbegin(), data.cend(), true) << endl 
           << " )" << endl; 
    } else {
        ss << " bodySize=" << dec << data.size() << endl; 
    }

    ss << ")";

    return ss.str();
}
