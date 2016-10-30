#include "AppDomain.hxx"

using namespace std;

const Guid& AppDomain::loadAssembly(const AssemblyData* assembly) {
    shared_ptr<const AssemblyData> assemblyPtr(assembly);

    auto result = assemblies.insert(pair<Guid, shared_ptr<const AssemblyData> >(assemblyPtr->getGUID(), assemblyPtr));
    if (!result.second) {
        cout << "Assembly " << assemblyPtr->getGUID() << " already loaded" << endl;
    }
    return (*result.first).first;
}

const AssemblyData* AppDomain::getAssembly(const Guid& guid) const {
    auto result = assemblies.find(guid);
    if (result == assemblies.end()) {
        throw runtime_error("No such assembly in this domain");
    }
    return (*result).second.get();
}

const AssemblyData* AppDomain::getAssembly(const u16string& name, const vector<uint16_t>& version) const {
    for (auto i : assemblies) {
        if (i.second->getName() == name && i.second->getVersion() == version) {
            return i.second.get();
        }
    }
    throw runtime_error("No such assembly in this domain");
}

ExecutionThread* AppDomain::createThread() {
    auto thread = ExecutionThread::create(this);
    threads.insert(threads.begin(), thread);
    return thread.get();
}
