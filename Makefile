ifeq (${USE_CLANG}, 1)
    CXX=clang++
else
    CXX=g++
endif

CXXFLAGS=-g -std=c++11 -Wall -Wextra -pedantic

EXEC=picovm
SOURCES= \
         PicoVM/main.cxx \
         PicoVM/CLR/AppDomain.cxx \
         PicoVM/CLR/AssemblyData.cxx \
         PicoVM/CLR/CLIMetadata.cxx \
         PicoVM/CLR/ExecutionThread.cxx \
         PicoVM/CLR/CLIMetadataTableIndex.cxx \
         PicoVM/CLR/CLIMetadataTableRows.cxx \
         PicoVM/CLR/CLIElementTypes.cxx \
         PicoVM/CLR/CLISignature.cxx \
         PicoVM/CLR/AssemblyReader.cxx \
         PicoVM/CLR/CLIMethodBody.cxx \
         PicoVM/CLR/crossguid/guid.cxx \
         PicoVM/CLR/HexStr.cxx
INCDIRS=PicoVM/CLR/
OBJECTS=$(SOURCES:.cxx=.o)

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -I $(INCDIRS) -c -o $@ $<

$(VERBOSE).SILENT: clean

clean:
	-rm -rf $(EXEC) $(OBJECTS)