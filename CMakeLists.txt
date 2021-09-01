project(UniversalSpeech 
        DESCRIPTION "Universal Speech library"
        LANGUAGES C)
cmake_minimum_required(VERSION 3.10)

include(CPack)
include(GenerateExportHeader)

option(BUILD_EXAMPLES "Builds example programs" ON)

# Select the release build type by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# OS info
message("-------------------------------------------------------")
message("-- CMAKE_SYSTEM_INFO_FILE: ${CMAKE_SYSTEM_INFO_FILE}")
message("-- CMAKE_SYSTEM_NAME:      ${CMAKE_SYSTEM_NAME}")
message("-- CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
message("-- CMAKE_SYSTEM:           ${CMAKE_SYSTEM}")
message("-- CMAKE_C_COMPILER:       ${CMAKE_C_COMPILER}")
string (REGEX MATCH "\\.el[1-9]" os_version_suffix ${CMAKE_SYSTEM})
message("-- os_version_suffix:      ${os_version_suffix}")
message("-- Project: ${CMAKE_CURRENT_SOURCE_DIR}")
message("-- Binaries: ${CMAKE_CURRENT_BINARY_DIR}")
message("-------------------------------------------------------")
message("-- CMAKE_BUILD_TYPE:       ${CMAKE_BUILD_TYPE}")
message("-- Build examples:         ${BUILD_TESTS}")
message("-------------------------------------------------------")

# Version info
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(VERSION_DATE 20210827)

# set architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_compile_definitions(_ARCH64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    add_compile_definitions(_ARCH32)
endif()

# setup compiler definitions
if(WIN32)
  add_compile_definitions(_WIN32
                          WIN32_LEAN_AND_MEAN)
endif(WIN32)

if(MSVC)
  add_compile_definitions(_MSVC
                          _CRT_SECURE_NO_WARNINGS)
endif(MSVC)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif(BUILD_EXAMPLES)

# Collect our source files
set(SOURCE_FILES
    src/UniversalSpeech.c
    )

set(HEADER_FILES
    include/UniversalSpeech.h
    src/private.h
    )

if(WIN32)
    list(APPEND SOURCE_FILES
        src/windows/cobra.c
        src/windows/com-interface.c
        src/windows/disphelper.c
        src/windows/dolphin.c
        src/windows/encoding-conversion.c
        src/windows/engines.c
        src/windows/hook.c
        src/windows/jaws.c
        src/windows/misc.c
        src/windows/narrator.c
        src/windows/nvda.c
        src/windows/processlist.c
        src/windows/sapi.c
        src/windows/systemaccess.c
        src/windows/windows-eyes.c
        src/windows/zoomtext.c
        src/windows/zoomtext-guid.c
        )
    
    list(APPEND HEADER_FILES
        CMakeLists.txt

        src/windows/disphelper.h
        src/windows/encoding-conversion.h
        src/windows/zoomtext.h
        )
endif(WIN32)

list(APPEND SOURCE_FILES ${HEADER_FILES})
set_source_files_properties(${HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(UniversalSpeech SHARED ${SOURCE_FILES})

set_target_properties(UniversalSpeech PROPERTIES 
    PUBLIC_HEADER include/UniversalSpeech.h
    C_STANDARD 11
	)

if(WIN32)
    target_link_libraries(UniversalSpeech psapi version)
endif(WIN32)

generate_export_header(UniversalSpeech
                       BASE_NAME UniversalSpeech
                       EXPORT_MACRO_NAME UniversalSpeech_EXPORT
                       EXPORT_FILE_NAME UniversalSpeech_Export.h
                       STATIC_DEFINE UniversalSpeech_BUILT_AS_STATIC
                       )

add_custom_command(TARGET UniversalSpeech POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/thirdparty/nvdaController/nvdacontrollerClient64.dll
    ${PROJECT_SOURCE_DIR}/thirdparty/nvdaController/nvdacontrollerClient32.dll
    ${PROJECT_SOURCE_DIR}/thirdparty/dolapi.dll
    ${PROJECT_SOURCE_DIR}/thirdparty/jfwapi.dll
    ${PROJECT_SOURCE_DIR}/thirdparty/SAAPI32.dll
    $<TARGET_FILE_DIR:UniversalSpeech>)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
