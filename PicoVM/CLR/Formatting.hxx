#ifndef FORMATTING_HXX
#define FORMATTING_HXX
#include <string>

inline std::string operator "" _s(const char * str, size_t len) {
    return std::string(str, str + len);
}

#endif