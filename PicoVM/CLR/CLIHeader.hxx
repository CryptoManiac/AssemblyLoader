#ifndef __CLI_HEADER__
#define __CLI_HEADER__

#include <cstdint>
#include "ImageDataDirectory.hxx"

// CLR header structure
struct CLIHeader {

	// Constrained to sizeof(CLIHeader)
	uint32_t cb; 

	// Versioning
	uint16_t majorRuntime;
	uint16_t minorRuntime;

	// Symbol table and startup info
	ImageDataDirectory metaData;
	uint32_t flags;
	uint32_t entryPointToken;

	// Binding info
	ImageDataDirectory resources;
	ImageDataDirectory strongNameSignature;

	// Always set to zero
	ImageDataDirectory codeManagerTable;

	// Regular fixup
	ImageDataDirectory vTableFixups;

	// Always set to zero
	ImageDataDirectory exportAddresstableJumps;
	ImageDataDirectory managednativeImageHeader;
};

enum struct CLIHeaderFlags : uint32_t {
    ilOnly = 0x00000001,
    thirtyTwoBitOnly = 0x00000002,
    ilLibrary = 0x00000004,
    strongNameSigned = 0x00000008,
    nativeEntryPoint = 0x00000010,
    trackDebugData = 0x00010000,
    isIbcOptimized = 0x00020000, 
};

#endif // !__CLI_HEADER__
