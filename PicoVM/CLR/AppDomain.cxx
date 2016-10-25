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
}

AppDomain::CallStackItem::CallStackItem(MethodBody& methodBody, AssemblyData& callingAssembly, AssemblyData& executingAssembly)
         : methodBody(methodBody), callingAssembly(callingAssembly), executingAssembly(executingAssembly), appDomain(appDomain), thread(thread) {}

AppDomain::CallStackItem& AppDomain::CallStackItem::operator=(const AppDomain::CallStackItem& other) {
    AppDomain::CallStackItem(other).swap(*this);
    return *this;
}

void AppDomain::CallStackItem::swap(AppDomain::CallStackItem& other) noexcept {
    using std::swap;
    
    swap(methodBody, other.methodBody);
    swap(callingAssembly, other.callingAssembly);
    swap(executingAssembly, other.executingAssembly);
    swap(appDomain, other.appDomain);
    swap(thread, other.thread);
    swap(prevStackLength, other.prevStackLength);
}
