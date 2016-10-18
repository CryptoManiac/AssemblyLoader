#ifndef __IMAGE_SECTIONHEADER__
#define __IMAGE_SECTIONHEADER__

#include <cstdint>

// Item of section table
struct ImageSectionHeader {
	uint8_t  name[8];
    uint32_t virtualSize;
	uint32_t virtualAddress;
	uint32_t sizeOfRawData;
	uint32_t pointerToRawData;
	uint32_t pointerToRelocations;
	uint32_t pointerToLinenumbers;
	uint16_t relocationsCount;
	uint16_t linenumbersCount;
	uint32_t characteristics;
};

#endif
