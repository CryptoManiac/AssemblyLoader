#ifndef __CLIMETATABLEROWS__
#define __CLIMETATABLEROWS__

#include <cstdint>
#include <vector>
#include <map>
#include <string>

#include "crossguid/guid.hxx"
#include "CLIMetadata.hxx"
#include "AssemblyReader.hxx"
#include "CLIMetadataTableIndex.hxx"
#include "CLIMethodBody.hxx"

struct MetadataRowsReader {
    AssemblyReader& reader;
    std::map<CLIMetadataTableItem, uint32_t> mapTableLength;

    uint32_t metaDataOffset = 0;
    uint32_t stringStreamOffset = 0;
    uint32_t guidStreamOffset = 0;
    uint32_t blobStreamOffset = 0;

    bool stringsIsLong = false;
    bool guidIsLong = false;
    bool blobIsLong = false;

    MetadataRowsReader() = delete;
    MetadataRowsReader(AssemblyReader& Reader, CLIMetadata& cliMetadata);
    MetadataRowsReader(const MetadataRowsReader& other) = default;
    MetadataRowsReader(MetadataRowsReader&& other) = default;

    MetadataRowsReader& operator=(const MetadataRowsReader& other);

    void swap(MetadataRowsReader& other) noexcept;
    void readGuid(Guid& result);
    void readBlob(std::vector<uint8_t>& result);
    void readString(std::u16string& result);
    void readSignature(std::vector<uint32_t>& result);

    uint32_t readRowIndex(CLIMetadataTableItem tableIndex);
    std::pair<uint32_t, CLIMetadataTableItem> readRowIndexChoice(const std::vector<CLIMetadataTableItem>& tables);

private:
    void Init(CLIMetadata& cliMetadata);
};

// A one row table representing the current assembly.
struct ModuleRow {
    Guid guid;
    std::u16string name;
    uint16_t generation = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::Module;

    ModuleRow() = default;
    ModuleRow(MetadataRowsReader& mr);
    std::string str() const;

    ~ModuleRow() noexcept;
};

// Each row represents an imported class, its namespace, and the assembly which contains it.
struct TypeRefRow {
    std::pair<uint32_t, CLIMetadataTableItem> resolutionScope;
    std::u16string typeName;
    std::u16string typeNamespace;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::TypeRef;

    TypeRefRow() = default;
    TypeRefRow(MetadataRowsReader& mr);

    ~TypeRefRow() noexcept;
};

struct TypeDefRow {
    std::u16string typeName;
    std::u16string typeNamespace;

    // Index into TypeDef, TypeRef or TypeSpec table
    std::pair<uint32_t, CLIMetadataTableItem> extendsType;

    // 4-byte bit mask of type TypeAttributes
    uint32_t flags = 0;

    // Index into Field table
    uint32_t fieldList = 0;

    // Index into MethodDef table
    uint32_t methodList = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::TypeDef;

    TypeDefRow() = default;
    TypeDefRow(MetadataRowsReader& mr);

    enum struct TypeAttributes : uint32_t {
        // Use this mask to retrieve the type visibility information.
        VisibilityMask = 0x00000007,
        NotPublic = 0x00000000,         // Class is not public scope.
        Public = 0x00000001,            // Class is public scope.
        NestedPublic = 0x00000002,      // Class is nested with public visibility.
        NestedPrivate = 0x00000003,     // Class is nested with private visibility.
        NestedFamily = 0x00000004,      // Class is nested with family visibility.
        NestedAssembly = 0x00000005,    // Class is nested with assembly visibility.
        NestedFamANDAssem = 0x00000006, // Class is nested with family and assembly visibility.
        NestedFamORAssem = 0x00000007,  // Class is nested with family or assembly visibility.

        // Use this mask to retrieve class layout information.
        LayoutMask = 0x00000018,
        AutoLayout = 0x00000000,         // Class fields are auto-laid out.
        SequentialLayout = 0x00000008,   // Class fields are laid out sequentially.
        ExplicitLayout = 0x00000010,     // Layout is supplied explicitly.
        ClassSemanticsMask = 0x00000060, // Use this mask to retrieve class semantics information.
        Class = 0x00000000,              // Type is a class.
        Interface = 0x00000020,          // Type is an interface.

        // Special semantics in addition to class semantics.
        Abstract = 0x00000080,     // Class is abstract.
        Sealed = 0x00000100,       // Class is concrete and may not be extended.
        SpecialName = 0x00000400,  // Class name is special. Name describes how.
        Import = 0x00001000,       // Class / interface is imported.
        Serializable = 0x00002000, // The class is Serializable.

