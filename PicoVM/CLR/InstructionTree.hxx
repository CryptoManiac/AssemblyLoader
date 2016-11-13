#ifndef __INSTRUCTIONTREE_CXX_
#define __INSTRUCTIONTREE_CXX_

#include <map>
#include <vector>
#include <cstdint>
#include <mapbox/variant.hpp>

typedef mapbox::util::variant<int32_t, int64_t, double> argument;

enum struct Instruction : uint16_t {
    inop,
};

struct InstructionTree {
    std::map<int32_t, std::pair<Instruction, std::vector<argument> > > tree;
};

#endif
