# Default: g++ , as option USE_CLANG=1 or USE_MINGW=1 or USE_ICPC=1
CXX=g++
CLSPECIFIC=-Og -Winline

EXEC=picovm

ifeq (${USE_CLANG}, 1)
    CXX=clang++
    CLSPECIFIC=
endif
ifeq (${USE_MINGW}, 1)
    CXX=x86_64-w64-mingw32-g++ -static -DWIN32=1
    CLSPECIFIC=-Og
    EXEC:=$(EXEC).exe
endif
ifeq (${USE_ICPC}, 1)
    CXX=icpc
    CLSPECIFIC=
endif

CXXFLAGS=-g -std=c++11 -Wall -Wextra -Wshadow $(CLSPECIFIC)

SOURCES=\
	PicoVM/main.cxx \
	$(wildcard PicoVM/CLR/*.cxx) \
	PicoVM/CLR/crossguid/guid.cxx
INCDIRS=PicoVM/CLR/
OBJECTS=$(SOURCES:.cxx=.o)

.PHONY: clean

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -I $(INCDIRS) -c -o $@ $<

$(VERBOSE).SILENT: clean

clean:
	-rm -rf $(EXEC) $(OBJECTS) $(EXEC).exe