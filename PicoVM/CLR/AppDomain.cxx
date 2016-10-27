#include "AppDomain.hxx"

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

AppDomain& AppDomain::operator=(const AppDomain& other) {
    AppDomain(other).swap(*this);
    return *this;
}

void AppDomain::swap(AppDomain& other) noexcept {
    using std::swap;
    assemblies.swap(other.assemblies);
    threads.swap(other.threads);
}

AppDomain::ExecutionThread::ExecutionThread(AppDomain& appDomain): appDomain(appDomain) {}

AppDomain::ExecutionThread& AppDomain::ExecutionThread::operator=(const AppDomain::ExecutionThread& other) {
    AppDomain::ExecutionThread(other).swap(*this);
    return *this;
}

void AppDomain::ExecutionThread::swap(AppDomain::ExecutionThread& other) noexcept {
    using std::swap;
    swap(appDomain, other.appDomain);
    swap(state, other.state);
    callStack.swap(other.callStack);
    evaluationStack.swap(other.evaluationStack);
}

AppDomain::CallStackItem::CallStackItem(AppDomain& appDomain, AppDomain::ExecutionThread& thread, AssemblyData& assembly, uint32_t methodToken)
         : appDomain(appDomain), thread(thread), callingAssembly(assembly), executingAssembly(assembly), methodToken(methodToken) {}

AppDomain::CallStackItem& AppDomain::CallStackItem::operator=(const AppDomain::CallStackItem& other) {
    AppDomain::CallStackItem(other).swap(*this);
    return *this;
}

void AppDomain::CallStackItem::swap(AppDomain::CallStackItem& other) noexcept {
    using std::swap;
    
    swap(methodToken, other.methodToken);
    swap(methodBody, other.methodBody);
    swap(callingAssembly, other.callingAssembly);
    swap(executingAssembly, other.executingAssembly);
    swap(appDomain, other.appDomain);
    swap(thread, other.thread);
    swap(prevStackLength, other.prevStackLength);
}