        // Use to retrieve string information for native interop.
        StringFormatMask = 0x00030000,
        AnsiClass = 0x00000000,         // LPTSTR is interpreted as ANSI in this class.
        UnicodeClass = 0x00010000,      // LPTSTR is interpreted as UNICODE.
        AutoClass = 0x00020000,         // LPTSTR is interpreted automatically.
        CustomFormatClass = 0x00030000, // A non-standard encoding specified by CustomFormatMask.

        // Use this mask to retrieve non-standard encoding information for native interop.
        CustomFormatMask = 0x00C00000, // The meaning of the values of these 2 bits is unspecified.
        BeforeFieldInit = 0x00100000,  // Initialize the class any time before first static field access.
        Forwarder = 0x00200000,        // This ExportedType is a type forwarder.
        ReservedMask = 0x00040800,     // Flags reserved for runtime use.
        RTSpecialName = 0x00000800,    // Runtime should check name encoding.
        HasSecurity = 0x00040000       // Class has security associate with it.
    };

    ~TypeDefRow() noexcept;
};

struct FieldDefRow {
    // 2-byte bit mask of type FieldAttributes
    uint16_t flags = 0;
    std::u16string name;
    std::vector<uint32_t> signature;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::FieldDef;

    FieldDefRow() = default;
    FieldDefRow(MetadataRowsReader& mr);

    enum struct FieldAttributes : uint16_t {
        // Use this mask to retrieve accessibility information.
        FieldAccessMask = 0x0007,
        PrivateScope = 0x0000, // Member not referenceable.
        Private = 0x0001,      // Accessible only by the parent type.
        FamANDAssem = 0x0002,  // Accessible by sub-types only in this Assembly.
        Assembly = 0x0003,     // Accessibly by anyone in the Assembly.
        Family = 0x0004,       // Accessible only by type and sub-types.
        FamORAssem = 0x0005,   // Accessibly by sub-types anywhere, plus anyone in assembly.
        Public = 0x0006,       // Accessibly by anyone who has visibility to this scope.

        // Field contract attributes.
        Static = 0x0010,        // Defined on type, else per instance.
        InitOnly = 0x0020,      // Field may only be initialized, not written to after init.
        Literal = 0x0040,       // Value is compile time constant.
        NotSerialized = 0x0080, // Field does not have to be serialized when type is remoted.
        SpecialName = 0x0200,   // Field is special. Name describes how.

        // Interop attributes
        PinvokeImpl = 0x2000,     // Implementation is forwarded through pinvoke.
        ReservedMask = 0x9500,    // Reserved flags for runtime use only.
        RTSpecialName = 0x0400,   // Runtime(metadata internal APIs) should check name encoding.
        HasFieldMarshal = 0x1000, // Field has marshalling information.
        HasDefault = 0x8000,      // Field has default.
        HasFieldRVA = 0x0100      // Field has RVA.
    };

    ~FieldDefRow() noexcept;
};

struct MethodDefRow {
    // Method name and signature
    std::u16string name;
    std::vector<uint32_t> signature;

    // Method body data, in-memory only
    MethodBody methodBody;

    // Index into ParamDef table
    uint32_t paramList = 0;
    // Method RVA
    uint32_t rva = 0;
    // 2-byte bit mask of type MethodImplAttributes
    uint16_t implFlags = 0;
    // 2-byte bit mask of type MethodAttribute
    uint16_t flags = 0;

    // For FillTable<T>()
    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::MethodDef;

    MethodDefRow() = default;
    MethodDefRow(MetadataRowsReader& mr);

    enum struct MethodAttribute : uint16_t {
        // Use this mask to retrieve accessibility information.
        MemberAccessMask = 0x0007,
        PrivateScope = 0x0000, // Member not referenceable.
        Private = 0x0001,      // Accessible only by the parent type.
        FamANDAssem = 0x0002,  // Accessible by sub-types only in this Assembly.
        Assem = 0x0003,        // Accessibly by anyone in the Assembly.
        Family = 0x0004,       // Accessible only by type and sub-types.
        FamORAssem = 0x0005,   // Accessibly by sub-types anywhere, plus anyone in assembly.
        Public = 0x0006,       // Accessibly by anyone who has visibility to this scope.

