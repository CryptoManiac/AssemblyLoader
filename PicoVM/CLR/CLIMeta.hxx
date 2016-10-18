#ifndef CLIMETA_HXX
#define CLIMETA_HXX
#include <string>
#include <stdexcept>

using namespace std;
enum CliMetadataTableIndex : uint8_t {
    Module = 0x00,
    TypeRef = 0x01,
    TypeDef = 0x02,
    Field = 0x04,
    Method = 0x06,
    Param = 0x08,
    InterfaceImpl = 0x09,
    MemberRef = 0x0A,
    Constant = 0x0B,
    CustomAttribute = 0x0C,
    FieldMarshal = 0x0D,
    DeclSecurity = 0x0E,
    ClassLayout = 0x0F,
    FieldLayout = 0x10,
    StandAloneSig = 0x11,
    EventMap = 0x12,
    Event = 0x14,
    PropertyMap = 0x15,
    Property = 0x17,
    MethodSemantics = 0x18,
    MethodImpl = 0x19,
    ModuleRef = 0x1A,
    TypeSpec = 0x1B,
    ImplMap = 0x1C,
    FieldRVA = 0x1D,
    Assembly = 0x20,
    AssemblyProcessor = 0x21,
    AssemblyOS = 0x22,
    AssemblyRef = 0x23,
    AssemblyRefProcessor = 0x24,
    AssemblyRefOS = 0x25,
    File = 0x26,
    ExportedType = 0x27,
    ManifestResource = 0x28,
    NestedClass = 0x29,
    GenericParam = 0x2A,
    MethodSpec = 0x2B,
    GenericParamConstraint = 0x2C,
    End = 0xFF
};

// Beware, brain-compiled code ahead! 
CliMetadataTableIndex& operator++(CliMetadataTableIndex& value)
{
    switch(value) {
        case Module : return value = TypeRef;
        case TypeRef : return value = TypeDef;
        case TypeDef : return value = Field;
        case Field : return value = Method;
        case Method : return value = Param;
        case Param : return value = InterfaceImpl;
        case InterfaceImpl : return value = MemberRef;
        case MemberRef : return value = Constant;
        case Constant : return value = CustomAttribute;
        case CustomAttribute : return value = FieldMarshal;
        case FieldMarshal : return value = DeclSecurity;
        case DeclSecurity : return value = ClassLayout;
        case ClassLayout : return value = FieldLayout;
        case FieldLayout : return value = StandAloneSig;
        case StandAloneSig : return value = EventMap;
        case EventMap : return value = Event;
        case Event : return value = PropertyMap;
        case PropertyMap : return value = Property;
        case Property : return value = MethodSemantics;
        case MethodSemantics : return value = MethodImpl;
        case MethodImpl : return value = ModuleRef;
        case ModuleRef : return value = TypeSpec;
        case TypeSpec : return value = ImplMap;
        case ImplMap : return value = FieldRVA;
        case FieldRVA : return value = Assembly;
        case Assembly : return value = AssemblyProcessor;
        case AssemblyProcessor : return value = AssemblyOS;
        case AssemblyOS : return value = AssemblyRef;
        case AssemblyRef : return value = AssemblyRefProcessor;
        case AssemblyRefProcessor : return value = AssemblyRefOS;
        case AssemblyRefOS : return value = File;
        case File : return value = ExportedType;
        case ExportedType : return value = ManifestResource;
        case ManifestResource : return value = NestedClass;
        case NestedClass : return value = GenericParam;
        case GenericParam : return value = MethodSpec;
        case MethodSpec : return value = GenericParamConstraint;
        case GenericParamConstraint : return value = End;
        default: {}
    }
    throw runtime_error("Unknown table");
}

string getTableName(CliMetadataTableIndex value) {
    switch (value) {
        case Module: return "Module";
        case TypeRef: return "TypeRef";
        case TypeDef: return "TypeDef";
        case Field: return "Field";
        case Method: return "Method";
        case Param: return "Param";
        case InterfaceImpl: return "InterfaceImpl";
        case MemberRef: return "MemberRef";
        case CustomAttribute: return "CustomAttribute";
        case FieldMarshal: return "FieldMarshal";
        case DeclSecurity: return "DeclSecurity";
        case ClassLayout: return "ClassLayout";
        case FieldLayout: return "FieldLayout";
        case StandAloneSig: return "StandAloneSig";
        case EventMap: return "EventMap";
        case Event: return "Event";
        case PropertyMap: return "PropertyMap";
        case Property: return "Property";
        case MethodSemantics: return "MethodSemantics";
        case MethodImpl: return "MethodImpl";
        case ModuleRef: return "ModuleRef";
        case TypeSpec: return "TypeSpec";
        case ImplMap: return "ImplMap";
        case FieldRVA: return "FieldRVA";
        case Assembly: return "Assembly";
        case AssemblyProcessor: return "AssemblyProcessor";
        case AssemblyOS: return "AssemblyOS";
        case AssemblyRef: return "AssemblyRef";
        case AssemblyRefProcessor: return "AssemblyRefProcessor";
        case AssemblyRefOS: return "AssemblyRefOS";
        case File: return "File";
        case ExportedType: return "ExportedType";
        case ManifestResource: return "ManifestResource";
        case NestedClass: return "NestedClass";
        case GenericParam: return "GenericParam";
        case MethodSpec: return "MethodSpec";
        case GenericParamConstraint: return "GenericParamConstraint";
        default: return "Unknown table";
    }
}
#endif
