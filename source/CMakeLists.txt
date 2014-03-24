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
project(NextarEngine)


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

## Plugin dependecy on Base
#============================================================
set(NEX_BASE_ROOT ${PROJECT_SOURCE_DIR}/Core/Base)
set(NEX_BASE_INCLUDE ${NEX_BASE_ROOT}/include)

## Common link directories
#============================================================
link_directories(
 "${NEX_LIB}"
)

## Common Pre definitions
#============================================================
add_definitions( -DNEXTAR_EXPORT_AS_DYNAMIC_LIB )

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

## Create nextar configuration file


## Macros
#============================================================
# Add_Plugin
#============================================================
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

#============================================================
# Add_Framework
#============================================================
macro(add_framework frameWorkName)
    set(NEXTAR_FRAMEWORK ${ARGV0})
    add_subdirectory(${ARGV0})
endmacro(add_framework)

#============================================================
# Add_Module
#============================================================
macro(add_module moduleName)
    set(NEXTAR_MODULE ${ARGV0})
    add_subdirectory(${ARGV0})
endmacro(add_module)

#============================================================
# Configure_Module
#============================================================
macro(configure_module moduleName frameWorkName)
	if(EXISTS "${NEX_ROOT}/${ARGV1}/${ARGV0}/local_include/Config.h.cmake")
		configure_file(
			"${NEX_ROOT}/${ARGV1}/${ARGV0}/local_include/Config.h.cmake"
			"${NEX_ROOT}/${ARGV1}/${ARGV0}/include/${ARGV0}Config.h"
		)
	endif()
endmacro(configure_module)

#============================================================
# Create_Module
#============================================================
macro(create_module moduleName frameWorkName sourceBlob)
    message("Create Module..: ${ARGV1}.${ARGV0}")
    include_directories(${NEX_ROOT}/${ARGV1}/${ARGV0}/include)
    include_directories(${NEX_ROOT}/${ARGV1}/${ARGV0}/local_include)
    set(NEXTAR_MODULE_NAME "NE_${ARGV1}.${ARGV0}")
    set(NEXTAR_MODULE_DEFINE "NE_${ARGV1}_${ARGV0}")
    add_library(${NEXTAR_MODULE_NAME} SHARED ${ARGV2})
    configure_module(${ARGV0} ${ARGV1})
endmacro(create_module)

#============================================================
# Create_Module_Default
#============================================================
macro(create_module_default)
	set(NEX_SRC_PATH ${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE}/src)
	if(EXISTS "${NEX_SRC_PATH}/platform")
		if(NEX_OS MATCHES "Windows")
			set(NEX_PLATFORM_SRC ${NEXCORE_BASE_SRC}/platform/win32)
		elseif(NEX_OS MATCHES "Linux")
			set(NEX_PLATFORM_SRC ${NEXCORE_BASE_SRC}/platform/linux)
		elseif(NEX_OS MATCHES "MacOSX")
			set(NEX_PLATFORM_SRC ${NEXCORE_BASE_SRC}/platform/macosx)
		endif(NEX_OS MATCHES "Windows")

		file(GLOB NEX_SRC_LIST 
			${NEX_SRC_PATH}/*.cpp
			${NEX_PLATFORM_SRC}/*.cpp
			)
	else()

		file(GLOB NEX_SRC_LIST 
			${NEX_SRC_PATH}/*.cpp
			)
	endif()
	create_module(${NEXTAR_MODULE} ${NEXTAR_FRAMEWORK} ${NEX_SRC_LIST})
endmacro(create_module_default)

#============================================================
# Add_Dependency
#============================================================
macro(add_dependency moduleFrameWork moduleName frameWorkName dependency)
    message("Dependency..: ${ARGV0}.${ARGV1} ->  ${ARGV2}.${ARGV3}")
    include_directories(${NEX_ROOT}/${ARGV2}/${ARGV3}/include)
    target_link_libraries("NE_${ARGV0}.${ARGV1}" "NE_${ARGV2}.${ARGV3}")
endmacro(add_dependency)

#============================================================
# Add_External_Dependency
#============================================================
macro(add_external_dependency moduleFrameWork moduleName includeFiles libraryName)
    message("Dependency..: ${ARGV0}.${ARGV1} -> ${ARGV3}")
    if(NOT ${ARGV2} MATCHES "")
	    include_directories(${ARGV2})
    endif()
    if(NOT ${ARGV3} MATCHES "")
	    target_link_libraries("NE_${ARGV0}.${ARGV1}" ${ARGV3})
    endif()
endmacro(add_external_dependency)

#============================================================
# Add_Dependency_Default
#============================================================
macro(add_dependency_default frameWorkName dependency)
	add_dependency(${NEXTAR_FRAMEWORK} ${NEXTAR_MODULE} ${ARGV0} ${ARGV1})
endmacro(add_dependency_default)

#============================================================
# Add_External_Dependency_Default
#============================================================
macro(add_external_dependency_default includeFiles libraryName)
	add_external_dependency(${NEXTAR_FRAMEWORK} ${NEXTAR_MODULE} "${ARGV0}" "${ARGV1}")
endmacro(add_external_dependency_default)

## Build targets
#============================================================
add_framework(Core)
add_framework(Plugins)
#add_framework(UnitTests)
#add_framework(Data)

## End plugin configuration
file(APPEND ${NEX_PLUGIN_CONFIG_FILE} "</plugins>\n")

