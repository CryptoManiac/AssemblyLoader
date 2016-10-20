#include "CliMetadataTableIndex.hxx"
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

CliMetadataTableIndex& operator++(CliMetadataTableIndex& value)
{
    const vector<CliMetadataTableIndex> tables = {
        /* 00 */ Module,                 /* 01 */ TypeRef,                /* 02  */ TypeDef,
        /* 04 */ Field,                  /* 06 */ MethodDef,              /* 08  */ Param,
        /* 09 */ InterfaceImpl,          /* 10 */ MemberRef,              /* 11  */ Constant,
        /* 12 */ CustomAttribute,        /* 13 */ FieldMarshal,           /* 14  */ DeclSecurity,
        /* 15 */ ClassLayout,            /* 16 */ FieldLayout,            /* 17  */ StandAloneSig,
        /* 18 */ EventMap,               /* 20 */ Event,                  /* 21  */ PropertyMap,
        /* 23 */ Property,               /* 24 */ MethodSemantics,        /* 25  */ MethodImpl,
        /* 26 */ ModuleRef,              /* 27 */ TypeSpec,               /* 28  */ ImplMap,
        /* 29 */ FieldRVA,               /* 32 */ Assembly,               /* 33  */ AssemblyProcessor,
        /* 34 */ AssemblyOS,             /* 35 */ AssemblyRef,            /* 36  */ AssemblyRefProcessor,
        /* 37 */ AssemblyRefOS,          /* 38 */ File,                   /* 39  */ ExportedType,
        /* 40 */ ManifestResource,       /* 41 */ NestedClass,            /* 42  */ GenericParam,
        /* 43 */ MethodSpec,             /* 44 */ GenericParamConstraint, /* 256 */ Unknown
    };

    auto it = std::find(tables.cbegin(), tables.cend(), value);
    if (*it == Unknown || it == tables.cend()) {
        // Overflow
        return value = Module;
    }

    return value = *next(it, 1);
}

string getTableName(const CliMetadataTableIndex& value) {
    const map<CliMetadataTableIndex, string> tables = {
        { Module, "Module"},
        { TypeRef, "TypeRef"},
        { TypeDef, "TypeDef"},
        { Field, "Field"},
        { MethodDef, "MethodDef"},
        { Param, "Param"},
        { InterfaceImpl, "InterfaceImpl"},
        { MemberRef, "MemberRef"},
        { Constant, "Constant"},
        { CustomAttribute, "CustomAttribute"},
        { FieldMarshal, "FieldMarshal"},
        { DeclSecurity, "DeclSecurity"},
        { ClassLayout, "ClassLayout"},
        { FieldLayout, "FieldLayout"},
        { StandAloneSig, "StandAloneSig"},
        { EventMap, "EventMap"},
        { Event, "Event"},
        { PropertyMap, "PropertyMap"},
        { Property, "Property"},
        { MethodSemantics, "MethodSemantics"},
        { MethodImpl, "MethodImpl"},
        { ModuleRef, "ModuleRef"},
        { TypeSpec, "TypeSpec"},
        { ImplMap, "ImplMap"},
        { FieldRVA, "FieldRVA"},
        { Assembly, "Assembly"},
        { AssemblyProcessor, "AssemblyProcessor"},
        { AssemblyOS, "AssemblyOS"},
        { AssemblyRef, "AssemblyRef"},
        { AssemblyRefProcessor, "AssemblyRefProcessor"},
        { AssemblyRefOS, "AssemblyRefOS"},
        { File, "File"},
        { ExportedType, "ExportedType"},
        { ManifestResource, "ManifestResource"},
        { NestedClass, "NestedClass"},
        { GenericParam, "GenericParam"},
        { MethodSpec, "MethodSpec"},
        { GenericParamConstraint, "GenericParamConstraint" },
        { Unknown, "Unknown"}
    };

    auto it = tables.find(value);
    if (it == tables.cend()) {
        throw runtime_error("Incorrect table index");
    }

    return (*it).second;
}