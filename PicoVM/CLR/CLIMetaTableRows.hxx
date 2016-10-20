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
        NotPublic             =   0x00000000, // Class is not public scope.
        Public                =   0x00000001, // Class is public scope.
        NestedPublic          =   0x00000002, // Class is nested with public visibility.
        NestedPrivate         =   0x00000003, // Class is nested with private visibility.
        NestedFamily          =   0x00000004, // Class is nested with family visibility.
        NestedAssembly        =   0x00000005, // Class is nested with assembly visibility.
        NestedFamANDAssem     =   0x00000006, // Class is nested with family and assembly visibility.
        NestedFamORAssem      =   0x00000007, // Class is nested with family or assembly visibility.

        // Use this mask to retrieve class layout information.
        LayoutMask            =   0x00000018, 
        AutoLayout            =   0x00000000, // Class fields are auto-laid out.
        SequentialLayout      =   0x00000008, // Class fields are laid out sequentially.
        ExplicitLayout        =   0x00000010, // Layout is supplied explicitly.
        ClassSemanticsMask    =   0x00000060, // Use this mask to retrieve class semantics information.
        Class                 =   0x00000000, // Type is a class.
        Interface             =   0x00000020, // Type is an interface.

        // Special semantics in addition to class semantics.
        Abstract              =   0x00000080, // Class is abstract.
        Sealed                =   0x00000100, // Class is concrete and may not be extended.     
        SpecialName           =   0x00000400, // Class name is special. Name describes how.
        Import                =   0x00001000, // Class / interface is imported.
        Serializable          =   0x00002000, // The class is Serializable.

        // Use to retrieve string information for native interop.
        StringFormatMask      =   0x00030000,
        AnsiClass             =   0x00000000, // LPTSTR is interpreted as ANSI in this class.
        UnicodeClass          =   0x00010000, // LPTSTR is interpreted as UNICODE.
        AutoClass             =   0x00020000, // LPTSTR is interpreted automatically.
        CustomFormatClass     =   0x00030000, // A non-standard encoding specified by CustomFormatMask.

        // Use this mask to retrieve non-standard encoding information for native interop.
        CustomFormatMask      =   0x00C00000, // The meaning of the values of these 2 bits is unspecified.
        BeforeFieldInit       =   0x00100000, // Initialize the class any time before first static field access.
        Forwarder             =   0x00200000, // This ExportedType is a type forwarder.
        ReservedMask          =   0x00040800, // Flags reserved for runtime use.
        RTSpecialName         =   0x00000800, // Runtime should check name encoding.
        HasSecurity           =   0x00040000, // Class has security associate with it.
    };
};

struct FieldRow {
    // 2-byte bit mask of type FieldAttributes
    uint16_t flags;
    std::vector<uint16_t> name;
    std::vector<uint32_t> signature;

    enum FieldAttributes : uint16_t {
        // Use this mask to retrieve accessibility information.
        FieldAccessMask           =   0x0007,
        PrivateScope              =   0x0000, // Member not referenceable.
        Private                   =   0x0001, // Accessible only by the parent type.
        FamANDAssem               =   0x0002, // Accessible by sub-types only in this Assembly.
        Assembly                  =   0x0003, // Accessibly by anyone in the Assembly.
        Family                    =   0x0004, // Accessible only by type and sub-types.
        FamORAssem                =   0x0005, // Accessibly by sub-types anywhere, plus anyone in assembly.
        Public                    =   0x0006, // Accessibly by anyone who has visibility to this scope.

        // Field contract attributes.
        Static                    =   0x0010, // Defined on type, else per instance.
        InitOnly                  =   0x0020, // Field may only be initialized, not written to after init.
        Literal                   =   0x0040, // Value is compile time constant.
        NotSerialized             =   0x0080, // Field does not have to be serialized when type is remoted.
        SpecialName               =   0x0200, // Field is special. Name describes how.

        // Interop attributes
        PinvokeImpl               =   0x2000, // Implementation is forwarded through pinvoke.
        ReservedMask              =   0x9500, // Reserved flags for runtime use only.
        RTSpecialName             =   0x0400, // Runtime(metadata internal APIs) should check name encoding.
        HasFieldMarshal           =   0x1000, // Field has marshalling information.
        HasDefault                =   0x8000, // Field has default.
        HasFieldRVA               =   0x0100, // Field has RVA.
    };
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

    enum MethodAttribute : uint16_t {
        // Use this mask to retrieve accessibility information.
        MemberAccessMask          =   0x0007,
        PrivateScope              =   0x0000, // Member not referenceable.
        Private                   =   0x0001, // Accessible only by the parent type.     
        FamANDAssem               =   0x0002, // Accessible by sub-types only in this Assembly.     
        Assem                     =   0x0003, // Accessibly by anyone in the Assembly.     
        Family                    =   0x0004, // Accessible only by type and sub-types.     
        FamORAssem                =   0x0005, // Accessibly by sub-types anywhere, plus anyone in assembly.     
        Public                    =   0x0006, // Accessibly by anyone who has visibility to this scope.     

