#ifndef ASSEMBLYREADER_HXX
#define ASSEMBLYREADER_HXX
#include <vector>
#include <cstdint>
#include <cstddef>
#include <algorithm>

#include "utf8.h"
#include "Property.hxx"
#include "ImageNTHeader32.hxx"
#include "ImageNTHeader64.hxx"
#include "ImageSectionHeader.hxx"
#include "CLIHeader.hxx"

class AssemblyReader
{
public:
    AssemblyReader() : data(0) {}
    AssemblyReader(const std::vector<uint8_t>& data) : data(data) { reset(); }
    void seek(uint32_t offset)
    {
        pc = next(data.begin(), offset);
    }

    void reset()
    {
        pc = data.begin();
    }

    ptrdiff_t tell()
    {
        return distance(data.begin(), pc);
    }

    const uint8_t& operator[](uint32_t offset) const
    {
        return data[offset];
    }

    uint16_t read_uint16()
    {
        return static_cast<uint16_t>(*(pc++)) | static_cast<uint16_t>(*(pc++)) << 8;
    }

    uint16_t read_uint16(uint32_t offset) const
    {
        return static_cast<uint16_t>(data[offset]) | static_cast<uint16_t>(data[offset + 1]) << 8;
    }

    uint32_t read_uint32()
    {
        return static_cast<uint32_t>(*(pc++)) |
               static_cast<uint32_t>(*(pc++)) << 8 |
               static_cast<uint32_t>(*(pc++)) << 16 |
               static_cast<uint32_t>(*(pc++)) << 24;
    }

    uint32_t read_uint32(uint32_t offset) const
    {
        auto it = next(data.cbegin(), offset);
        return static_cast<uint32_t>(*(it++)) |
               static_cast<uint32_t>(*(it++)) << 8 |
               static_cast<uint32_t>(*(it++)) << 16 |
               static_cast<uint32_t>(*(it++)) << 24;
    }

    uint64_t read_uint64()
    {
        return static_cast<uint64_t>(*(pc++)) |
               static_cast<uint64_t>(*(pc++)) << 8 |
               static_cast<uint64_t>(*(pc++)) << 16 |
               static_cast<uint64_t>(*(pc++)) << 24 |
               static_cast<uint64_t>(*(pc++)) << 32 |
               static_cast<uint64_t>(*(pc++)) << 40 |
               static_cast<uint64_t>(*(pc++)) << 48 |
               static_cast<uint64_t>(*(pc++)) << 56;
    }

    uint64_t read_uint64(uint32_t offset) const
    {
        auto it = next(data.cbegin(), offset);
        return static_cast<uint64_t>(*(it++)) |
               static_cast<uint64_t>(*(it++)) << 8 |
               static_cast<uint64_t>(*(it++)) << 16 |
               static_cast<uint64_t>(*(it++)) << 24 |
               static_cast<uint64_t>(*(it++)) << 32 |
               static_cast<uint64_t>(*(it++)) << 40 |
               static_cast<uint64_t>(*(it++)) << 48 |
               static_cast<uint64_t>(*(it++)) << 56;
    }

    ptrdiff_t read_asciiz(std::vector<uint8_t>& result, ptrdiff_t limit)
    {
        ptrdiff_t offset = distance(data.begin(), pc);
        ptrdiff_t read = read_asciiz(result, offset, limit);
        pc = next(pc, read);
        return read;
    }

    ptrdiff_t read_asciiz(std::vector<uint8_t>& result, ptrdiff_t offset, ptrdiff_t limit) const
    {
        auto start_it = next(data.cbegin(), offset);
        auto end_it = std::find(start_it, data.cend(), 0);
        if (distance(start_it, end_it) >= limit) {
            end_it = next(start_it, limit);
        }
        result.clear();
        result.assign(start_it, end_it);
        return distance(start_it, end_it);
    }

    ptrdiff_t read_utf8z(std::vector<uint16_t>& result, uint32_t limit)
    {
        ptrdiff_t offset = distance(data.begin(), pc);
        ptrdiff_t read = read_utf8z(result, offset, limit);
        pc = next(pc, read);
        return read;
    }

