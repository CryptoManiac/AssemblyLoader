#ifndef __APPDOMAIN_HXX__
#define __APPDOMAIN_HXX__

#include <vector>
#include <cstdint>
#include "AssemblyData.hxx"

struct AppDomain {
    struct CallStackItem;
    enum struct ThreadExecutionState : uint8_t;

    struct ExecutionThread {
        AppDomain& appDomain;
        std::vector<CallStackItem> callStack;
        ThreadExecutionState state = ThreadExecutionState::Undefined;

        ExecutionThread(AppDomain& appDomain) : appDomain(appDomain) {}
    };

    std::map<Guid, AssemblyData> assemblies;
    std::vector<ExecutionThread> threads;

    template<typename T>
    const Guid& loadAssembly(const T& assembly) {
        AssemblyData assemblyData(assembly);
        return loadAssembly(assemblyData);
    }
    
    const Guid& loadAssembly(AssemblyData& assemblyData);
    AssemblyData& getAssembly(const Guid& guid);

    struct CallStackItem {
        const MethodBody& methodBody;
        AssemblyData& callingAssembly;
        AssemblyData& executingAssembly;
        AppDomain& appDomain;
        ExecutionThread& thread;

        CallStackItem(const MethodBody& methodBody, AssemblyData& callingAssembly, AssemblyData& executingAssembly)
         : methodBody(methodBody), callingAssembly(callingAssembly), executingAssembly(executingAssembly), appDomain(appDomain), thread(thread) {}
    };

    enum struct ThreadExecutionState : uint8_t {
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
