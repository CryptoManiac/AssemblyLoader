cmake_minimum_required( VERSION 2.8 )

project(PicoVM)

set( SRC_DIR ${PROJECT_SOURCE_DIR} )

set( CMAKE_BUILD_TYPE DEBUG )

set ( CMAKE_CXX_FLAGS_DEBUG "-g -std=c++11 -Wall -Wextra")

set( APP_NAME picovm )

include_directories(CLR/)

set ( HEADERS_ONLY
        CLIHeader.hxx
        EnumCasting.hxx
        EvaluationStackItem.hxx
        Formatting.hxx
        ImageDataDirectory.hxx
        ImageFileHeader.hxx
        ImageNTHeader32.hxx
        ImageNTHeader64.hxx
        ImageOptionalHeader32.hxx
        ImageOptionalHeader64.hxx
        ImageSectionHeader.hxx
        Property.hxx
        utf8.h
        utf8/checked.h
        utf8/core.h
        utf8/unchecked.h
    )

list( APPEND SRC ${SRC_DIR}/main.cxx )
list( APPEND SRC ${SRC_DIR}/CLR/AppDomain.cxx )
list( APPEND SRC ${SRC_DIR}/CLR/AssemblyData.cxx )
list( APPEND SRC ${SRC_DIR}/CLR/AssemblyReader.cxx )
list( APPEND SRC ${SRC_DIR}/CLR/CLIElementTypes.cxx )
list( APPEND SRC ${SRC_DIR}/CLR/CLIMetadata.cxx )
list( APPEND SRC ${SRC_DIR}/CLR/CLIMetadataTableIndex.cxx )
list( APPEND SRC ${SRC_DIR}/CLR/CLIMetadataTableRows.cxx )
list( APPEND SRC ${SRC_DIR}/CLR/CLIMethodBody.cxx )
list( APPEND SRC ${SRC_DIR}/CLR/CLISignature.cxx )
list( APPEND SRC ${SRC_DIR}/CLR/HexStr.cxx )
list( APPEND SRC ${SRC_DIR}/CLR/crossguid/guid.cxx )

add_executable ( ${APP_NAME} ${SRC} )