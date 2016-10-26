#ifndef __CLIElementTypeS_HXX__
#define __CLIElementTypeS_HXX__
#include <cstdint>
#include <map>

enum struct CLIElementType : uint8_t {
    ELEMENT_TYPE_END=0x00, // Marks end of a list
    ELEMENT_TYPE_VOID=0x01, // System.Void
    ELEMENT_TYPE_BOOLEAN=0x02, // System.Boolean
    ELEMENT_TYPE_CHAR=0x03, // System.Char
    ELEMENT_TYPE_I1=0x04, // System.Sbyte
    ELEMENT_TYPE_U1=0x05, // System.Byte
    ELEMENT_TYPE_I2=0x06, // System.Int16
    ELEMENT_TYPE_U2=0x07, // System.UInt16
    ELEMENT_TYPE_I4=0x08, // System.Int32
    ELEMENT_TYPE_U4=0x09, // System.UInt32
    ELEMENT_TYPE_I8=0x0a, // System.Int64
    ELEMENT_TYPE_U8=0x0b, // System.UInt64
    ELEMENT_TYPE_R4=0x0c, // System.Float
    ELEMENT_TYPE_R8=0x0d, // System/Double
    ELEMENT_TYPE_STRING=0x0e, // System.String
    ELEMENT_TYPE_PTR=0x0f, // Followed by type
    ELEMENT_TYPE_BYREF=0x10, // Followed by type
    ELEMENT_TYPE_VALUETYPE=0x11, // Followed by TypeDef or TypeRef token
    ELEMENT_TYPE_CLASS=0x12, // Followed by TypeDef or TypeRef token
    ELEMENT_TYPE_VAR=0x13, // Generic parameter in a generic type definition, represented as number (compressed unsigned integer)
    ELEMENT_TYPE_ARRAY=0x14, // type rank boundsCount bound1 ... loCount lo1 ...
    ELEMENT_TYPE_GENERICINST=0x15, // Generic type instantiation. Followed by type type-arg-count type-1 ... type-n
    ELEMENT_TYPE_TYPEDBYREF=0x16, // System.TypedReference
    ELEMENT_TYPE_I=0x18, // System.IntPtr
    ELEMENT_TYPE_U=0x19, // System.UIntPtr
    ELEMENT_TYPE_FNPTR=0x1b, // Followed by full method signature
    ELEMENT_TYPE_OBJECT=0x1c, // System.Object
    ELEMENT_TYPE_SZARRAY=0x1d, // Single-dim array with 0 lower bound
    ELEMENT_TYPE_MVAR=0x1e, // Generic parameter in a generic method definition, represented as number (compressed unsigned integer)
    ELEMENT_TYPE_CMOD_REQD=0x1f, // Required modifier : followed by a TypeDef or TypeRef token
    ELEMENT_TYPE_CMOD_OPT=0x20, // Optional modifier : followed by a TypeDef or TypeRef token
    ELEMENT_TYPE_INTERNAL=0x21, // Implemented within the CLI
    ELEMENT_TYPE_MODIFIER=0x40, // Or’d with following element types
    ELEMENT_TYPE_SENTINEL=0x01|ELEMENT_TYPE_MODIFIER, // Sentinel for vararg method signature
    ELEMENT_TYPE_PINNED=0x05|ELEMENT_TYPE_MODIFIER, // Denotes a local variable that points at a pinned object
    ELEMENT_TYPE_R4_HFA= 0x06|ELEMENT_TYPE_MODIFIER, // used only internally for R4 HFA types
    ELEMENT_TYPE_R8_HFA= 0x07|ELEMENT_TYPE_MODIFIER, // used only internally for R8 HFA types
    ELEMENT_TYPE_SYSTEM_TYPE=0x50, // Indicates an argument of type System.Type.
    ELEMENT_TYPE_BOXED_OBJECT=0x51, // Used in custom attributes to specify a boxed object 
    ELEMENT_TYPE_RESERVED=0x52, // Reserved
    ELEMENT_TYPE_FIELD=0x53, // Used in custom attributes to indicate a FIELD
    ELEMENT_TYPE_PROPERTY=0x54, // Used in custom attributes to indicate a PROPERTY
    ELEMENT_TYPE_ENUM=0x55 // Used in custom attributes to specify an enum
};

