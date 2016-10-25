#ifndef __APPDOMAIN_HXX__
#define __APPDOMAIN_HXX__

#include <vector>
#include <cstdint>
#include "AssemblyData.hxx"

struct AppDomain {
    struct CallStackItem;
    enum struct ThreadExecutionState : uint8_t;
    typedef std::pair<const Guid&, const std::vector<uint16_t>& > AssemblyID;

    struct ExecutionThread {
        AppDomain& appDomain;
        std::vector<CallStackItem> callStack;
        ThreadExecutionState state = ThreadExecutionState::Undefined;

        ExecutionThread(AppDomain& appDomain) : appDomain(appDomain) {}
    };

    std::map<AssemblyID, AssemblyData> assemblies;
    std::vector<ExecutionThread> threads;

    template<typename T>
    const AssemblyID& loadAssembly(const T& assembly) {
        AssemblyData assemblyData(assembly);
        return loadAssembly(assemblyData);
    }
    
    const AssemblyID& loadAssembly(AssemblyData& assemblyData);
    AssemblyData& getAssembly(const AssemblyID& assemblyID);

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
