project(UniversalSpeech 
        DESCRIPTION "Universal Speech library"
        LANGUAGES C)
cmake_minimum_required(VERSION 3.10)

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

# Version info
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(VERSION_DATE 20210827)

# set architecture
if(NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio")
    if("${ARCH_TYPE}" STREQUAL "32")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    elseif("${ARCH_TYPE}" STREQUAL "64")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
    endif()
endif()

# Select the release build type by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    message("-- CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif(NOT CMAKE_BUILD_TYPE)

# setup compiler definitions
if(WIN32)
  add_compile_definitions(_WIN32
                          WIN32_LEAN_AND_MEAN)
endif(WIN32)

if(MSVC)
  add_compile_definitions(_MSVC
                          _CRT_SECURE_NO_WARNINGS)
endif(MSVC)

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

add_library(UniversalSpeech ${SOURCE_FILES})

set_target_properties(UniversalSpeech PROPERTIES 
    PUBLIC_HEADER include/UniversalSpeech.h
    C_STANDARD 11
	)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