    ptrdiff_t read_utf8z(std::vector<uint16_t>& result, ptrdiff_t offset, ptrdiff_t limit) const
    {
        auto start_it = next(data.cbegin(), offset);
        auto end_it = std::find(start_it, data.cend(), 0);
        if (distance(start_it, end_it) >= limit) {
            end_it = next(start_it, limit);
        }

        result.clear();
        utf8::utf8to16(start_it, end_it, back_inserter(result));
        return distance(start_it, end_it);
    }

    void read_guid(std::vector<uint8_t>& result)
    {
        result.clear();
        result.assign(pc, next(pc, 16));
        pc = next(pc, 16);
    }

    void read_guid(std::vector<uint8_t>& result, ptrdiff_t offset) const
    {
        auto start_it = next(data.cbegin(), offset);
        result.clear();
        result.assign(start_it, next(start_it, 16));
    }

    void read_bytes(std::vector<uint8_t>& result, ptrdiff_t offset, uint32_t length) const
    {
        result.clear();
        auto start_it = next(data.cbegin(), offset);
        result.assign(start_it, next(start_it, length));
    }

    ptrdiff_t read_varsize(uint32_t& code)
    {
        auto it = pc;
        uint8_t b1 = *(pc++);

        if ((b1 & 0x80) == 0) {
            code = b1;
        } else if ((b1 & 0xC0) == 0x80) {
            code = (b1 & 0x3F) << 8;
            code |= *(pc++);
        } else if ((b1 & 0xE0) == 0xC0) {
            code = (b1 & 0x1F) << 24;
            code |= *(pc++) << 16;
            code |= *(pc++) << 8;
            code |= *(pc++);
        } else {
            //We don't recognize this encoding
            throw std::runtime_error("Invalid signature");
        }

        return distance(it, pc);
    }

    ptrdiff_t read_varsize(uint32_t& code, ptrdiff_t offset) const
    {
        auto it_start = next(data.cbegin(), offset);
        auto it = it_start;
        uint8_t b1 = *(it++);

        if ((b1 & 0x80) == 0) {
            code = b1;
        } else if ((b1 & 0xC0) == 0x80) {
            code = (b1 & 0x3F) << 8;
            code |= *(it++);
        } else if ((b1 & 0xE0) == 0xC0) {
            code = (b1 & 0x1F) << 24;
            code |= *(it++) << 16;
            code |= *(it++) << 8;
            code |= *(it++);
        } else {
            //We don't recognize this encoding
            throw std::runtime_error("Invalid signature");
        }

        return distance(it_start, it);
    }

    static ptrdiff_t read_varsize(uint32_t& code, const std::vector<uint8_t>& data, ptrdiff_t offset)
    {
        auto it_start = next(data.cbegin(), offset);
        auto it = it_start;
        uint8_t b1 = *(it++);

        if ((b1 & 0x80) == 0) {
            code = b1;
        }
        else if ((b1 & 0xC0) == 0x80) {
            code = (b1 & 0x3F) << 8;
            code |= *(it++);
        }
        else if ((b1 & 0xE0) == 0xC0) {
            code = (b1 & 0x1F) << 24;
            code |= *(it++) << 16;
            code |= *(it++) << 8;
            code |= *(it++);
        }
        else {
            //We don't recognize this encoding
            throw std::runtime_error("Invalid signature");
        }

        return distance(it_start, it);
    }

    void read_ntheader32(ImageNTHeader32& header32, uint32_t offset)
    {
        header32 = *reinterpret_cast<ImageNTHeader32*>(&(*(data.begin() + offset)));
    }

    void read_ntheader64(ImageNTHeader64& header64, uint32_t offset)
    {
        header64 = *reinterpret_cast<ImageNTHeader64*>(&(*(data.begin() + offset)));
    }

    void read_sectionheader(ImageSectionHeader& sectionheader, uint32_t offset)
    {
        sectionheader = *reinterpret_cast<ImageSectionHeader*>(&(*(data.begin() + offset)));
    }

    void read_cliheader(CLIHeader& cliheader, uint32_t offset)
    {
        cliheader = *reinterpret_cast<CLIHeader*>(&(*(data.begin() + offset)));
    }

    void read_directory(ImageDataDirectory& directory)
    {
        directory.rva = read_uint32();
        directory.size = read_uint32();
    }

    void read_directory(ImageDataDirectory& directory, uint32_t offset) const
    {
        directory.rva = read_uint32(offset);
        directory.size = read_uint32(offset + 4);
    }

private:
    std::vector<uint8_t> data;
    std::vector<uint8_t>::iterator pc;
};


#endif
