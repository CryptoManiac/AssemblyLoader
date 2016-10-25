#include "AppDomain.hxx"
#include <iostream>

using namespace std;

const AppDomain::AssemblyID& AppDomain::loadAssembly(AssemblyData& assemblyData) {
    auto assemblyID = AssemblyID(assemblyData.getGUID(), assemblyData.getName());
    auto result = assemblies.insert(pair<AssemblyID, AssemblyData>(assemblyID, assemblyData));

    if (!result.second) {
        cout << "Assembly " << assemblyID.first << " already loaded" << endl;  
    }

    // return AssemblyID
    return (*result.first).first;
}

AssemblyData& AppDomain::getAssembly(const AppDomain::AssemblyID& assemblyID) {
    auto result = assemblies.find(assemblyID);
    if (result == assemblies.end()) {
        throw runtime_error("No such assembly in this domain");
    }
    return (*result).second;
}