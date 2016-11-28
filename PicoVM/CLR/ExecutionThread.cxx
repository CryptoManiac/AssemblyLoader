#include "ExecutionThread.hxx"
#include "AppDomain.hxx"
#include <iomanip>

using namespace std;

ExecutionThread::ExecutionThread(AppDomain* appDomain) : domain(appDomain) {
    evaluationStack = EvaluationStack();
}

bool ExecutionThread::run() {
    bool result = false;
    do {
        auto frame = &callStack[callStack.size() - 1];
        auto clrData = frame->callingAssembly;
        switch (frame->state) {
        case ExecutionState::FrameSetup:
        {
            // frame->prevStackSize = static_cast<uint32_t>(evaluationStack.size());
            switch (frame->methodToken >> 24) {
            case 0x06: // MethodDef
            {
                auto index = (frame->methodToken & 0xFFFFFF) - 1;
                frame->methodDef = &clrData->cliMetaDataTables._MethodDef[index];
                frame->executingAssembly = frame->callingAssembly;
                frame->state = ExecutionState::MethodBodyExecution;
            }
            break;
            case 0x0A: // MemberRef
            {
                auto index = (frame->methodToken & 0xFFFFFF) - 1;
                auto memberRef = clrData->cliMetaDataTables._MemberRef[index];
                auto signature = memberRef.signature;

                switch (memberRef.classRef.second) {
                case CLIMetadataTableItem::TypeRef: // TypeRef
                {
                    auto typeRef = clrData->cliMetaDataTables._TypeRef[memberRef.classRef.first];
                    switch (typeRef.resolutionScope.second) {
                    case CLIMetadataTableItem::AssemblyRef:
                    {
                        auto assemblyRef = clrData->cliMetaDataTables._AssemblyRef[typeRef.resolutionScope.first];
                        try {
                            frame->executingAssembly = domain->getAssembly(assemblyRef.name, assemblyRef.version);
                            frame->state = ExecutionState::AssemblySet;
                        }
                        catch (runtime_error&) {
                            const auto& id = domain->loadAssembly(assemblyRef.name, assemblyRef.version);
                            frame->executingAssembly = domain->getAssembly(id);
                            frame->state = ExecutionState::AssemblySet;
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
            frame->instructionPointer = 0;
            frame->state = ExecutionState::MethodExecution;
            // TODO: decode methodDefSig and localVarSigs 

            if (frame->localVarSigs.size() != 0) {
                // TODO: process local variable signatures
            }

            result = true;
            break;
        case ExecutionState::WaitForAssembly:
            // wait for assembly... do nothing
            break;
        case ExecutionState::AssemblySet:
        case ExecutionState::NativeMethodExecution:
        case ExecutionState::MethodExecution:
        case ExecutionState::Cleanup:
            /*if (frame->locals.size() != 0) {
                // TODO
            }
            if (frame->arguments.size() != 0) {
                // TODO
            }
            */
            callStack.pop_back();
            result = true;
            break;
        case ExecutionState::Undefined:
        default:
            throw runtime_error("NYI");
        }
        if (callStack.size() == 0) break;
    } while (result);

    return result;
}

void ExecutionThread::setup(const Guid& guid) {
    CallStackItem frame;
    frame.appDomain = domain;
    const auto* assembly = domain->getAssembly(guid);
    frame.callingAssembly = frame.executingAssembly = assembly;
    frame.methodToken = assembly->cliHeader.entryPointToken;
    frame.state = ExecutionState::FrameSetup;
    callStack.push_back(frame);
}

shared_ptr<ExecutionThread> ExecutionThread::create(AppDomain* appDomain) {
    auto thread = new ExecutionThread(appDomain);
    return shared_ptr<ExecutionThread>(thread);
}
