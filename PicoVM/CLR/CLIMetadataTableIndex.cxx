#include "CliMetadataTableIndex.hxx"
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

CLIMetadataTableIndex& operator++(CLIMetadataTableIndex& value)
{
    const vector<CLIMetadataTableIndex> tables = {
        /* 00 */ CLIMetadataTableIndex::Module,                 /* 01 */ CLIMetadataTableIndex::TypeRef,                /* 02  */ CLIMetadataTableIndex::TypeDef,
        /* 04 */ CLIMetadataTableIndex::Field,                  /* 06 */ CLIMetadataTableIndex::MethodDef,              /* 08  */ CLIMetadataTableIndex::Param,
        /* 09 */ CLIMetadataTableIndex::InterfaceImpl,          /* 10 */ CLIMetadataTableIndex::MemberRef,              /* 11  */ CLIMetadataTableIndex::Constant,
        /* 12 */ CLIMetadataTableIndex::CustomAttribute,        /* 13 */ CLIMetadataTableIndex::FieldMarshal,           /* 14  */ CLIMetadataTableIndex::DeclSecurity,
        /* 15 */ CLIMetadataTableIndex::ClassLayout,            /* 16 */ CLIMetadataTableIndex::FieldLayout,            /* 17  */ CLIMetadataTableIndex::StandAloneSig,
        /* 18 */ CLIMetadataTableIndex::EventMap,               /* 20 */ CLIMetadataTableIndex::Event,                  /* 21  */ CLIMetadataTableIndex::PropertyMap,
        /* 23 */ CLIMetadataTableIndex::Property,               /* 24 */ CLIMetadataTableIndex::MethodSemantics,        /* 25  */ CLIMetadataTableIndex::MethodImpl,
        /* 26 */ CLIMetadataTableIndex::ModuleRef,              /* 27 */ CLIMetadataTableIndex::TypeSpec,               /* 28  */ CLIMetadataTableIndex::ImplMap,
        /* 29 */ CLIMetadataTableIndex::FieldRVA,               /* 32 */ CLIMetadataTableIndex::Assembly,               /* 33  */ CLIMetadataTableIndex::AssemblyProcessor,
        /* 34 */ CLIMetadataTableIndex::AssemblyOS,             /* 35 */ CLIMetadataTableIndex::AssemblyRef,            /* 36  */ CLIMetadataTableIndex::AssemblyRefProcessor,
        /* 37 */ CLIMetadataTableIndex::AssemblyRefOS,          /* 38 */ CLIMetadataTableIndex::File,                   /* 39  */ CLIMetadataTableIndex::ExportedType,
        /* 40 */ CLIMetadataTableIndex::ManifestResource,       /* 41 */ CLIMetadataTableIndex::NestedClass,            /* 42  */ CLIMetadataTableIndex::GenericParam,
        /* 43 */ CLIMetadataTableIndex::MethodSpec,             /* 44 */ CLIMetadataTableIndex::GenericParamConstraint, /* 256 */ CLIMetadataTableIndex::Unknown
    };

    auto it = find(tables.cbegin(), tables.cend(), value);
    if (*it == CLIMetadataTableIndex::Unknown || it == tables.cend()) {
        // Overflow
        return value = CLIMetadataTableIndex::Module;
    }

    return value = *next(it, 1);
}

string getTableName(const CLIMetadataTableIndex& value) {
    const map<CLIMetadataTableIndex, string> tables = {
        { CLIMetadataTableIndex::Module, "Module"},
        { CLIMetadataTableIndex::TypeRef, "TypeRef"},
        { CLIMetadataTableIndex::TypeDef, "TypeDef"},
        { CLIMetadataTableIndex::Field, "Field"},
        { CLIMetadataTableIndex::MethodDef, "MethodDef"},
        { CLIMetadataTableIndex::Param, "Param"},
        { CLIMetadataTableIndex::InterfaceImpl, "InterfaceImpl"},
        { CLIMetadataTableIndex::MemberRef, "MemberRef"},
        { CLIMetadataTableIndex::Constant, "Constant"},
        { CLIMetadataTableIndex::CustomAttribute, "CustomAttribute"},
        { CLIMetadataTableIndex::FieldMarshal, "FieldMarshal"},
        { CLIMetadataTableIndex::DeclSecurity, "DeclSecurity"},
        { CLIMetadataTableIndex::ClassLayout, "ClassLayout"},
        { CLIMetadataTableIndex::FieldLayout, "FieldLayout"},
        { CLIMetadataTableIndex::StandAloneSig, "StandAloneSig"},
        { CLIMetadataTableIndex::EventMap, "EventMap"},
        { CLIMetadataTableIndex::Event, "Event"},
        { CLIMetadataTableIndex::PropertyMap, "PropertyMap"},
        { CLIMetadataTableIndex::Property, "Property"},
        { CLIMetadataTableIndex::MethodSemantics, "MethodSemantics"},
        { CLIMetadataTableIndex::MethodImpl, "MethodImpl"},
        { CLIMetadataTableIndex::ModuleRef, "ModuleRef"},
        { CLIMetadataTableIndex::TypeSpec, "TypeSpec"},
        { CLIMetadataTableIndex::ImplMap, "ImplMap"},
        { CLIMetadataTableIndex::FieldRVA, "FieldRVA"},
        { CLIMetadataTableIndex::Assembly, "Assembly"},
        { CLIMetadataTableIndex::AssemblyProcessor, "AssemblyProcessor"},
        { CLIMetadataTableIndex::AssemblyOS, "AssemblyOS"},
        { CLIMetadataTableIndex::AssemblyRef, "AssemblyRef"},
        { CLIMetadataTableIndex::AssemblyRefProcessor, "AssemblyRefProcessor"},
        { CLIMetadataTableIndex::AssemblyRefOS, "AssemblyRefOS"},
        { CLIMetadataTableIndex::File, "File"},
        { CLIMetadataTableIndex::ExportedType, "ExportedType"},
        { CLIMetadataTableIndex::ManifestResource, "ManifestResource"},
        { CLIMetadataTableIndex::NestedClass, "NestedClass"},
        { CLIMetadataTableIndex::GenericParam, "GenericParam"},
        { CLIMetadataTableIndex::MethodSpec, "MethodSpec"},
        { CLIMetadataTableIndex::GenericParamConstraint, "GenericParamConstraint" },
        { CLIMetadataTableIndex::Unknown, "Unknown"}
    };

    auto it = tables.find(value);
    if (it == tables.cend()) {
        throw runtime_error("Incorrect table index");
    }

    return (*it).second;
}
