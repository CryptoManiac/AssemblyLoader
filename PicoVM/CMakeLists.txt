cmake_minimum_required( VERSION 2.8 )

project(PicoVM)

set( SRC_DIR ${PROJECT_SOURCE_DIR} )

set( CMAKE_BUILD_TYPE DEBUG )

set( CMAKE_CXX_FLAGS_DEBUG "-g -Og -std=c++11 -Wall -Wextra")

set( APP_NAME picovm )

include_directories( CLR/ )

set( HEADERS_ONLY

        CLIHeader
        EnumCasting
        EvaluationStackItem
        Formatting
        ImageDataDirectory
        ImageFileHeader
        ImageNTHeader32
        ImageNTHeader64
        ImageOptionalHeader32
        ImageOptionalHeader64
        ImageSectionHeader
        Property
        utf8
   )

list( APPEND SRC ${SRC_DIR}/main.cxx )

set( OUR_SRC

        AppDomain
        AssemblyData
        AssemblyReader
        ExecutionThread
        CLIElementTypes
        CLIMetadata
        CLIMetadataTableIndex
        CLIMetadataTableRows
        CLIMethodBody
        CLISignature
        HexStr
   )

foreach( class ${OUR_SRC} )
    list( APPEND SRC ${SRC_DIR}/CLR/${class}.cxx )
endforeach()

list( APPEND SRC ${SRC_DIR}/CLR/crossguid/guid.cxx )

add_executable( ${APP_NAME} ${SRC} )
