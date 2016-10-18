#ifndef __IMAGE_OPTIONAL_HEADER64__
#define __IMAGE_OPTIONAL_HEADER64__

#include "ImageDataDirectory.hxx"

const uint32_t ImageOptionalDirectoriesNumber64 = 16;

struct ImageOptionalHeader64 {
	struct OptionalStandard {
		uint16_t magic;
		uint8_t majorLinkerVersion;
		uint8_t minorLinkerVersion;
		uint32_t codeSize;
		uint32_t initializedDataSize;
		uint32_t uninitializedDataSize;
		uint32_t entryPointRVA;  //The RVA of the code entry point
		uint32_t baseOfCode;
	} standard;

	struct OptionalNT {
		uint64_t imageBase;
		uint32_t sectionAlignment;
		uint32_t fileAlignment;
		uint16_t majorOSVersion;
		uint16_t minorOSVersion;
		uint16_t majorImageVersion;
		uint16_t minorImageVersion;
		uint16_t majorSubsystemVersion;
		uint16_t minorSubsystemVersion;
		uint32_t win32VersionValue;
		uint32_t imageSize;
		uint32_t headerSize;
		uint32_t checksum;
		uint16_t subSystem;
		uint16_t dllCharacteristics;
		uint64_t stackReserveSize;
		uint64_t stackCommitSize;
		uint64_t heapReserveSize;
		uint64_t heapCommitSize;
		uint32_t loaderFlags;
		uint32_t numberOfRvaAndSizes; // Constrained at 16

		ImageDataDirectory directories[ImageOptionalDirectoriesNumber64];
	} nt;
};

#endif