const std::map<CLIElementType, std::string> cliElementTypeNames {
    { CLIElementType::ELEMENT_TYPE_END, "END" },
    { CLIElementType::ELEMENT_TYPE_VOID, "VOID" },
    { CLIElementType::ELEMENT_TYPE_BOOLEAN, "BOOLEAN" },
    { CLIElementType::ELEMENT_TYPE_CHAR, "CHAR" },
    { CLIElementType::ELEMENT_TYPE_I1, "I1" },
    { CLIElementType::ELEMENT_TYPE_U1, "U1" },
    { CLIElementType::ELEMENT_TYPE_I2, "I2" },
    { CLIElementType::ELEMENT_TYPE_U2, "U2" },
    { CLIElementType::ELEMENT_TYPE_I4, "I4" },
    { CLIElementType::ELEMENT_TYPE_U4, "U4" },
    { CLIElementType::ELEMENT_TYPE_I8, "I8" },
    { CLIElementType::ELEMENT_TYPE_U8, "U8" },
    { CLIElementType::ELEMENT_TYPE_R4, "R4" },
    { CLIElementType::ELEMENT_TYPE_R8, "R8" },
    { CLIElementType::ELEMENT_TYPE_STRING, "STRING" },
    { CLIElementType::ELEMENT_TYPE_PTR, "PTR" },
    { CLIElementType::ELEMENT_TYPE_BYREF, "BYREF" },
    { CLIElementType::ELEMENT_TYPE_VALUETYPE, "VALUETYPE" },
    { CLIElementType::ELEMENT_TYPE_CLASS, "CLASS" },
    { CLIElementType::ELEMENT_TYPE_VAR, "VAR" },
    { CLIElementType::ELEMENT_TYPE_ARRAY, "ARRAY" },
    { CLIElementType::ELEMENT_TYPE_GENERICINST, "GENERICINST" },
    { CLIElementType::ELEMENT_TYPE_TYPEDBYREF, "TYPEDBYREF" },
    { CLIElementType::ELEMENT_TYPE_I, "I" },
    { CLIElementType::ELEMENT_TYPE_U, "U" },
    { CLIElementType::ELEMENT_TYPE_FNPTR, "FNPTR" },
    { CLIElementType::ELEMENT_TYPE_OBJECT, "OBJECT" },
    { CLIElementType::ELEMENT_TYPE_SZARRAY, "SZARRAY" },
    { CLIElementType::ELEMENT_TYPE_MVAR, "MVAR" },
    { CLIElementType::ELEMENT_TYPE_CMOD_REQD, "CMOD_REQD" },
    { CLIElementType::ELEMENT_TYPE_CMOD_OPT, "CMOD_OPT" },
    { CLIElementType::ELEMENT_TYPE_INTERNAL, "INTERNAL" },
    { CLIElementType::ELEMENT_TYPE_MODIFIER, "MODIFIER" },
    { CLIElementType::ELEMENT_TYPE_SENTINEL, "SENTINEL" },
    { CLIElementType::ELEMENT_TYPE_PINNED, "PINNED" },
    { CLIElementType::ELEMENT_TYPE_SYSTEM_TYPE, "SYSTEM_TYPE" },
    { CLIElementType::ELEMENT_TYPE_BOXED_OBJECT, "BOXED_OBJECT" },
    { CLIElementType::ELEMENT_TYPE_RESERVED, "RESERVED" },
    { CLIElementType::ELEMENT_TYPE_FIELD, "FIELD" },
    { CLIElementType::ELEMENT_TYPE_PROPERTY, "PROPERTY" },
    { CLIElementType::ELEMENT_TYPE_ENUM, "ENUM" },
};

std::string getTypeName(const CLIElementType &elementType);

#endif
