#ifndef __APPDOMAIN_HXX__
#define __APPDOMAIN_HXX__

#include <cstdint>
#include <vector>
#include <map>
#include <memory>
#include <string>

#include "AssemblyData.hxx"
#include "ExecutionThread.hxx"
#include "crossguid/guid.hxx"

struct AppDomain {
    std::map<Guid, std::shared_ptr<const AssemblyData> > assemblies;
    std::vector<std::shared_ptr<ExecutionThread> > threads;
    std::string assemblyPath = "";

    const Guid& loadAssembly(const AssemblyData* assembly);
    const Guid& loadAssembly(const std::u16string& name, const std::vector<uint16_t>& version);
    const AssemblyData* getAssembly(const Guid& guid) const;
    const AssemblyData* getAssembly(const std::u16string& name, const std::vector<uint16_t>& version) const;
    ExecutionThread* createThread();

    AppDomain(const std::string& searchPath);
};

#endif
