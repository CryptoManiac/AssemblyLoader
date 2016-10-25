#include "AppDomain.hxx"
#include <iostream>

using namespace std;

const Guid& AppDomain::loadAssembly(AssemblyData& assemblyData) {
    auto result = assemblies.insert(pair<Guid, AssemblyData>(assemblyData.getGUID(), assemblyData));

    if (!result.second) {
        cout << "Assembly " << assemblyData.getGUID() << " already loaded" << endl;  
    }

    // return AssemblyID
    return (*result.first).first;
}

AssemblyData& AppDomain::getAssembly(const Guid& guid) {
    auto result = assemblies.find(guid);
    if (result == assemblies.end()) {
        throw runtime_error("No such assembly in this domain");
    }
    return (*result).second;
}
