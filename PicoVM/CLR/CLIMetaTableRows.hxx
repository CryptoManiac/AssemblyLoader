#ifndef __CLIMETATABLEROWS__
#define __CLIMETATABLEROWS__

#include <cstdint>
#include <vector>

#include "CLIMetadataTableIndex.hxx"

// A one row table representing the current assembly.
struct ModuleRow {
    uint16_t generation;
    std::vector<uint16_t> name;
    std::vector<uint8_t> guid;
};

// Each row represents an imported class, its namespace, and the assembly which contains it.
struct TypeRefRow {
    std::pair<uint32_t, CliMetadataTableIndex> resolutionScope;
    std::vector<uint16_t> typeName;
    std::vector<uint16_t> typeNamespace;
};

struct TypeDefRow {
    uint32_t flags;
    std::vector<uint16_t> typeName;
    std::vector<uint16_t> typeNamespace;
    std::pair<uint32_t, CliMetadataTableIndex> extendsType;

    uint32_t fieldList;
    uint32_t methodList; 
};

struct FieldRow {
    uint16_t flags;
    std::vector<uint16_t> name;
    std::vector<uint32_t> signature;
};

struct MethodDefRow {
    uint32_t rva;
    uint16_t implFlags;
    uint16_t flags;
    std::vector<uint16_t> name;
    std::vector<uint32_t> signature;
    uint32_t paramList;
};

struct ParamRow {
    uint16_t flags;
    uint16_t sequence;
    std::vector<uint16_t> name;
};

struct InterfaceImplRow {
    uint32_t classRef;
    std::pair<uint32_t, CliMetadataTableIndex> interfaceRef;
};

struct MemberRefRow {
    std::pair<uint32_t, CliMetadataTableIndex> classRef;
    std::vector<uint16_t> name;
    std::vector<uint32_t> signature;
};

struct ConstantRow {
    uint16_t type;
    std::pair<uint32_t, CliMetadataTableIndex> parent;
    std::vector<uint8_t> value;
};

struct CustomAttributeRow {
    std::pair<uint32_t, CliMetadataTableIndex> parent;
    std::pair<uint32_t, CliMetadataTableIndex> type;
    std::vector<uint8_t> value;
};

struct FieldMarshalRow {
    std::pair<uint32_t, CliMetadataTableIndex> parent;
    std::vector<uint8_t> nativeType;
};

struct DeclSecurityRow {
    uint16_t action;
    std::pair<uint32_t, CliMetadataTableIndex> parent;
    std::vector<uint8_t> permissionSet;
};

struct ClassLayoutRow {
    uint16_t packingSize;
    uint32_t classSize;
    uint32_t parent;
};

struct FieldLayoutRow {
    uint32_t offset;
    uint32_t parent;
};

struct EventMapRow {
    uint32_t parent;
    uint32_t eventList;
};

struct EventRow {
    uint16_t eventFlags;
    std::vector<uint16_t> name;
    std::pair<uint32_t, CliMetadataTableIndex> eventType;
};

struct PropertyMapRow {
    uint32_t parent;
    uint32_t propertyList;
};

struct PropertyRow {
    uint16_t flags;
    std::vector<uint16_t> name;
    std::vector<uint32_t> signature;
};

struct MethodSemanticsRow {
    uint16_t semantics;
    uint32_t method;
    std::pair<uint32_t, CliMetadataTableIndex> association;
};

struct MethodImplRow {
    uint32_t classRef;
    std::pair<uint32_t, CliMetadataTableIndex> methodBody;
    std::pair<uint32_t, CliMetadataTableIndex> methodDeclaration;
};

struct ImplMapRow {
    uint16_t mappingFlags;
    std::pair<uint32_t, CliMetadataTableIndex> memberForwarded;
    std::vector<uint16_t> importName;
    uint32_t importScope;
};

struct FieldRVARow {
    uint32_t rva;
    uint32_t field;
};

struct AssemblyRow {
    uint32_t hashAlgId;
    std::vector<uint16_t> version;
    uint32_t flags;
    std::vector<uint8_t> publicKey;
    std::vector<uint16_t> name;
    std::vector<uint16_t> culture;
};

struct AssemblyOSRow {
    uint32_t osPlatformID;
    uint32_t osMajorVersion;
    uint32_t osMinorVersion;
};

struct AssemblyRefRow {
    std::vector<uint16_t> version;
    uint32_t flags;
    std::vector<uint8_t> publicKeyOrToken;
    std::vector<uint16_t> name;
    std::vector<uint16_t> culture;
    std::vector<uint8_t> hashValue;
};

struct AssemblyRefProcessorRow {
    uint32_t processor;
    uint32_t assemblyRef;
};

struct AssemblyRefOSRow {
    uint32_t osPlatformID;
    uint32_t osMajorVersion;
    uint32_t osMinorVersion;
    uint32_t assemblyRef;
};

struct FileRow {
    uint32_t flags;
    std::vector<uint16_t> name;
    std::vector<uint8_t> hashValue;
};

struct ExportedTypeRow {
    uint32_t flags;
    uint32_t typeDefId;
    std::vector<uint16_t> typeName;
    std::vector<uint16_t> typeNamespace;
    std::pair<uint32_t, CliMetadataTableIndex> implementation;
};

struct ManifestResourceRow {
    uint32_t offset;
    uint32_t flags;
    std::vector<uint16_t> name;
    std::pair<uint32_t, CliMetadataTableIndex> implementation;
};

struct NestedClassRow {
    uint32_t nestedClass;
    uint32_t enclosingClass;
};

struct GenericParamRow {
    uint16_t number;
    uint16_t flags;
    std::pair<uint32_t, CliMetadataTableIndex> owner;
    std::vector<uint16_t> name;
};

struct MethodSpecRow {
    std::pair<uint32_t, CliMetadataTableIndex> method;
    std::vector<uint32_t> instantiation;
};

struct GenericParamConstraintRow {
    uint32_t owner;
    std::pair<uint32_t, CliMetadataTableIndex> constraint;
};

#endif
