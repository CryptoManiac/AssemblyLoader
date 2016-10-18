#ifndef __IMAGE_NT_HEADER64__
#define __IMAGE_NT_HEADER64__

#include "ImageFileHeader.hxx"
#include "ImageOptionalHeader64.hxx"

struct ImageNTHeader64 {
	uint32_t signature; // PE\0\0
	ImageFileHeader fileHeader;
	ImageOptionalHeader64 optionalHeader;
};

#endif