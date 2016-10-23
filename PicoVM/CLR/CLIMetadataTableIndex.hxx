#ifndef CLIMETA_HXX
#define CLIMETA_HXX
#include <string>
#include <map>
#include <vector>

enum struct CLIMetadataTableItem : uint8_t {
    Module = 0x00,              TypeRef = 0x01,             TypeDef = 0x02,
    FieldDef = 0x04,            MethodDef = 0x06,           ParamDef = 0x08,
    InterfaceImpl = 0x09,       MemberRef = 0x0A,           Constant = 0x0B,
    CustomAttribute = 0x0C,     FieldMarshal = 0x0D,        DeclSecurity = 0x0E,
    ClassLayout = 0x0F,         FieldLayout = 0x10,         StandAloneSig = 0x11,
    EventMap = 0x12,            Event = 0x14,               PropertyMap = 0x15,
    Property = 0x17,            MethodSemantics = 0x18,     MethodImpl = 0x19,
    ModuleRef = 0x1A,           TypeSpec = 0x1B,            ImplMap = 0x1C,
    FieldRVA = 0x1D,            Assembly = 0x20,            AssemblyProcessor = 0x21,
    AssemblyOS = 0x22,          AssemblyRef = 0x23,         AssemblyRefProcessor = 0x24,
    AssemblyRefOS = 0x25,       File = 0x26,                ExportedType = 0x27,
    ManifestResource = 0x28,    NestedClass = 0x29,         GenericParam = 0x2A,
    MethodSpec = 0x2B,          GenericParamConstraint = 0x2C, Unknown = 0xFF
};

const std::map<CLIMetadataTableItem, std::string> cliMetadataTableNames = {
        { CLIMetadataTableItem::Module, "Module"},
        { CLIMetadataTableItem::TypeRef, "TypeRef"},
        { CLIMetadataTableItem::TypeDef, "TypeDef"},
        { CLIMetadataTableItem::FieldDef, "FieldDef"},
        { CLIMetadataTableItem::MethodDef, "MethodDef"},
        { CLIMetadataTableItem::ParamDef, "ParamDef"},
        { CLIMetadataTableItem::InterfaceImpl, "InterfaceImpl"},
        { CLIMetadataTableItem::MemberRef, "MemberRef"},
        { CLIMetadataTableItem::Constant, "Constant"},
        { CLIMetadataTableItem::CustomAttribute, "CustomAttribute"},
        { CLIMetadataTableItem::FieldMarshal, "FieldMarshal"},
        { CLIMetadataTableItem::DeclSecurity, "DeclSecurity"},
        { CLIMetadataTableItem::ClassLayout, "ClassLayout"},
        { CLIMetadataTableItem::FieldLayout, "FieldLayout"},
        { CLIMetadataTableItem::StandAloneSig, "StandAloneSig"},
        { CLIMetadataTableItem::EventMap, "EventMap"},
        { CLIMetadataTableItem::Event, "Event"},
        { CLIMetadataTableItem::PropertyMap, "PropertyMap"},
        { CLIMetadataTableItem::Property, "Property"},
        { CLIMetadataTableItem::MethodSemantics, "MethodSemantics"},
        { CLIMetadataTableItem::MethodImpl, "MethodImpl"},
        { CLIMetadataTableItem::ModuleRef, "ModuleRef"},
        { CLIMetadataTableItem::TypeSpec, "TypeSpec"},
        { CLIMetadataTableItem::ImplMap, "ImplMap"},
        { CLIMetadataTableItem::FieldRVA, "FieldRVA"},
        { CLIMetadataTableItem::Assembly, "Assembly"},
        { CLIMetadataTableItem::AssemblyProcessor, "AssemblyProcessor"},
        { CLIMetadataTableItem::AssemblyOS, "AssemblyOS"},
        { CLIMetadataTableItem::AssemblyRef, "AssemblyRef"},
        { CLIMetadataTableItem::AssemblyRefProcessor, "AssemblyRefProcessor"},
        { CLIMetadataTableItem::AssemblyRefOS, "AssemblyRefOS"},
        { CLIMetadataTableItem::File, "File"},
        { CLIMetadataTableItem::ExportedType, "ExportedType"},
        { CLIMetadataTableItem::ManifestResource, "ManifestResource"},
        { CLIMetadataTableItem::NestedClass, "NestedClass"},
        { CLIMetadataTableItem::GenericParam, "GenericParam"},
        { CLIMetadataTableItem::MethodSpec, "MethodSpec"},
        { CLIMetadataTableItem::GenericParamConstraint, "GenericParamConstraint" },
};

std::string getTableName(const CLIMetadataTableItem& value);

// ResolutionScope: 2 bits to encode tag.
//
// Module       0
// ModuleRef    1
// AssemblyRef  2
// TypeRef      3
const std::vector<CLIMetadataTableItem> resolutionScopeIndex = { CLIMetadataTableItem::Module, CLIMetadataTableItem::ModuleRef, CLIMetadataTableItem::AssemblyRef, CLIMetadataTableItem::TypeRef };

