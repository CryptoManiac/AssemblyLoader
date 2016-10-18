#ifndef __IMAGE_NT_HEADER32__
#define __IMAGE_NT_HEADER32__

#include <vector>

#include "ImageFileHeader.hxx"
#include "ImageOptionalHeader32.hxx"

struct ImageNTHeader32 {
	uint32_t signature; // PE\0\0
	ImageFileHeader fileHeader;
	ImageOptionalHeader32 optionalHeader;
};


#endif