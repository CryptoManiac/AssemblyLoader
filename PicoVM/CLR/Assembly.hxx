#ifndef __ASSEMBLY_HXX__
#define __ASSEMBLY_HXX__

#include "AssemblyData.hxx"

struct Assembly {
    AssemblyData clrData;
    std::vector<uint16_t> name;
};

#endif