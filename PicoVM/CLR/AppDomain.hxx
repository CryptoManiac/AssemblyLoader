#ifndef __APPDOMAIN_HXX__
#define __APPDOMAIN_HXX__

#include <vector>
#include <cstdint>
#include "EvaluationStackItem.hxx"
#include "AssemblyData.hxx"

struct AppDomain {
    struct CallStackItem;
    enum struct ThreadExecutionState : uint8_t;

    struct ExecutionThread {
        AppDomain& appDomain;
        std::vector<CallStackItem> callStack;
        std::deque<EvaluationStackItem> evaluationStack;
        ThreadExecutionState state = ThreadExecutionState::Undefined;

        ExecutionThread(AppDomain& appDomain);
        ExecutionThread(const ExecutionThread& other) = default;
        ExecutionThread(ExecutionThread&& other) = default;

        ExecutionThread& operator=(const ExecutionThread& other);
        void swap(ExecutionThread& other) noexcept;
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

    AppDomain() = default;
    AppDomain(const AppDomain& other) = default;
    AppDomain(AppDomain&& other) = default;
    AppDomain& operator=(const AppDomain& other);
    void swap(AppDomain& other) noexcept;

    struct CallStackItem {
        uint32_t methodToken;
        AssemblyData& callingAssembly;
        AssemblyData& executingAssembly;
        AppDomain& appDomain;
        ExecutionThread& thread;
        uint32_t prevStackLength = 0;

        CallStackItem(uint32_t methodToken, AssemblyData& callingAssembly, AssemblyData& executingAssembly);
        CallStackItem(const CallStackItem& other) = default;
        CallStackItem(CallStackItem&& other) = default;

        CallStackItem& operator=(const CallStackItem& other);
        void swap(CallStackItem& other) noexcept;

    private:
        MethodBody methodBody;
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
