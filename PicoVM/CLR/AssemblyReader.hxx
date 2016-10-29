#ifndef ASSEMBLYREADER_HXX
#define ASSEMBLYREADER_HXX
#include <vector>
#include <cstdint>

#include "utf8.h"
#include "crossguid/guid.hxx"
#include "Property.hxx"
#include "ImageNTHeader32.hxx"
#include "ImageNTHeader64.hxx"
#include "ImageSectionHeader.hxx"
#include "CLIHeader.hxx"

class AssemblyReader
{
public:
    AssemblyReader() = default;
    AssemblyReader(const std::vector<uint8_t>& data);
    AssemblyReader(const AssemblyReader& other) = default;
    AssemblyReader(AssemblyReader&& other) = default;

    AssemblyReader& operator=(const AssemblyReader& other);
    void swap(AssemblyReader& other) noexcept;

    void seek(uint32_t offset);
    void reset();

    // tell the current location
    uint32_t tell();

    // [] operator for better readability
    const uint8_t& operator[](uint32_t offset) const;

    // Read unsigned integer
    uint8_t  read_uint8();
    uint16_t read_uint16();
    uint16_t read_uint16(uint32_t offset) const;
    uint32_t read_uint32();
    uint32_t read_uint32(uint32_t offset) const;
    uint64_t read_uint64();
    uint64_t read_uint64(uint32_t offset) const;

    // Read ascii string
    uint32_t read_asciiz(std::string& result, uint32_t limit);

    // Read ascii string
    uint32_t read_asciiz(std::string& result, uint32_t offset, uint32_t limit) const;

    // Read utf8 string and convert it into utf16 encoding
    uint32_t read_utf8z(std::u16string& result, uint32_t limit);

    // Read utf8 string and convert it into utf16 encoding
    uint32_t read_utf8z(std::u16string& result, uint32_t offset, uint32_t limit) const;

    // Read unique ID
    void read_guid(Guid& result);
    void read_guid(Guid& result, uint32_t offset) const;

    // Read binary data
    void read_bytes(std::vector<uint8_t>& result, uint32_t length);
    void read_bytes(std::vector<uint8_t>& result, uint32_t offset, uint32_t length) const;

    // Read and decode variable length integer
    //   For additional information see CorSigUncompress data, declared in cor.h at line 2284.
    uint32_t read_varsize(uint32_t& code);

    // Read and decode variable length integer located at the specified offset
    uint32_t read_varsize(uint32_t& code, uint32_t offset) const;

    // Read and decode variable length integer (static version)
    static uint32_t read_varsize(uint32_t& code, const std::vector<uint8_t>& _data, uint32_t offset);

    // Load PE32 header from the specified offset
    void read_ntheader32(ImageNTHeader32& header32, uint32_t offset);

    // Load PE32+ header from the specified offset
    void read_ntheader64(ImageNTHeader64& header64, uint32_t offset);

    // Load section header from the specified offset
    void read_sectionheader(ImageSectionHeader& sectionheader, uint32_t offset);

    // Load CLI header from the specified offset
    void read_cliheader(CLIHeader& cliheader, uint32_t offset);

    // Read ImageDataDirectory item at the current location
    void read_directory(ImageDataDirectory& directory);
    void read_directory(ImageDataDirectory& directory, uint32_t offset) const;

private:
    std::vector<uint8_t> data;
    std::vector<uint8_t>::iterator pc;
};


#endif
