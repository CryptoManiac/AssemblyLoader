#ifndef __EXECUTIONTHREAD_HXX__
#define __EXECUTIONTHREAD_HXX__

#include <vector>
#include <cstdint>
#include "AppDomain.hxx"

enum struct ExecutionState {
    FrameSetup = 0,
    MethodBodyExecution = 1,
    WaitForAssembly = 2,
    AssemblySetup = 3,
    NativeMethodExecution = 4,
    MethodExecution = 5,
    Cleanup = 6
};

struct ExecutionThread;
struct CallStackItem {
    uint32_t methodEntryPointToken;
    ExecutionThread& thread;
};

struct ExecutionThread {
    AppDomain& appDomain;
    std::vector<CallStackItem> callStack;
    // TODO: calling assembly, execution assembly, evaluation stack, etc.
    ExecutionState state;
};


#endif
