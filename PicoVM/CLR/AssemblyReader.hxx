#ifndef ASSEMBLYREADER_HXX
#define ASSEMBLYREADER_HXX
#include <vector>
#include <cstdint>
#include <algorithm>

#include "utf8.h"
#include "Property.hxx"
#include "ImageNTHeader32.hxx"
#include "ImageNTHeader64.hxx"
#include "ImageSectionHeader.hxx"
#include "CLIHeader.hxx"

class AssemblyReader {
    public:
        AssemblyReader() : data(0) { }
        AssemblyReader(const std::vector<uint8_t>& data) : data(data) { }

        const uint8_t& operator[](uint32_t offset) const {
            return data[offset];
        }

        uint16_t read_uint16(uint32_t offset) const {
			return static_cast<uint16_t>(data[offset]) | static_cast<uint16_t>(data[offset + 1]) << 8;
        }
        
        uint32_t read_uint32(uint32_t offset) const {
			auto it = next(data.cbegin(), offset);
			return  static_cast<uint32_t>(*(it++)) |
                    static_cast<uint32_t>(*(it++)) << 8 |
                    static_cast<uint32_t>(*(it++)) << 16 |
                    static_cast<uint32_t>(*(it++)) << 24;
        }

        uint64_t read_uint64(uint32_t offset) const {
			auto it = next(data.cbegin(), offset);
			return  static_cast<uint64_t>(*(it++)) |
                    static_cast<uint64_t>(*(it++)) << 8 |
                    static_cast<uint64_t>(*(it++)) << 16 |
                    static_cast<uint64_t>(*(it++)) << 24 |
                    static_cast<uint64_t>(*(it++)) << 32 |
                    static_cast<uint64_t>(*(it++)) << 40 |
                    static_cast<uint64_t>(*(it++)) << 48 |
                    static_cast<uint64_t>(*(it++)) << 56;
        }

        uint32_t read_asciiz(std::vector<uint8_t>& result, uint32_t offset, uint32_t limit = 8) const {
			auto start_it = next(data.cbegin(), offset);
			auto end_it = std::find(start_it, data.cend(), 0);
			if (distance(start_it, end_it) >= limit) {
				end_it = next(start_it, limit);
			}
			result.clear();
            result.assign(start_it, end_it);
            return distance(start_it, end_it);
        }

        uint32_t read_utf8z(std::vector<uint16_t>& result, uint32_t offset, uint32_t limit = 0xffff) const {
            auto start_it = next(data.cbegin(), offset);
            auto end_it = std::find(start_it, data.cend(), 0);
			if (distance(start_it, end_it) >= limit) {
				end_it = next(start_it, limit);
			}

            result.clear();
            utf8::utf8to16(start_it, end_it, back_inserter(result));
            return end_it - start_it;
        }

        void read_guid(std::vector<uint8_t>& result, uint32_t offset) const {
            auto start_it = next(data.cbegin(), offset);
            result.clear();
            result.assign(start_it, next(start_it, 16));
        }

        uint32_t read_varsize(uint32_t& code, uint32_t offset) const {
			auto it = data.cbegin();
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

            return distance(data.cbegin(), it);
        }

		void read_ntheader32(ImageNTHeader32& header32, uint32_t offset) {
			header32 = *reinterpret_cast<ImageNTHeader32*>(&(*(data.begin() + offset)));
		}

		void read_ntheader64(ImageNTHeader64& header64, uint32_t offset) {
			header64 = *reinterpret_cast<ImageNTHeader64*>(&(*(data.begin() + offset)));
		}

		void read_sectionheader(ImageSectionHeader& sectionheader, uint32_t offset) {
			sectionheader = *reinterpret_cast<ImageSectionHeader*>(&(*(data.begin() + offset)));
		}

		void read_cliheader(CLIHeader& cliheader, uint32_t offset) {
			cliheader = *reinterpret_cast<CLIHeader*>(&(*(data.begin() + offset)));
		}

        void read_directory(ImageDataDirectory& directory, uint32_t offset) const {
            directory.rva = read_uint32(offset);
            directory.size = read_uint32(offset + 4);
        }

    private:
        std::vector<uint8_t> data;
};



#endif