        // method contract attributes.
        Static                    =   0x0010, // Defined on type, else per instance.     
        Final                     =   0x0020, // Method may not be overridden.     
        Virtual                   =   0x0040, // Method virtual.     
        HideBySig                 =   0x0080, // Method hides by name+sig, else just by name.

        // Use this mask to retrieve vtable attributes.
        VtableLayoutMask          =   0x0100,
        ReuseSlot                 =   0x0000, // The default.
        NewSlot                   =   0x0100, // Method always gets a new slot in the vtable.

        // Method implementation attributes.
        CheckAccessOnOverride     =   0x0200, // Overridability is the same as the visibility.
        Abstract                  =   0x0400, // Method does not provide an implementation.   
        SpecialName               =   0x0800, // Method is special. Name describes how.    
       
        // Interop attributes
        PinvokeImpl               =   0x2000, // Implementation is forwarded through pinvoke.
        UnmanagedExport           =   0x0008, // Managed method exported via thunk to unmanaged code.
       
        // Reserved flags for runtime use only.
        ReservedMask              =   0xd000,
        RTSpecialName             =   0x1000, // Runtime should check name encoding.
        HasSecurity               =   0x4000, // Method has security associate with it.
        RequireSecObject          =   0x8000, // Method calls another method containing security code.
    };

    enum MethodImplAttributes : uint16_t {
        // Code impl mask
        CodeTypeMask      =   0x0003,   // Flags about code type.
        IL                =   0x0000,   // Method impl is IL.
        Native            =   0x0001,   // Method impl is native.
        OPTIL             =   0x0002,   // Method impl is OPTIL
        Runtime           =   0x0003,   // Method impl is provided by the runtime.

        // Managed mask
        ManagedMask       =   0x0004,   // Flags specifying whether the code is managed or unmanaged.
        Unmanaged         =   0x0004,   // Method impl is unmanaged, otherwise managed.
        Managed           =   0x0000,   // Method impl is managed.

        // Implementation info and interop
        ForwardRef        =   0x0010,   // Indicates method is defined; used primarily in merge scenarios.
        PreserveSig       =   0x0080,   // Indicates method sig is not to be mangled to do HRESULT conversion.
        InternalCall      =   0x1000,   // Reserved for internal use.
        Synchronized      =   0x0020,   // Method is single threaded through the body.
        NoInlining        =   0x0008,   // Method may not be inlined.
        MaxMethodImplVal  =   0xffff,   // Range check value
    };
};

struct ParamRow {
    uint16_t flags;
    // 2-byte bit mask of type ParamAttributes
    uint16_t sequence;
    std::vector<uint16_t> name;

    enum ParamAttributes : uint16_t {
        In                        =   0x0001,     // Param is [In]
        Out                       =   0x0002,     // Param is [out]
        Optional                  =   0x0010,     // Param is optional

        // Reserved flags for Runtime use only.
        ReservedMask              =   0xf000,
        HasDefault                =   0x1000,     // Param has default value.
        HasFieldMarshal           =   0x2000,     // Param has FieldMarshal.
        Unused                    =   0xcfe0,
    };
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

    enum EventAttribute : uint16_t {
        evSpecialName           =   0x0200, // Event is special. Name describes how.     
        // Reserved flags for Runtime use only.
        evReservedMask          =   0x0400,
        evRTSpecialName         =   0x0400, // Runtime(metadata internal APIs) should check name encoding. 
    };
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

    enum PropertyAttributes : uint16_t {
        SpecialName           =   0x0200, // Property is special. Name describes how.
        // Reserved flags for Runtime use only.
        ReservedMask          =   0xf400,
        RTSpecialName         =   0x0400, // Runtime(metadata internal APIs) should check name encoding.
        HasDefault            =   0x1000, // Property has default.
        Unused                =   0xe9ff,
    };
};

struct MethodSemanticsRow {
    // 2-byte bit mask of type MethodSemanticsAttributes
    uint16_t semantics;
    uint32_t method;
    std::pair<uint32_t, CliMetadataTableIndex> association;

    enum MethodSemanticsAttributes : uint16_t {
        Setter    =   0x0001,     // Setter for property
        Getter    =   0x0002,     // Getter for property
        Other     =   0x0004,     // other method for property or event
        AddOn     =   0x0008,     // AddOn method for event
        RemoveOn  =   0x0010,     // RemoveOn method for event
        Fire      =   0x0020,     // Fire method for event
    };
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

