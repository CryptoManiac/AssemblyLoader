#ifndef __EXECUTIONTHREAD_HXX__
#define __EXECUTIONTHREAD_HXX__

#include <cstdint>
#include <deque>
#include <memory>

#include "crossguid/guid.hxx"
#include "EvaluationStackItem.hxx"

enum struct ExecutionState : uint8_t {
    FrameSetup = 0,
    MethodBodyExecution = 1,
    WaitForAssembly = 2,
    AssemblySet = 3,
    NativeMethodExecution = 4,
    MethodExecution = 5,
    Cleanup = 6,
    Undefined = 7
};

struct AppDomain; // forward declaration
struct MethodDefRow;
class AssemblyData;
struct ExecutionThread;

struct CallStackItem {
    AppDomain* appDomain = nullptr;
    ExecutionThread* thread = nullptr;
    const AssemblyData* callingAssembly = nullptr;
    const AssemblyData* executingAssembly = nullptr;
    const MethodDefRow* methodDef = nullptr;

    uint32_t methodToken = 0;
    uint32_t prevStackSize = 0;
    uint32_t instructionPointer = 0;
    uint32_t argumentsCount = 0;
    
    std::vector<EvaluationStackItem> locals;
    std::vector<EvaluationStackItem> arguments;
    std::vector<uint32_t> localVarSigs;
    std::vector<uint32_t> methodDefSig;
    ExecutionState state = ExecutionState::Undefined;
};


struct ExecutionThread {
    AppDomain* domain = nullptr;
    std::deque<CallStackItem> callStack;
    std::deque<EvaluationStackItem> evaluationStack;

    bool run();
    bool evaluate();
    void setup(const Guid& guid);
    static std::shared_ptr<ExecutionThread> create(AppDomain* appDomain);

private:
    ExecutionThread(AppDomain* appDomain);
};


#endif // !__EXECUTIONTHREAD_HXX__
