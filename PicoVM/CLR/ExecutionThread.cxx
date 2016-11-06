#include "ExecutionThread.hxx"
#include "AppDomain.hxx"
#include "AssemblyData.hxx"
#include "CLIMethodBody.hxx"
#include <iomanip>

using namespace std;

ExecutionThread::ExecutionThread(AppDomain* appDomain) : domain(appDomain) {}

bool ExecutionThread::evaluate() {
    map<uint8_t, void*> opcodes = {
        { 0x00, &&inop },
        { 0x01, &&ibreak },
        { 0x02, &&ildarg_0 },
        { 0x03, &&ildarg_1 },
        { 0x04, &&ildarg_2 },
        { 0x05, &&ildarg_3 },
        { 0x06, &&ildloc_0 },
        { 0x07, &&ildloc_1 },
        { 0x08, &&ildloc_2 },
        { 0x09, &&ildloc_3 },
        { 0x0A, &&istloc_0 },
        { 0x0B, &&istloc_1 },
        { 0x0C, &&istloc_2 },
        { 0x0D, &&istloc_3 }

        // TODO
    };

    for (const auto &i : opcodes) {
        cout << hex << setfill('0') << setw(2) << static_cast<uint16_t>(i.first) << " " << reinterpret_cast<size_t>(i.second) << endl;
    }

    return true;

    inop:
        throw runtime_error("NYI");
    ibreak:
        throw runtime_error("NYI");
    ildarg_0:
        throw runtime_error("NYI");
    ildarg_1:
        throw runtime_error("NYI");
    ildarg_2:
        throw runtime_error("NYI");
    ildarg_3:
        throw runtime_error("NYI");

    ildloc_0:
        throw runtime_error("NYI");
    ildloc_1:
        throw runtime_error("NYI");
    ildloc_2:
        throw runtime_error("NYI");
    ildloc_3:
        throw runtime_error("NYI");

    istloc_0:
        throw runtime_error("NYI");
    istloc_1:
        throw runtime_error("NYI");
    istloc_2:
        throw runtime_error("NYI");
    istloc_3:
        throw runtime_error("NYI");
}

bool ExecutionThread::run() {
    bool result = false;
    do {
        auto frame = &callStack[callStack.size() - 1];
        auto clrData = frame->callingAssembly;
        switch (frame->state) {
        case ExecutionState::FrameSetup:
        {
            frame->prevStackSize = static_cast<uint32_t>(evaluationStack.size());
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
            if (frame->locals.size() != 0) {
                // TODO
            }
            if (frame->arguments.size() != 0) {
                // TODO
            }
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
