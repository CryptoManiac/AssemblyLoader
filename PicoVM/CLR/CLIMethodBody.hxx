#ifndef __CLIMETHODBODY_HXX__
#define __CLIMETHODBODY_HXX__

#include <cstdint>
#include <vector>
#include <string>

#include "CLIMetadataTableRows.hxx"

enum struct MethodBodyFlags : uint8_t {
    TinyFormat = 0x2, // Method header is Tiny.
    FatFormat = 0x3,  // Method header is Fat.
    MoreSects = 0x8,  // More sections follow after this header.
    InitLocals = 0x10,// Call default constructor on all local variables.
};

struct ExceptionClause {
    uint32_t flags = 0;
    uint32_t tryOffset = 0;
    uint32_t tryLength = 0;
    uint32_t handlerOffset = 0;
    uint32_t handlerLength = 0;
    uint32_t classTokenOrFilterOffset = 0;

    std::string str() const;
};

enum struct ExceptionFlags : uint8_t {
    EHTable = 0x01,   // It's exception handling section
    FatFormat = 0x40, // Section uses Fat format
    MoreSects = 0x80, // More sections to follow
};

enum struct ExceptionClauseFlags : uint16_t {
    ClauseException = 0x0000, // A typed exception clause
    ClauseFilter = 0x0001,    // An exception filter and handler clause
    ClauseFinally = 0x0002,   // A finally clause
    ClauseFault = 0x0004      // Fault clause(finally that is called on the exception only)
};

struct MethodBody {
    MethodDefRow methodDef;
    std::vector<uint8_t> data;
    std::vector<uint32_t> localVarSigs;
    std::vector<ExceptionClause> exceptions;
    uint32_t maxStack = 0;
    bool initLocals = false;

    std::string str(bool fPrintBody=false) const;
    std::string str() const;
};

#endif
