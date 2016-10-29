#include "AppDomain.hxx"

using namespace std;

const Guid& AppDomain::loadAssembly(AssemblyData& assemblyData) {
    auto result = assemblies.insert(pair<Guid, AssemblyData>(assemblyData.getGUID(), assemblyData));
    if (!result.second) {
        cout << "Assembly " << assemblyData.getGUID() << " already loaded" << endl;  
    }
    return (*result.first).first;
}

AssemblyData& AppDomain::getAssembly(const Guid& guid) {
    auto result = assemblies.find(guid);
    if (result == assemblies.end()) {
        throw runtime_error("No such assembly in this domain");
    }
    return (*result).second;
}

AssemblyData& AppDomain::getAssembly(const u16string& name, const vector<uint16_t>& version) {
    for (auto it = assemblies.begin(); it != assemblies.end(); ++it) {
        if ((*it).second.getName() == name && (*it).second.getVersion() == version) {
            return(*it).second; 
        }
    }
    throw runtime_error("No such assembly in this domain");
}

AppDomain::ExecutionThread& AppDomain::createThread() {
    ExecutionThread thread(*this);
    auto it = threads.insert(threads.begin(), thread);

    return *it;
}

AppDomain& AppDomain::operator=(const AppDomain& other) {
    AppDomain(other).swap(*this);
    return *this;
}

void AppDomain::swap(AppDomain& other) noexcept {
    assemblies.swap(other.assemblies);
    threads.swap(other.threads);
}

AppDomain::ExecutionThread::ExecutionThread(AppDomain& appDomain): appDomain(appDomain) {}

AppDomain::ExecutionThread& AppDomain::ExecutionThread::operator=(const AppDomain::ExecutionThread& other) {
    AppDomain::ExecutionThread(other).swap(*this);
    return *this;
}

void AppDomain::ExecutionThread::setStartupFrame(const Guid& assemblyGuid) {
    AssemblyData& assembly = appDomain.getAssembly(assemblyGuid);
    AppDomain::CallStackItem frame(appDomain, *this, assembly, assembly.cliHeader.entryPointToken);
    callStack.push_back(frame);
}

bool AppDomain::ExecutionThread::run() {
    bool result = false;
    do {
        auto& frame = callStack[callStack.size() - 1];
        auto& clrData = frame.callingAssembly;
        switch(frame.state) {
            case ExecutionState::FrameSetup:
            {
                frame.prevStackSize = evaluationStack.size();
                switch (frame.methodToken >> 24) {
                case 0x06: // MethodDef
                {
                    frame.executingAssembly = frame.callingAssembly;
                    frame.methodBody = clrData.getMethodDef(frame.methodToken).methodBody;
                    frame.state = ExecutionState::MethodBodyExecution;
                }
                break;
                case 0x0A: // MemberRef
                {
                    auto memberRef = clrData.cliMetaDataTables._MemberRef[frame.methodToken & 0xFFFFFF];
                    auto signature = memberRef.signature;

                    switch (memberRef.classRef.second) {
                    case CLIMetadataTableItem::TypeRef: // TypeRef
                    {
                        auto typeRef = clrData.cliMetaDataTables._TypeRef[memberRef.classRef.first];
                        switch (typeRef.resolutionScope.second) {
                        case CLIMetadataTableItem::AssemblyRef:
                        {
                            auto assemblyRef = clrData.cliMetaDataTables._AssemblyRef[typeRef.resolutionScope.first];
                            try {
                                frame.executingAssembly = appDomain.getAssembly(assemblyRef.name, assemblyRef.version);
                                frame.state = ExecutionState::AssemblySet;
                            }
                            catch (runtime_error) {
                                // TODO: load assembly
                            }
                        }
                        break;
                        default:
                            throw runtime_error("Invalid method class assembly ref");
                        }
                    }
                    break;
                    default:
                        throw runtime_error("Invalid method class ref");
                    }
                };
                break;
                case 0x2B: // MethodSpec
                default:
                    throw runtime_error("Invalid method token");
                }
                result = true;
            };
            break;
            case ExecutionState::MethodBodyExecution:
            case ExecutionState::WaitForAssembly:
                // wait for assembly... do nothing
                break;
            case ExecutionState::AssemblySet:
            case ExecutionState::NativeMethodExecution:
            case ExecutionState::MethodExecution:
            case ExecutionState::Cleanup:
                if (frame.locals.size() != 0) {
                    // TODO
                }
                if (frame.arguments.size() != 0) {
                    // TODO
                }
                callStack.pop_back();
                result = true;
                break;
            case ExecutionState::Undefined:
            default:
                throw runtime_error("NYI");
        }
    } while (result);

    return result;
}

void AppDomain::ExecutionThread::swap(AppDomain::ExecutionThread& other) noexcept {
    ::swap(appDomain, other.appDomain);
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
    ::swap(methodToken, other.methodToken);
    ::swap(methodBody, other.methodBody);
    ::swap(callingAssembly, other.callingAssembly);
    ::swap(executingAssembly, other.executingAssembly);
    ::swap(appDomain, other.appDomain);
    ::swap(thread, other.thread);
    ::swap(state, other.state);
    ::swap(prevStackSize, other.prevStackSize);
}