        // method contract attributes.
        Static = 0x0010,    // Defined on type, else per instance.
        Final = 0x0020,     // Method may not be overridden.
        Virtual = 0x0040,   // Method virtual.
        HideBySig = 0x0080, // Method hides by name+sig, else just by name.

        // Use this mask to retrieve vtable attributes.
        VtableLayoutMask = 0x0100,
        ReuseSlot = 0x0000, // The default.
        NewSlot = 0x0100,   // Method always gets a new slot in the vtable.

        // Method implementation attributes.
        CheckAccessOnOverride = 0x0200, // Overridability is the same as the visibility.
        Abstract = 0x0400,              // Method does not provide an implementation.
        SpecialName = 0x0800,           // Method is special. Name describes how.

        // Interop attributes
        PinvokeImpl = 0x2000,     // Implementation is forwarded through pinvoke.
        UnmanagedExport = 0x0008, // Managed method exported via thunk to unmanaged code.

        // Reserved flags for runtime use only.
        ReservedMask = 0xd000,
        RTSpecialName = 0x1000,    // Runtime should check name encoding.
        HasSecurity = 0x4000,      // Method has security associate with it.
        RequireSecObject = 0x8000  // Method calls another method containing security code.
    };

    enum struct MethodImplAttributes : uint16_t {
        // Code impl mask
        CodeTypeMask = 0x0003, // Flags about code type.
        IL = 0x0000,           // Method impl is IL.
        Native = 0x0001,       // Method impl is native.
        OPTIL = 0x0002,        // Method impl is OPTIL
        Runtime = 0x0003,      // Method impl is provided by the runtime.

        // Managed mask
        ManagedMask = 0x0004, // Flags specifying whether the code is managed or unmanaged.
        Unmanaged = 0x0004,   // Method impl is unmanaged, otherwise managed.
        Managed = 0x0000,     // Method impl is managed.

        // Implementation info and interop
        ForwardRef = 0x0010,       // Indicates method is defined; used primarily in merge scenarios.
        PreserveSig = 0x0080,      // Indicates method sig is not to be mangled to do HRESULT conversion.
        InternalCall = 0x1000,     // Reserved for internal use.
        Synchronized = 0x0020,     // Method is single threaded through the body.
        NoInlining = 0x0008,       // Method may not be inlined.
        MaxMethodImplVal = 0xffff  // Range check value
    };

    ~MethodDefRow() noexcept;
};

struct ParamDefRow {
    // Parameter name
    std::u16string name;
    // 2-byte bit mask of type ParamAttributes
    uint16_t flags = 0;
    // Param record index
    uint16_t sequence = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::ParamDef;

    ParamDefRow() = default;
    ParamDefRow(MetadataRowsReader& mr);

    enum struct ParamAttributes : uint16_t {
        In = 0x0001,       // Param is [In]
        Out = 0x0002,      // Param is [out]
        Optional = 0x0010, // Param is optional

        // Reserved flags for Runtime use only.
        ReservedMask = 0xf000,
        HasDefault = 0x1000,      // Param has default value.
        HasFieldMarshal = 0x2000, // Param has FieldMarshal.
        Unused = 0xcfe0
    };
};

struct InterfaceImplRow {
    // Index into the TypeDef, TypeRef or TypeSpec table
    std::pair<uint32_t, CLIMetadataTableItem> interfaceRef;
    // Index into the TypeDef table
    uint32_t classRef = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::InterfaceImpl;

    InterfaceImplRow() = default;
    InterfaceImplRow(MetadataRowsReader& mr);
};

struct MemberRefRow {
    // Index into the TypeRef, ModuleRef, MethodDef, TypeSpec, or TypeDef
    std::pair<uint32_t, CLIMetadataTableItem> classRef;
    std::u16string name;
    std::vector<uint32_t> signature;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::MemberRef;

    MemberRefRow() = default;
    MemberRefRow(MetadataRowsReader& mr);

    ~MemberRefRow() noexcept;
};

struct ConstantRow {
    // Index into the Param or Field or Property table
    std::pair<uint32_t, CLIMetadataTableItem> parent;
    // Constant value
    std::vector<uint8_t> value;
    uint16_t type = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::Constant;

    ConstantRow() = default;
    ConstantRow(MetadataRowsReader& mr);
};

struct CustomAttributeRow {
    // HasCustomAttribute index.
    std::pair<uint32_t, CLIMetadataTableItem> parent;
    std::pair<uint32_t, CLIMetadataTableItem> type;
    std::vector<uint8_t> value;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::CustomAttribute;

    CustomAttributeRow() = default;
    CustomAttributeRow(MetadataRowsReader& mr);
};

