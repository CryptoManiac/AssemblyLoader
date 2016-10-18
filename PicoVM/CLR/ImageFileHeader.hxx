#ifndef __IMAGE_FILE_HEADER__
#define __IMAGE_FILE_HEADER__

#include <cstdint>

struct ImageFileHeader {
	uint16_t machine;
	uint16_t sectionsCount;
	uint32_t timeDateStamp;
	uint32_t symbolTableOffset;
	uint32_t symbolsCount;
	uint16_t optionalHeaderSize;
	uint16_t characteristics;
};

#endif