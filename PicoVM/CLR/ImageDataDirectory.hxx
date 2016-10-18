#ifndef __DATA_DIRECTORY__
#define __DATA_DIRECTORY__

#include <cstdint>

struct ImageDataDirectory {
	uint32_t rva;
	uint32_t size;
};

enum ImageDirectoryType : uint32_t {
	exportTable = 0,
	importTable = 1,
	resourceDirectory = 2,
	exceptionDirectory = 3,
	securityDirectory = 4,
	baseRelocationTable = 5,
	debugDirectory = 6,
	architecture = 7,
	globalPtr = 8,
	tls = 9,
	config = 10,
	boundImport = 11,
	iat = 12,
	delayImport = 13,
	cliHeader = 14,
	reserved = 15
};

#endif