struct FieldMarshalRow {
    // HasFieldMarshal index.
    std::pair<uint32_t, CLIMetadataTableItem> parent;
    std::vector<uint8_t> nativeType;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::FieldMarshal;

    FieldMarshalRow() = default;
    FieldMarshalRow(MetadataRowsReader& mr);
};

struct DeclSecurityRow {
    // HasDeclSecurity index.
    std::pair<uint32_t, CLIMetadataTableItem> parent;
    std::vector<uint8_t> permissionSet;

    uint16_t action = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::DeclSecurity;

    DeclSecurityRow() = default;
    DeclSecurityRow(MetadataRowsReader& mr);
};

struct ClassLayoutRow {
    uint32_t classSize = 0;
    uint32_t parent = 0;
    uint16_t packingSize = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::ClassLayout;

    ClassLayoutRow() = default;
    ClassLayoutRow(MetadataRowsReader& mr);
};

struct FieldLayoutRow {
    uint32_t offset = 0;
    uint32_t parent = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::FieldLayout;

    FieldLayoutRow() = default;
    FieldLayoutRow(MetadataRowsReader& mr);
};

struct EventMapRow {
    uint32_t parent = 0;
    uint32_t eventList = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::EventMap;

    EventMapRow() = default;
    EventMapRow(MetadataRowsReader& mr);
};

struct EventRow {
    std::u16string name;
    std::pair<uint32_t, CLIMetadataTableItem> eventType;

    // 2-byte bit mask of type EventAttribute
    uint16_t eventFlags = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::Event;

    EventRow() = default;
    EventRow(MetadataRowsReader& mr);

    enum struct EventAttribute : uint16_t {
        evSpecialName = 0x0200, // Event is special. Name describes how.
        // Reserved flags for Runtime use only.
        evReservedMask = 0x0400,
        evRTSpecialName = 0x0400 // Runtime(metadata internal APIs) should check name encoding.
    };
};

struct PropertyMapRow {
    uint32_t parent = 0;
    uint32_t propertyList = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::PropertyMap;

    PropertyMapRow() = default;
    PropertyMapRow(MetadataRowsReader& mr);
};

struct PropertyRow {
    std::u16string name;
    std::vector<uint32_t> signature;

    // 2-byte bit mask of type PropertyAttributes
    uint16_t flags = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::Property;

    PropertyRow() = default;
    PropertyRow(MetadataRowsReader& mr);

    enum struct PropertyAttributes : uint16_t {
        SpecialName = 0x0200, // Property is special. Name describes how.
        // Reserved flags for Runtime use only.
        ReservedMask = 0xf400,
        RTSpecialName = 0x0400, // Runtime(metadata internal APIs) should check name encoding.
        HasDefault = 0x1000,    // Property has default.
        Unused = 0xe9ff
    };

    ~PropertyRow() noexcept;
};

struct MethodSemanticsRow {
    uint32_t method = 0;
    std::pair<uint32_t, CLIMetadataTableItem> association;

    // 2-byte bit mask of type MethodSemanticsAttributes
    uint16_t semantics = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::MethodSemantics;

    MethodSemanticsRow() = default;
    MethodSemanticsRow(MetadataRowsReader& mr);

    enum struct MethodSemanticsAttributes : uint16_t {
        Setter = 0x0001,   // Setter for property
        Getter = 0x0002,   // Getter for property
        Other = 0x0004,    // other method for property or event
        AddOn = 0x0008,    // AddOn method for event
        RemoveOn = 0x0010, // RemoveOn method for event
        Fire = 0x0020      // Fire method for event
    };
};

struct MethodImplRow {
    std::pair<uint32_t, CLIMetadataTableItem> methodBody;
    std::pair<uint32_t, CLIMetadataTableItem> methodDeclaration;
    uint32_t classRef = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::MethodImpl;

    MethodImplRow() = default;
    MethodImplRow(MetadataRowsReader& mr);
};

struct ImplMapRow {
    std::pair<uint32_t, CLIMetadataTableItem> memberForwarded;
    std::u16string importName;
    // Index into the ModuleRef table
    uint32_t importScope = 0;
    // 2-byte bit mask of type PInvokeAttributes
    uint16_t mappingFlags = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::ImplMap;

    ImplMapRow() = default;
    ImplMapRow(MetadataRowsReader& mr);

    enum struct PInvokeAttributes : uint16_t {
        NoMangle = 0x0001,

