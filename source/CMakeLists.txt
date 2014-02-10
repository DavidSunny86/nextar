##---------------------------------------------------
# Nextar build file.
# : Root file common to all configuration.
# Abhishek Dey
# 5:56 PM 9/25/2011
##---------------------------------------------------

# CMake version
cmake_minimum_required(VERSION 2.8)


## Project
#============================================================
project(nextar)


## Root variables
#============================================================
set(NEX_ROOT ${PROJECT_SOURCE_DIR})
set(NEX_BIN_BASE  ${NEX_ROOT}/../bin)
set(NEX_BUILD ${NEX_ROOT}/../build)
set(NEX_LIB ${NEX_ROOT}/../lib)
set(NEX_DEPENDENCIES ${NEX_ROOT}/../dependencies)
set(NEX_DATA ${NEX_ROOT}/Data)
set(NEX_PLUGINS ${NEX_ROOT}/Plugins)
set(NEX_UNIT_TESTS ${PROJECT_SOURCE_DIR}/UnitTests)

## Module path
#============================================================
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${NEX_ROOT}/CMake/Modules/")

## Basic Libraries
#============================================================
set(LIBNEXTECH NexTech)

## Compiler flags
#============================================================
set(GCC_COMPILE_FLAGS "-std=c++11 -pthread")

## Configure build config paths
#============================================================
if(NOT CMAKE_BUILD_TYPE)
 set(NEX_BIN ${NEX_BIN_BASE})
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
 set(NEX_BIN ${NEX_BIN_BASE}/debug)
# append compiler flags here
else()
 set(NEX_BIN ${NEX_BIN_BASE}/release)
endif()

## SSE Support check
#============================================================
message("@TODO Write a script to detect SSE support" )
set (SSE3_FOUND 1)
set (SSSE3_FOUND 1)
set (SSE4_1_FOUND 1)
set (SSE4_2_FOUND 1)

if (SSE3_FOUND)
  set (GCC_COMPILE_FLAGS "${GCC_COMPILE_FLAGS} -msse3")
endif()
if (SSSE3_FOUND)
  set (GCC_COMPILE_FLAGS "${GCC_COMPILE_FLAGS} -mssse3")
endif()
if (SSE4_1_FOUND)
  set (GCC_COMPILE_FLAGS "${GCC_COMPILE_FLAGS} -msse4.1")
endif()
if (SSE4_2_FOUND)
  set (GCC_COMPILE_FLAGS "${GCC_COMPILE_FLAGS} -msse4.2")
endif()

## Configure paths
#============================================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${NEX_LIB})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${NEX_BIN})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${NEX_BIN})


## Configurable Options
#============================================================
option(NEX_DEBUG "Turn on debuginfo." ON)
option(NEX_STORE_IN_LITTLEENDIN "Serialize data in little endian." ON)
option(NEX_USE_PTMALLOC "Use PtMalloc instead of NedAlloc." OFF)
option(NEX_SUPPORT_MULTITHREADING "Use multithreading." ON)

set(NEX_ENDIAN LITTLE)
set(NEX_VER_MAJOR 1)
set(NEX_VER_MINOR 0)
set(NEX_VER_REVISION 3)

set(NEX_VERSION "${NEX_VER_MAJOR}.${NEX_VER_MINOR}.${NEX_VER_REVISION}")

## Plugin dependecy on nextech
#============================================================
set(NEXTECH_ROOT ${PROJECT_SOURCE_DIR}/Core/LibNexTech)
set(NEXTECH_INCLUDE ${NEXTECH_ROOT}/include)

## Common include directories
#============================================================
include_directories(
  "${NEXTECH_INCLUDE}"
)

## Common link directories
#============================================================
link_directories(
 "${NEX_LIB}"
)

## Common Pre definitions
#============================================================
add_definitions( -DNEXTAR_EXPORT_AS_DYNAMIC_LIB )

## Configuration for libnextech
#============================================================
configure_file(
   "${NEXTECH_INCLUDE}/config/Config.h.cmake"
   "${NEXTECH_INCLUDE}/config/Config.h"
)

## Setup compiler flags based on compiler
#============================================================
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
# using Clang
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
# using GCC
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}" )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
# using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
# using Visual Studio C++
endif()

## Platform detection
#============================================================
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(NEX_OS "Windows")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(NEX_OS "Linux")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(NEX_OS "MacOSX")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

## Create plugin configuration file
#============================================================
set(NEX_PLUGIN_CONFIG_FILE ${NEX_BIN}/Plugins.xml)

macro(add_plugin pluginName pluginPath type build optional)
file(APPEND ${NEX_PLUGIN_CONFIG_FILE} 
"${NEX_PLUGIN_CONFIG}"
 " <plugin>\n"
 "  <name>${ARGV0}</name>\n"
 "  <path>${ARGV1}</path>\n"
 "  <type>${ARGV2}</type>\n"
 "  <build>${ARGV3}</build>\n"
 "  <optional>${ARGV4}</optional>\n"
 " </plugin>\n")
 endmacro(add_plugin)

## Create nextar configuration file


## Build targets
#============================================================
add_subdirectory(Core)
add_subdirectory(Plugins)
add_subdirectory(UnitTests)
add_subdirectory(Data)
## End plugin configuration
file(APPEND ${NEX_PLUGIN_CONFIG_FILE} "</plugins>\n")

