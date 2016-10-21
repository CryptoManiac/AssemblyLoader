#include "CliMetadataTableIndex.hxx"
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

CLIMetadataTableIndex& operator++(CLIMetadataTableIndex& value)
{
    using idx = CLIMetadataTableIndex;
    const vector<idx> tables = {
        /* 00 */ idx::Module,                 /* 01 */ idx::TypeRef,                /* 02  */ idx::TypeDef,
        /* 04 */ idx::Field,                  /* 06 */ idx::MethodDef,              /* 08  */ idx::Param,
        /* 09 */ idx::InterfaceImpl,          /* 10 */ idx::MemberRef,              /* 11  */ idx::Constant,
        /* 12 */ idx::CustomAttribute,        /* 13 */ idx::FieldMarshal,           /* 14  */ idx::DeclSecurity,
        /* 15 */ idx::ClassLayout,            /* 16 */ idx::FieldLayout,            /* 17  */ idx::StandAloneSig,
        /* 18 */ idx::EventMap,               /* 20 */ idx::Event,                  /* 21  */ idx::PropertyMap,
        /* 23 */ idx::Property,               /* 24 */ idx::MethodSemantics,        /* 25  */ idx::MethodImpl,
        /* 26 */ idx::ModuleRef,              /* 27 */ idx::TypeSpec,               /* 28  */ idx::ImplMap,
        /* 29 */ idx::FieldRVA,               /* 32 */ idx::Assembly,               /* 33  */ idx::AssemblyProcessor,
        /* 34 */ idx::AssemblyOS,             /* 35 */ idx::AssemblyRef,            /* 36  */ idx::AssemblyRefProcessor,
        /* 37 */ idx::AssemblyRefOS,          /* 38 */ idx::File,                   /* 39  */ idx::ExportedType,
        /* 40 */ idx::ManifestResource,       /* 41 */ idx::NestedClass,            /* 42  */ idx::GenericParam,
        /* 43 */ idx::MethodSpec,             /* 44 */ idx::GenericParamConstraint, /* 256 */ idx::Unknown
    };

    auto it = find(tables.cbegin(), tables.cend(), value);
    if (*it == idx::Unknown || it == tables.cend()) {
        // Overflow
        return value = idx::Module;
    }

    return value = *next(it, 1);
}

string getTableName(const CLIMetadataTableIndex& value) {
    using idx = CLIMetadataTableIndex;
    const map<idx, string> tables = {
        { idx::Module, "Module"},
        { idx::TypeRef, "TypeRef"},
        { idx::TypeDef, "TypeDef"},
        { idx::Field, "Field"},
        { idx::MethodDef, "MethodDef"},
        { idx::Param, "Param"},
        { idx::InterfaceImpl, "InterfaceImpl"},
        { idx::MemberRef, "MemberRef"},
        { idx::Constant, "Constant"},
        { idx::CustomAttribute, "CustomAttribute"},
        { idx::FieldMarshal, "FieldMarshal"},
        { idx::DeclSecurity, "DeclSecurity"},
        { idx::ClassLayout, "ClassLayout"},
        { idx::FieldLayout, "FieldLayout"},
        { idx::StandAloneSig, "StandAloneSig"},
        { idx::EventMap, "EventMap"},
        { idx::Event, "Event"},
        { idx::PropertyMap, "PropertyMap"},
        { idx::Property, "Property"},
        { idx::MethodSemantics, "MethodSemantics"},
        { idx::MethodImpl, "MethodImpl"},
        { idx::ModuleRef, "ModuleRef"},
        { idx::TypeSpec, "TypeSpec"},
        { idx::ImplMap, "ImplMap"},
        { idx::FieldRVA, "FieldRVA"},
        { idx::Assembly, "Assembly"},
        { idx::AssemblyProcessor, "AssemblyProcessor"},
        { idx::AssemblyOS, "AssemblyOS"},
        { idx::AssemblyRef, "AssemblyRef"},
        { idx::AssemblyRefProcessor, "AssemblyRefProcessor"},
        { idx::AssemblyRefOS, "AssemblyRefOS"},
        { idx::File, "File"},
        { idx::ExportedType, "ExportedType"},
        { idx::ManifestResource, "ManifestResource"},
        { idx::NestedClass, "NestedClass"},
        { idx::GenericParam, "GenericParam"},
        { idx::MethodSpec, "MethodSpec"},
        { idx::GenericParamConstraint, "GenericParamConstraint" },
        { idx::Unknown, "Unknown"}
    };

    auto it = tables.find(value);
    if (it == tables.cend()) {
        throw runtime_error("Incorrect table index");
    }

    return (*it).second;
}