        // Use this mask to retrieve the CharSet information.
        CharSetMask = 0x0006,
        CharSetNotSpec = 0x0000,
        CharSetAnsi = 0x0002,
        CharSetUnicode = 0x0004,
        CharSetAuto = 0x0006,

        BestFitUseAssem = 0x0000,
        BestFitEnabled = 0x0010,
        BestFitDisabled = 0x0020,
        BestFitMask = 0x0030,

        ThrowOnUnmappableCharUseAssem = 0x0000,
        ThrowOnUnmappableCharEnabled = 0x1000,
        ThrowOnUnmappableCharDisabled = 0x2000,
        ThrowOnUnmappableCharMask = 0x3000,

        SupportsLastError = 0x0040,

        // Information about target function. Not relevant for fields.
        // None of the calling convention flags is relevant for fields.
        CallConvMask = 0x0700,
        CallConvWinapi = 0x0100,
        // Pinvoke will use native callconv appropriate to target windows platform.
        CallConvCdecl = 0x0200,
        CallConvStdcall = 0x0300,
        CallConvThiscall = 0x0400,
        // In M9, pinvoke will raise exception.
        CallConvFastcall = 0x0500,
        MaxValue = 0xFFFF
    };
};

struct FieldRVARow {
    uint32_t rva = 0;
    uint32_t field = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::FieldRVA;

    FieldRVARow() = default;
    FieldRVARow(MetadataRowsReader& mr);
};

struct AssemblyRow {
    // MajorVersion, MinorVersion, BuildNumber, RevisionNumber 
    std::vector<uint16_t> version;
    std::vector<uint8_t> publicKey;
    std::u16string name;
    std::u16string culture;

    // 4-byte constant of type AssemblyHashAlgorithm
    uint32_t hashAlgId = 0;
    // 4-byte bit mask of type AssemblyFlags
    uint32_t flags = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::Assembly;

    AssemblyRow() = default;
    AssemblyRow(MetadataRowsReader& mr);

    enum struct AssemblyFlags : uint16_t {
        PublicKey = 0x0001,                  // The assembly ref holds the full (unhashed) public key.
        PA_None = 0x0000,                    // Processor Architecture unspecified.
        PA_MSIL = 0x0010,                    // Processor Architecture: neutral (PE32).
        PA_x86 = 0x0020,                     // Processor Architecture: x86 (PE32).
        PA_IA64 = 0x0030,                    // Processor Architecture: Itanium (PE32+).
        PA_AMD64 = 0x0040,                   // Processor Architecture: AMD X64 (PE32+).
        PA_Specified = 0x0080,               // Propagate PA flags to AssemblyRef record.
        PA_Mask = 0x0070,                    // Bits describing the processor architecture.
        PA_FullMask = 0x00F0,                // Bits describing the PA incl. Specified
        PA_Shift = 0x0004,                   // NOT A FLAG, shift count in PA flags <--> index conversion.
        EnableJITcompileTracking = 0x8000,   // From "DebuggableAttribute".
        DisableJITcompileOptimizer = 0x4000, // From "DebuggableAttribute".
        Retargetable = 0x0100                // The assembly can be retargeted (at runtime) to an assembly from a different publisher.
    };

    enum struct AssemblyHashAlgorithm : uint32_t {
        None = 0,
        MD2 = 0x8001,
        MD4 = 0x8002,
        MD5 = 0x8003,
        SHA1 = 0x8004,
        MAC = 0x8005,
        SSL3_SHAMD5 = 0x8008,
        HMAC = 0x8009,
        TLS1PRF = 0x800A,
        HASH_REPLACE_OWF = 0x800B,
        SHA_256 = 0x800C,
        SHA_384 = 0x800D,
        SHA_512 = 0x800E
    };

    ~AssemblyRow() noexcept;
};

struct AssemblyOSRow {
    uint32_t osPlatformID = 0;
    uint32_t osMajorVersion = 0;
    uint32_t osMinorVersion = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::AssemblyOS;

    AssemblyOSRow() = default;
    AssemblyOSRow(MetadataRowsReader& mr);
};

struct AssemblyRefRow {
    // MajorVersion, MinorVersion, BuildNumber, RevisionNumber 
    std::vector<uint16_t> version;
    std::vector<uint8_t> publicKeyOrToken;
    std::vector<uint8_t> hashValue;
    std::u16string name;
    std::u16string culture;

    // 4-byte bit mask of type AssemblyFlags
    uint32_t flags = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::AssemblyRef;