    enum PInvokeAttributes : uint16_t {
        NoMangle          = 0x0001,   

        // Use this mask to retrieve the CharSet information.
        CharSetMask       = 0x0006,
        CharSetNotSpec    = 0x0000,
        CharSetAnsi       = 0x0002,
        CharSetUnicode    = 0x0004,
        CharSetAuto       = 0x0006,

        BestFitUseAssem   = 0x0000,
        BestFitEnabled    = 0x0010,
        BestFitDisabled   = 0x0020,
        BestFitMask       = 0x0030,

        ThrowOnUnmappableCharUseAssem   = 0x0000,
        ThrowOnUnmappableCharEnabled    = 0x1000,
        ThrowOnUnmappableCharDisabled   = 0x2000,
        ThrowOnUnmappableCharMask       = 0x3000,

        SupportsLastError = 0x0040,   

        // Information about target function. Not relevant for fields.
        // None of the calling convention flags is relevant for fields.
        CallConvMask      = 0x0700,
        CallConvWinapi    = 0x0100,   
        // Pinvoke will use native callconv appropriate to target windows platform.
        CallConvCdecl     = 0x0200,
        CallConvStdcall   = 0x0300,
        CallConvThiscall  = 0x0400,   
        // In M9, pinvoke will raise exception.
        CallConvFastcall  = 0x0500,
        MaxValue          = 0xFFFF,
    };
};

struct FieldRVARow {
    uint32_t rva;
    uint32_t field;
};

struct AssemblyRow {
    // 4-byte constant of type AssemblyHashAlgorithm
    uint32_t hashAlgId;
    std::vector<uint16_t> version;
    // 4-byte bit mask of type AssemblyFlags
    uint32_t flags;
    std::vector<uint8_t> publicKey;
    std::vector<uint16_t> name;
    std::vector<uint16_t> culture;

    enum AssemblyFlags : uint16_t {
        PublicKey             =   0x0001,     // The assembly ref holds the full (unhashed) public key.
        PA_None               =   0x0000,     // Processor Architecture unspecified.
        PA_MSIL               =   0x0010,     // Processor Architecture: neutral (PE32).
        PA_x86                =   0x0020,     // Processor Architecture: x86 (PE32).
        PA_IA64               =   0x0030,     // Processor Architecture: Itanium (PE32+).
        PA_AMD64              =   0x0040,     // Processor Architecture: AMD X64 (PE32+).
        PA_Specified          =   0x0080,     // Propagate PA flags to AssemblyRef record.
        PA_Mask               =   0x0070,     // Bits describing the processor architecture.
        PA_FullMask           =   0x00F0,     // Bits describing the PA incl. Specified
        PA_Shift              =   0x0004,     // NOT A FLAG, shift count in PA flags <--> index conversion.
        EnableJITcompileTracking  =   0x8000, // From "DebuggableAttribute".
        DisableJITcompileOptimizer=   0x4000, // From "DebuggableAttribute".
        Retargetable          =   0x0100,     // The assembly can be retargeted (at runtime) to an assembly from a different publisher.
    };

    enum AssemblyHashAlgorithm : uint32_t {
		None				= 0,
		MD2					= 0x8001,
		MD4					= 0x8002,
		MD5					= 0x8003,
		SHA1				= 0x8004,
		MAC					= 0x8005,
		SSL3_SHAMD5			= 0x8008,
		HMAC				= 0x8009,
		TLS1PRF				= 0x800A,
		HASH_REPLACE_OWF	= 0x800B,
		SHA_256				= 0x800C,
		SHA_384				= 0x800D,
		SHA_512				= 0x800E,
	};
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

    enum FileAttributes {
        ContainsMetaData   = 0x0000, // This is not a resource file.
        ContainsNoMetaData = 0x0001, // This is a resource file or other non-metadata-containing file.
    };
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

    enum ManifestResourceAttributes {
        VisibilityMask        =   0x0007,
        Public                =   0x0001, // The Resource is exported from the Assembly.
        Private               =   0x0002, // The Resource is private to the Assembly.
    };
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

    enum GenericParamAttributes {
        // Variance of type parameters, only applicable to generic parameters 
        // for generic interfaces and delegates
        gpVarianceMask          =   0x0003,
        gpNonVariant            =   0x0000, 
        gpCovariant             =   0x0001,
        gpContravariant         =   0x0002,

        // Special constraints, applicable to any type parameters
        gpSpecialConstraintMask =  0x001C,
        gpNoSpecialConstraint   =   0x0000,      
        gpReferenceTypeConstraint = 0x0004, // type argument must be a reference type
        gpNotNullableValueTypeConstraint = 0x0008, // type argument must be a value type but not Nullable
        gpDefaultConstructorConstraint = 0x0010, // type argument must have a public
                                                // default constructor
    }
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
