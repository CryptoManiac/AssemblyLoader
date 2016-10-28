#ifndef __APPDOMAIN_HXX__
#define __APPDOMAIN_HXX__

#include <vector>
#include <cstdint>
#include "EvaluationStackItem.hxx"
#include "AssemblyData.hxx"

struct AppDomain {

    enum struct ExecutionState : uint8_t {
        FrameSetup = 0,
        MethodBodyExecution = 1,
        WaitForAssembly = 2,
        AssemblySetup = 3,
        NativeMethodExecution = 4,
        MethodExecution = 5,
        Cleanup = 6,
        Undefined = 7
    };

    struct ExecutionThread;

    struct CallStackItem {
        AppDomain& appDomain;
        ExecutionThread& thread;
        AssemblyData& callingAssembly;
        AssemblyData& executingAssembly;
        uint32_t methodToken;
        uint32_t prevStackSize = 0;
        ExecutionState state = ExecutionState::Undefined;

        CallStackItem(AppDomain& appDomain, ExecutionThread& thread, AssemblyData& assembly, uint32_t methodToken);
        CallStackItem(const CallStackItem& other) = default;
        CallStackItem(CallStackItem&& other) = default;

        CallStackItem& operator=(const CallStackItem& other);
        void swap(CallStackItem& other) noexcept;

    private:
        MethodBody methodBody;
    };

    struct ExecutionThread {
        AppDomain& appDomain;
        std::deque<CallStackItem> callStack;
        std::deque<EvaluationStackItem> evaluationStack;

        ExecutionThread(AppDomain& appDomain);
        ExecutionThread(const ExecutionThread& other) = default;
        ExecutionThread(ExecutionThread&& other) = default;

        void setStartupFrame(const Guid& assemblyGuid);
        bool run();

        ExecutionThread& operator=(const ExecutionThread& other);
        void swap(ExecutionThread& other) noexcept;
    };

    std::map<Guid, AssemblyData> assemblies;
    std::deque<ExecutionThread> threads;

    template<typename T>
    const Guid& loadAssembly(const T& assembly) {
        AssemblyData assemblyData(assembly);
        return loadAssembly(assemblyData);
    }
    
    const Guid& loadAssembly(AssemblyData& assemblyData);
    AssemblyData& getAssembly(const Guid& guid);
    AssemblyData& getAssembly(const std::u16string& name, const std::vector<uint16_t>& version);

    ExecutionThread& createThread();

    AppDomain() = default;
    AppDomain(const AppDomain& other) = default;
    AppDomain(AppDomain&& other) = default;
    AppDomain& operator=(const AppDomain& other);
    void swap(AppDomain& other) noexcept;

};

#endif