    AssemblyRefRow() = default;
    AssemblyRefRow(MetadataRowsReader& mr);

    ~AssemblyRefRow() noexcept;
};

struct AssemblyRefProcessorRow {
    uint32_t processor = 0;
    uint32_t assemblyRef = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::AssemblyRefProcessor;

    AssemblyRefProcessorRow() = default;
    AssemblyRefProcessorRow(MetadataRowsReader& mr);
};

struct AssemblyRefOSRow {
    uint32_t osPlatformID = 0;
    uint32_t osMajorVersion = 0;
    uint32_t osMinorVersion = 0;
    uint32_t assemblyRef = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::AssemblyRefOS;

    AssemblyRefOSRow() = default;
    AssemblyRefOSRow(MetadataRowsReader& mr);
};

struct FileRow {
    std::u16string name;
    std::vector<uint8_t> hashValue;

    // 4-byte bit mask of type FileAttributes
    uint32_t flags = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::File;

    FileRow() = default;
    FileRow(MetadataRowsReader& mr);

    enum struct FileAttributes {
        ContainsMetaData = 0x0000,  // This is not a resource file.
        ContainsNoMetaData = 0x0001 // This is a resource file or other non-metadata-containing file.
    };

    ~FileRow() noexcept;
};

struct ExportedTypeRow {
    // Names of type and namespace
    std::u16string typeName;
    std::u16string typeNamespace;

    // Implementation coded index
    std::pair<uint32_t, CLIMetadataTableItem> implementation;

    // 4-byte bit mask of type TypeAttributes
    uint32_t flags = 0;
    uint32_t typeDefId = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::ExportedType;

    ExportedTypeRow() = default;
    ExportedTypeRow(MetadataRowsReader& mr);

    ~ExportedTypeRow() noexcept;
};

struct ManifestResourceRow {
    std::u16string name;
    // Implementation coded index
    std::pair<uint32_t, CLIMetadataTableItem> implementation;
    
    uint32_t offset;
    // 4-byte bit mask of type ManifestResourceAttributes
    uint32_t flags = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::ManifestResource;

    ManifestResourceRow() = default;
    ManifestResourceRow(MetadataRowsReader& mr);

    enum struct ManifestResourceAttributes {
        VisibilityMask = 0x0007,
        Public = 0x0001, // The Resource is exported from the Assembly.
        Private = 0x0002 // The Resource is private to the Assembly.
    };
};

struct NestedClassRow {
    uint32_t nestedClass = 0;
    uint32_t enclosingClass = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::NestedClass;

    NestedClassRow() = default;
    NestedClassRow(MetadataRowsReader& mr);
};

struct GenericParamRow {
    std::pair<uint32_t, CLIMetadataTableItem> owner;
    std::u16string name;

    // 2-byte index of the generic parameter
    uint16_t number = 0;
    // 2-byte bitmask of type GenericParamAttributes
    uint16_t flags = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::GenericParam;

    GenericParamRow() = default;
    GenericParamRow(MetadataRowsReader& mr);

    enum struct GenericParamAttributes {
        // Variance of type parameters, only applicable to generic parameters
        // for generic interfaces and delegates
        gpVarianceMask = 0x0003,
        gpNonVariant = 0x0000,
        gpCovariant = 0x0001,
        gpContravariant = 0x0002,

        // Special constraints, applicable to any type parameters
        gpSpecialConstraintMask = 0x001C,
        gpNoSpecialConstraint = 0x0000,
        gpReferenceTypeConstraint = 0x0004,        // type argument must be a reference type
        gpNotNullableValueTypeConstraint = 0x0008, // type argument must be a value type but not Nullable
        gpDefaultConstructorConstraint = 0x0010,   // type argument must have a public
                                                   // default constructor
    };
};

struct MethodSpecRow {
    std::pair<uint32_t, CLIMetadataTableItem> method;
    std::vector<uint32_t> instantiation;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::MethodSpec;

    MethodSpecRow() = default;
    MethodSpecRow(MetadataRowsReader& mr);
};

struct GenericParamConstraintRow {
    // Index into the TypeDef, TypeRef, or TypeSpec tables
    std::pair<uint32_t, CLIMetadataTableItem> constraint;
    // Index into the GenericParam table
    uint32_t owner = 0;

    static const CLIMetadataTableItem tableID = CLIMetadataTableItem::GenericParamConstraint;

    GenericParamConstraintRow() = default;
    GenericParamConstraintRow(MetadataRowsReader& mr);
};

#endif
