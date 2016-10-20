#ifndef __CLIMETATABLEROWS__
#define __CLIMETATABLEROWS__

#include <cstdint>
#include <vector>

#include "CliMetadataTableIndex.hxx"

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
    // 4-byte bit mask of type TypeAttributes
    uint32_t flags;
    std::vector<uint16_t> typeName;
    std::vector<uint16_t> typeNamespace;
    std::pair<uint32_t, CliMetadataTableIndex> extendsType;

    uint32_t fieldList;
    uint32_t methodList;

    enum TypeAttributes : uint32_t {
        // Use this mask to retrieve the type visibility information.
        VisibilityMask        =   0x00000007,
        // Class is not public scope.
        NotPublic             =   0x00000000,     
        // Class is public scope.
        Public                =   0x00000001,     
        // Class is nested with public visibility.
        NestedPublic          =   0x00000002,     
        // Class is nested with private visibility.
        NestedPrivate         =   0x00000003,     
        // Class is nested with family visibility.
        NestedFamily          =   0x00000004,     
        // Class is nested with assembly visibility.
        NestedAssembly        =   0x00000005,     
        // Class is nested with family and assembly visibility.
        NestedFamANDAssem     =   0x00000006,     
        // Class is nested with family or assembly visibility.
        NestedFamORAssem      =   0x00000007,     

        // Use this mask to retrieve class layout information
        LayoutMask            =   0x00000018,
        // Class fields are auto-laid out
        AutoLayout            =   0x00000000,     
        // Class fields are laid out sequentially
        SequentialLayout      =   0x00000008,     
        // Layout is supplied explicitly
        ExplicitLayout        =   0x00000010,     

        // Use this mask to retrieve class semantics information.
        ClassSemanticsMask    =   0x00000060,
        // Type is a class.
        Class                 =   0x00000000,     
        // Type is an interface.
        Interface             =   0x00000020,     

        // Special semantics in addition to class semantics.

        // Class is abstract
        Abstract              =   0x00000080,     
        // Class is concrete and may not be extended
        Sealed                =   0x00000100,     
        // Class name is special. Name describes how.
        SpecialName           =   0x00000400,

        // Class / interface is imported
        Import                =   0x00001000,     
        // The class is Serializable.
        Serializable          =   0x00002000,     

        // Use tdStringFormatMask to retrieve string information for native interop
        StringFormatMask      =   0x00030000,
        // LPTSTR is interpreted as ANSI in this class
        AnsiClass             =   0x00000000,     
        // LPTSTR is interpreted as UNICODE
        UnicodeClass          =   0x00010000,     
        // LPTSTR is interpreted automatically
        AutoClass             =   0x00020000,     
        // A non-standard encoding specified by CustomFormatMask
        CustomFormatClass     =   0x00030000,     

        // Use this mask to retrieve non-standard encoding information for native interop. 
        // The meaning of the values of these 2 bits is unspecified.
        CustomFormatMask      =   0x00C00000,
        // Initialize the class any time before first static field access.
        BeforeFieldInit       =   0x00100000,     
        // This ExportedType is a type forwarder.
        Forwarder             =   0x00200000,     
        // Flags reserved for runtime use.
        ReservedMask          =   0x00040800,
        // Runtime should check name encoding.
        RTSpecialName         =   0x00000800,     
        // Class has security associate with it.
        HasSecurity           =   0x00040000,
    };
};

struct FieldRow {
    // 2-byte bit mask of type FieldAttributes
    uint16_t flags;
    std::vector<uint16_t> name;
    std::vector<uint32_t> signature;
};

struct MethodDefRow {
    uint32_t rva;
    // 2-byte bit mask of type MethodImplAttributes
    uint16_t implFlags;
    // 2-byte bit mask of type MethodAttribute
    uint16_t flags;
    std::vector<uint16_t> name;
    std::vector<uint32_t> signature;
    uint32_t paramList;
};

struct ParamRow {
    uint16_t flags;
    // 2-byte bit mask of type ParamAttributes
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
    // 2-byte bit mask of type EventAttribute
    uint16_t eventFlags;
    std::vector<uint16_t> name;
    std::pair<uint32_t, CliMetadataTableIndex> eventType;
};

struct PropertyMapRow {
    uint32_t parent;
    uint32_t propertyList;
};

struct PropertyRow {
    // 2-byte bit mask of type PropertyAttributes
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
    // 2-byte bit mask of type PInvokeAttributes
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
    // 4-byte bit mask of type AssemblyFlags
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
    // 4-byte bit mask of type AssemblyFlags
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
    // 4-byte bit mask of type FileAttributes
    uint32_t flags;
    std::vector<uint16_t> name;
    std::vector<uint8_t> hashValue;
};

struct ExportedTypeRow {
    // 4-byte bit mask of type TypeAttributes
    uint32_t flags;
    uint32_t typeDefId;
    std::vector<uint16_t> typeName;
    std::vector<uint16_t> typeNamespace;
    std::pair<uint32_t, CliMetadataTableIndex> implementation;
};

struct ManifestResourceRow {
    uint32_t offset;
    // 4-byte bit mask of type ManifestResourceAttributes
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
    // 2-byte bitmask of type GenericParamAttributes
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