// TypeDefOrRef: 2 bits to encode tag
//
// TypeDef      0
// TypeRef      1
// TypeSpec     2
const std::vector<CLIMetadataTableItem> typeDefOrRef = { CLIMetadataTableItem::TypeDef, CLIMetadataTableItem::TypeRef, CLIMetadataTableItem::TypeSpec };

// MemberRefParent: 3 bits to encode tag
//
// TypeDef     0
// TypeRef     1
// ModuleRef   2
// MethodDef   3
// TypeSpec    4
const std::vector<CLIMetadataTableItem> memberRefParent = { CLIMetadataTableItem::TypeDef, CLIMetadataTableItem::TypeRef, CLIMetadataTableItem::ModuleRef, CLIMetadataTableItem::MethodDef, CLIMetadataTableItem::TypeSpec };

// HasConstant: 2 bits to encode tag
// 
// FieldDef    0
// ParamDef    1
// Property    2
const std::vector<CLIMetadataTableItem> hasConstant = { CLIMetadataTableItem::FieldDef, CLIMetadataTableItem::ParamDef, CLIMetadataTableItem::Property };

// HasCustomAttribute: 5 bits to encode tag
// 
// MethodDef         0
// FieldDef          1
// TypeRef           2
// TypeDef           3
// ParamDef          4
// InterfaceImpl     5
// MemberRef         6
// Module            7
// Permission        8
// Property          9
// Event            10
// StandAloneSig    11
// ModuleRef        12
// TypeSpec         13
// Assembly         14
// AssemblyRef      15
// File             16
// ExportedType     17
// ManifestResource 18
const std::vector<CLIMetadataTableItem> hasCustomAttribute = {
        CLIMetadataTableItem::MethodDef, CLIMetadataTableItem::FieldDef,
        CLIMetadataTableItem::TypeRef, CLIMetadataTableItem::TypeDef, 
        CLIMetadataTableItem::ParamDef, CLIMetadataTableItem::InterfaceImpl, 
        CLIMetadataTableItem::MemberRef, CLIMetadataTableItem::Module, 
        CLIMetadataTableItem::DeclSecurity, CLIMetadataTableItem::Property, 
        CLIMetadataTableItem::Event, CLIMetadataTableItem::StandAloneSig, 
        CLIMetadataTableItem::ModuleRef, CLIMetadataTableItem::TypeSpec, 
        CLIMetadataTableItem::Assembly, CLIMetadataTableItem::AssemblyRef, 
        CLIMetadataTableItem::File, CLIMetadataTableItem::ExportedType, 
        CLIMetadataTableItem::ManifestResource
};

// CustomAttributeType: 3 bits to encode tag
// Not used    0
// Not used    1
// MethodDef   2
// MemberRef   3
// Not used    4
const std::vector<CLIMetadataTableItem> customAttributeType = { CLIMetadataTableItem::Unknown, CLIMetadataTableItem::Unknown, CLIMetadataTableItem::MethodDef, CLIMetadataTableItem::MemberRef, CLIMetadataTableItem::Unknown };

// HasFieldMarshall: 1 bit to encode tag
// FieldDef    0
// ParamDef    1
const std::vector<CLIMetadataTableItem> hasFieldMarshall = { CLIMetadataTableItem::FieldDef, CLIMetadataTableItem::ParamDef };

// HasDeclSecurity: 2 bits to encode tag
// TypeDef     0
// MethodDef   1
// Assembly    2
const std::vector<CLIMetadataTableItem> hasDeclSecurity = { CLIMetadataTableItem::TypeDef, CLIMetadataTableItem::MethodDef, CLIMetadataTableItem::Assembly };

// HasSemantics: 1 bit to encode tag
// Event       0
// Property    1
const std::vector<CLIMetadataTableItem> hasSemantics = { CLIMetadataTableItem::Event, CLIMetadataTableItem::Property };

// MethodDefOrRef: 1 bit to encode tag
//
// MethodDef       0
// MemberRef       1
const std::vector<CLIMetadataTableItem> methodDefOrRef = { CLIMetadataTableItem::MethodDef, CLIMetadataTableItem::MemberRef };

// MemberForwarded: 1 bit to encode tag
// 
// FieldDef        0
// MethodDef       1
const std::vector<CLIMetadataTableItem> memberForwarded = { CLIMetadataTableItem::FieldDef, CLIMetadataTableItem::MethodDef };

// Implementation: 2 bits to encode tag
// 
// File            0
// AssemblyRef     1
// ExportedType    2
const std::vector<CLIMetadataTableItem> implementation = { CLIMetadataTableItem::File, CLIMetadataTableItem::AssemblyRef, CLIMetadataTableItem::ExportedType };

// TypeOrMethodDef : 2 bits to encode value
//
// TypeDef        0
// MethodDef      1
const std::vector<CLIMetadataTableItem> typeOrMethodDef = { CLIMetadataTableItem::TypeDef, CLIMetadataTableItem::MethodDef };



#endif
