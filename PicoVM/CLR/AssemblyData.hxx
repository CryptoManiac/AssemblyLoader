#ifndef __ASSEMBLYDATA_HXX__
#define __ASSEMBLYDATA_HXX__
#include <cstdint>
#include <vector>

#include "AssemblyReader.hxx"
#include "CLIMetadata.hxx"
#include "CLIMetadataTableRows.hxx"
#include "CLIMethodBody.hxx"
#include "CLISignature.hxx"

class AssemblyData
{
public:
    // Sections header
    std::vector<ImageSectionHeader> sections;

    // CLR header
    CLIHeader cliHeader;

    // CLI streams information
    CLIMetadata cliMetadata;

    // In-memory representation of CLI metadata tables
    struct CLIMetaDataTables {
        std::vector<ModuleRow> _Module;
        std::vector<TypeRefRow> _TypeRef;
        std::vector<TypeDefRow> _TypeDef;
        std::vector<FieldDefRow> _FieldDef;
        std::vector<MethodDefRow> _MethodDef;
        std::vector<ParamDefRow> _ParamDef;
        std::vector<InterfaceImplRow> _InterfaceImpl;
        std::vector<MemberRefRow> _MemberRef;
        std::vector<ConstantRow> _Constant;
        std::vector<CustomAttributeRow> _CustomAttribute;
        std::vector<FieldMarshalRow> _FieldMarshal;
        std::vector<DeclSecurityRow> _DeclSecurity;
        std::vector<ClassLayoutRow> _ClassLayout;
        std::vector<FieldLayoutRow> _FieldLayout;
        std::vector<std::vector<uint32_t> > _StandAloneSig;
        std::vector<EventMapRow> _EventMap;
        std::vector<EventRow> _Event;
        std::vector<PropertyMapRow> _PropertyMap;
        std::vector<PropertyRow> _Property;
        std::vector<MethodSemanticsRow> _MethodSemantics;
        std::vector<MethodImplRow> _MethodImpl;
        std::vector<std::u16string> _ModuleRef;
        std::vector<std::vector<uint32_t> > _TypeSpec;
        std::vector<ImplMapRow> _ImplMap;
        std::vector<FieldRVARow> _FieldRVA;
        std::vector<AssemblyRow> _Assembly;
        std::vector<uint32_t> _AssemblyProcessor;
        std::vector<AssemblyOSRow> _AssemblyOS;
        std::vector<AssemblyRefRow> _AssemblyRef;
        std::vector<AssemblyRefProcessorRow> _AssemblyRefProcessor;
        std::vector<AssemblyRefOSRow> _AssemblyRefOS;
        std::vector<FileRow> _File;
        std::vector<ExportedTypeRow> _ExportedType;
        std::vector<ManifestResourceRow> _ManifestResource;
        std::vector<NestedClassRow> _NestedClass;
        std::vector<GenericParamRow> _GenericParam;
        std::vector<MethodSpecRow> _MethodSpec;
        std::vector<GenericParamConstraintRow> _GenericParamConstraint;
    } cliMetaDataTables;

    AssemblyData() = delete;
    AssemblyData(std::string strFilePathName);
    AssemblyData(const std::vector<uint8_t>& assembly_bytes);

    AssemblyData(const AssemblyData& other) = default;
    AssemblyData(AssemblyData&& other) = default;

    AssemblyData& operator=(const AssemblyData& other);
    void swap(AssemblyData& other) noexcept;

    uint32_t getDataOffset(uint32_t address) const;
    size_t getMethodCount() const;
    const MethodDefRow& getMethodDef(uint32_t token) const;

    const Guid& getGUID() const;
    const std::u16string& getName() const;
    const std::vector<uint16_t>& getVersion() const;
    const std::vector<AssemblyRefRow>& getAssemblyRef() const;

private:
    // Reader instance
    AssemblyReader reader;
    // Image header
    ImageFileHeader fileHeader;

    void InitAssembly(); // called from constructor

    template<typename T1>
    void FillTable(MetadataRowsReader& mr, std::vector<T1>& table) {
        table.reserve(mr.mapTableLength[T1::tableID]);
        for (uint32_t n = 0; n < mr.mapTableLength[T1::tableID]; ++n) {
            T1 row(mr);
            table.push_back(row);
        }
    }

    void FillTables();
    void loadMethodBody(uint32_t index);
};

#endif
