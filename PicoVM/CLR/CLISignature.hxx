#ifndef __CLISIGNATURE_HXX__
#define __CLISIGNATURE_HXX__

#include "CLIElementTypes.hxx"
#include <vector>

enum struct CLISignatureFlags : uint32_t {
    SIG_METHOD_DEFAULT = 0x0, // default calling convention
    SIG_METHOD_C = 0x1,       // C calling convention
    SIG_METHOD_STDCALL = 0x2, // Stdcall calling convention
    SIG_METHOD_THISCALL = 0x3, // thiscall  calling convention
    SIG_METHOD_FASTCALL = 0x4, // fastcall calling convention
    SIG_METHOD_VARARG = 0x5,  // vararg calling convention
    SIG_FIELD = 0x6,           // encodes a field
    SIG_LOCAL_SIG = 0x7,       // used for the .locals directive
    SIG_PROPERTY = 0x8,        // used to encode a property
    SIG_GENERIC = 0x10, // used to indicate that the method has one or more generic parameters.
    SIG_HASTHIS = 0x20,  // used to encode the keyword instance in the calling convention
    SIG_EXPLICITTHIS = 0x40, // used to encode the keyword explicit in the calling convention
    SIG_SENTINEL = 0x41 // used to encode the end of required parameters list
};

struct ArrayShape {
    CLIElementType type;
    uint32_t rank;
    std::vector<uint32_t> hiBounds;
    std::vector<uint32_t> loBounds;

    ArrayShape(std::vector<uint32_t>::const_iterator& it);
};


#endif
