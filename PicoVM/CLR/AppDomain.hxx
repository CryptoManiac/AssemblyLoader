#ifndef __APPDOMAIN_HXX__
#define __APPDOMAIN_HXX__

#include <map>
#include <vector>
#include <cstdint>
#include "AssemblyData.hxx"
#include "ExecutionThread.hxx"

struct AppDomain {
    std::map<std::vector<uint16_t>, AssemblyData> assemblies;
    std::vector<ExecutionThread> threads;
};

#endif
