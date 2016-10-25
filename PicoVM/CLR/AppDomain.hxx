#ifndef __APPDOMAIN_HXX__
#define __APPDOMAIN_HXX__

#include <vector>
#include <cstdint>
#include "Assembly.hxx"

struct AppDomain {
    struct CallStackItem;
    enum struct ThreadExecutionState;

    struct ExecutionThread {
        AppDomain& appDomain;
        std::vector<CallStackItem> callStack;
        ThreadExecutionState state = ThreadExecutionState::Undefined;

        ExecutionThread(AppDomain& appDomain) : appDomain(appDomain) {}
    };

    struct CallStackItem {
        const MethodBody& methodBody;
        Assembly& callingAssembly;
        Assembly& executingAssembly;
        AppDomain& appDomain;
        ExecutionThread& thread;

        CallStackItem(const MethodBody& methodBody, Assembly& callingAssembly, Assembly& executingAssembly)
         : methodBody(methodBody), callingAssembly(callingAssembly), executingAssembly(executingAssembly), appDomain(appDomain), thread(thread) {}
    };

    std::vector<Assembly> assemblies;
    std::vector<ExecutionThread> threads;

    enum struct ThreadExecutionState {
        FrameSetup = 0,
        MethodBodyExecution = 1,
        WaitForAssembly = 2,
        AssemblySetup = 3,
        NativeMethodExecution = 4,
        MethodExecution = 5,
        Cleanup = 6,
        Undefined = 7
    };
};

#endif
