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
if(NOT NEX_CONFIG_VERBOSE)
	set(NEX_CONFIG_VERBOSE TRUE CACHE BOOL "Outputs cmake parse.")
endif()

## Cmake setup
#============================================================
set(CMAKE_CONFIGURATION_TYPES "Release;Debug;UnitTests" CACHE STRING "Configurations" FORCE)
set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug;UnitTests")

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
 set(NEX_BUILD "Release")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
 set(NEX_BIN ${NEX_BIN_BASE}/debug)
 set(NEX_BUILD "Debug")
# append compiler flags here
else()
 set(NEX_BIN ${NEX_BIN_BASE}/release)
 set(NEX_BUILD "Release")
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
option(NEX_USE_TBB "Use Intel Thread Building Blocks." OFF)
option(NEX_CROSS_COMPILE "Cross compile for different architecture." ON)

set(NEX_ARCHITECTURE_NAME "x86_64" CACHE STRING "Architecture chosen for cross compilation.")
set(NEX_SUPPORTED_ARCHITECTURE_LIST "x86;x86_64;ia64;ppc;alpha")
set_property(CACHE NEX_ARCHITECTURE_NAME PROPERTY STRINGS ${NEX_SUPPORTED_ARCHITECTURE_LIST})

#if (NEX_CROSS_COMPILE)
	string(TOUPPER ${NEX_ARCHITECTURE_NAME} NEX_ARCHITECTURE)
#endif (NEX_CROSS_COMPILE)

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
set(CMAKE_CXX_FLAGS_UNITTESTS "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_SHARED_LINKER_FLAGS_UNITTESTS "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
set(CMAKE_EXE_LINKER_FLAGS_UNITTESTS "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")

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

#============================================================
# get_chained_include
#============================================================
macro(get_chained_include forTarget)
	get_property(CHAINED_INCLUDE GLOBAL PROPERTY "${ARGV0}.ChainedInclude")
endmacro(get_chained_include)

#============================================================
# append_chained_include
#============================================================
macro(append_chained_include ciArg)
	get_property(CHAINED_INCLUDE GLOBAL PROPERTY "${NEXTAR_TARGET}.ChainedInclude")
	if (CHAINED_INCLUDE)
		set(CHAINED_INCLUDE "${CHAINED_INCLUDE};${ARGV0}")
	else()
		set(CHAINED_INCLUDE "${ARGV0}")
	endif()
	set_property(GLOBAL PROPERTY "${NEXTAR_TARGET}.ChainedInclude" "${CHAINED_INCLUDE}")
endmacro(append_chained_include)
#============================================================
# Target_To_Var_Name
#============================================================
macro(target_to_var target)
	string(REPLACE "." "_" TARGET_TO_VAR ${ARGV0})
endmacro(target_to_var)
#============================================================
# Add_Dependency
#============================================================
macro(add_dependency frameWorkName dependency)
	if (${NEX_CONFIG_VERBOSE})
	    message("-->> Dependency..: ${NEXTAR_TARGET} ->  ${ARGV0}.${ARGV1}")
	endif()
	set(DEP_INC_PATH ${NEX_ROOT}/${ARGV0}/${ARGV1}/include)
	if(EXISTS "${DEP_INC_PATH}/platform")
		if(NEX_OS MATCHES "Windows")
			set(DEP_INC_PATH "${DEP_INC_PATH};${DEP_INC_PATH}/platform/win32")
		elseif(NEX_OS MATCHES "Linux")
			set(DEP_INC_PATH "${DEP_INC_PATH};${DEP_INC_PATH}/platform/linux")
		elseif(NEX_OS MATCHES "MacOSX")
			set(DEP_INC_PATH "${DEP_INC_PATH};${DEP_INC_PATH}/platform/macosx")
		endif(NEX_OS MATCHES "Windows")
	endif()
	
	include_directories(${DEP_INC_PATH})

	set(DEP_NAME "NE_${ARGV0}.${ARGV1}")
	get_chained_include(${DEP_NAME})
	
	if (CHAINED_INCLUDE)
		if (${NEX_CONFIG_VERBOSE})
			message("-->> Include Chain For: ${DEP_NAME}: ${CHAINED_INCLUDE}")
		endif()
		include_directories(${CHAINED_INCLUDE})
		# This may be an explicit (chained) dependency, in which case
		# the include here needs propagation
		set(DEP_INC_PATH "${DEP_INC_PATH};${CHAINED_INCLUDE}")
	endif()
	target_link_libraries("${NEXTAR_TARGET}" "${DEP_NAME}")
endmacro(add_dependency)

#============================================================
# Add_External_Dependency
#============================================================
macro(add_external_dependency includeFiles libraryName)
	if (${NEX_CONFIG_VERBOSE})
		message("-->> Dependency..: ${NEXTAR_TARGET} -> ${ARGV1}")
	endif()
	set(TMP_VAL ${ARGV0})
	if(TMP_VAL)
		include_directories(${TMP_VAL})
	endif()
	set(TMP_VAL ${ARGV1})
	if(TMP_VAL)
		target_link_libraries("${NEXTAR_TARGET}" ${TMP_VAL})
	endif()
endmacro(add_external_dependency)

#============================================================
# Add_Chained_Dependency
#============================================================
macro(add_chained_dependency framework dependency)
	add_dependency( "${ARGV0}" "${ARGV1}")
	append_chained_include("${DEP_INC_PATH}")
endmacro(add_chained_dependency)

#============================================================
# Add_Chained_External_Dependency
#============================================================
macro(add_chained_external_dependency includeFiles libraryName)
	add_external_dependency( "${ARGV0}" "${ARGV1}" )
	append_chained_include("${ARGV0}")
endmacro(add_chained_external_dependency)

#============================================================
# Config_Src
# todo: Use source_group
#============================================================
macro(config_src)
	set(NEXTAR_SRC_LIST "")
	if (EXISTS "${NEX_ROOT}/${NEXTAR_FRAMEWORK}/Commons/src")
		list(APPEND "${NEX_ROOT}/${NEXTAR_FRAMEWORK}/Commons/src/*.cpp")
	endif()
	if (EXISTS "${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE_PATH}/Commons/src")
		list(APPEND "${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE_PATH}/Commons/src/*.cpp")
	endif()
	list(APPEND NEXTAR_SRC_LIST "${NEXTAR_TARGET_PATH}/src/*.cpp")
	if(EXISTS "${NEXTAR_TARGET_PATH}/src/platform")
		if(NEX_OS MATCHES "Windows")
			list(APPEND NEXTAR_SRC_LIST "${NEXTAR_TARGET_PATH}/src/platform/win32/*.cpp")
		elseif(NEX_OS MATCHES "Linux")
			list(APPEND NEXTAR_SRC_LIST "${NEXTAR_TARGET_PATH}/src/platform/linux/*.cpp")
		elseif(NEX_OS MATCHES "MacOSX")
			list(APPEND NEXTAR_SRC_LIST "${NEXTAR_TARGET_PATH}/src/platform/macosx/*.cpp")
		endif(NEX_OS MATCHES "Windows")
	endif()
	
	file(GLOB NEXTAR_SRC_FILES 
		${NEXTAR_SRC_LIST}
		)
endmacro(config_src)

#============================================================
# Configure_Module
#============================================================
macro(configure_target)
	include_directories(${NEX_ROOT}/${NEXTAR_FRAMEWORK}/Commons/include)
	include_directories(${NEX_ROOT}/${NEXTAR_FRAMEWORK}/Commons/local_include)
	include_directories(${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE_PATH}/Commons/include)
	include_directories(${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE_PATH}/Commons/local_include)
	include_directories(${NEXTAR_TARGET_PATH}/include)
	include_directories(${NEXTAR_TARGET_PATH}/local_include)
	if(EXISTS "${NEXTAR_TARGET_PATH}/include/platform")
		if(NEX_OS MATCHES "Windows")
			include_directories("${NEXTAR_TARGET_PATH}/include/platform/win32")
		elseif(NEX_OS MATCHES "Linux")
			include_directories("${NEXTAR_TARGET_PATH}/include/platform/linux")
		elseif(NEX_OS MATCHES "MacOSX")
			include_directories("${NEXTAR_TARGET_PATH}/include/platform/macosx")
		endif(NEX_OS MATCHES "Windows")
	endif()
	if(EXISTS "${NEXTAR_TARGET_PATH}/local_include/Config.h.cmake")
		configure_file(
			"${NEXTAR_TARGET_PATH}/local_include/Config.h.cmake"
			"${NEXTAR_TARGET_PATH}/include/${NEXTAR_CONFIG_NAME}Config.h"
		)
	endif()
endmacro(configure_target)

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
	if(EXISTS "${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE}")
		set(NEXTAR_MODULE_PATH ${NEXTAR_MODULE})
		add_subdirectory(${NEXTAR_MODULE})
	elseif(EXISTS "${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE}.tst")
		set(NEXTAR_MODULE_PATH ${NEXTAR_MODULE}.tst)
		add_subdirectory(${NEXTAR_MODULE_PATH})
	endif()
endmacro(add_module)

#============================================================
# Add_Unit_Test
#============================================================
macro(add_unit_test test)
	set(NEXTAR_UT ${ARGV0})
	add_subdirectory(${ARGV0})
endmacro(add_unit_test)

#============================================================
# Create_Module
#============================================================
macro(create_library)
	set(NEXTAR_TARGET NE_${NEXTAR_FRAMEWORK}.${NEXTAR_MODULE})	
	set(NEXTAR_TARGET_PATH ${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE})
	set(NEXTAR_CONFIG_NAME ${NEXTAR_MODULE})
	target_to_var(${NEXTAR_TARGET})
	set(NEXTAR_LIBRARY_DEFINE "${TARGET_TO_VAR}")
	config_src()
	configure_target()
	message("-->> Create Module..: ${NEXTAR_TARGET}")
	add_library(${NEXTAR_TARGET} SHARED ${NEXTAR_SRC_FILES})
endmacro(create_library)

#============================================================
# Create_Exec
#============================================================
macro(create_exec)
	set(NEXTAR_TARGET NE_${NEXTAR_FRAMEWORK}.${NEXTAR_MODULE})	
	set(NEXTAR_TARGET_PATH ${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE})
	set(NEXTAR_CONFIG_NAME ${NEXTAR_MODULE})
	config_src()
	configure_target()
	message("-->> Create Exec..: ${NEXTAR_TARGET}")	
	add_executable(${NEXTAR_TARGET} ${NEXTAR_SRC_FILES})
endmacro(create_exec)

#============================================================
# Create_UT
#============================================================
macro(create_ut consoleApp)
	set(NEXTAR_TARGET "NE_${NEXTAR_FRAMEWORK}.${NEXTAR_MODULE}_${NEXTAR_UT}.tst")
	set(NEXTAR_TARGET_PATH ${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE_PATH}/${NEXTAR_UT})
	set(NEXTAR_CONFIG_NAME ${NEXTAR_UT})
	if(${ARGV0})
		add_definitions( -DNEX_CONSOLE_APP_ENTRY )
	endif()
	config_src()
	configure_target()
	message("-->> Create Test..: ${NEXTAR_TARGET}")
	add_executable(${NEXTAR_TARGET} ${NEXTAR_SRC_FILES})
endmacro(create_ut)

## Macros
#============================================================
# Create_Plugin
#============================================================
macro(create_plugin pluginName pluginPath type build optional)
	file(APPEND ${NEX_PLUGIN_CONFIG_FILE} 
		"${NEX_PLUGIN_CONFIG}"
		 " <plugin>\n"
		 "  <name>${ARGV0}</name>\n"
		 "  <path>${ARGV1}</path>\n"
		 "  <type>${ARGV2}</type>\n"
		 "  <build>${ARGV3}</build>\n"
		 "  <optional>${ARGV4}</optional>\n"
		 " </plugin>\n")
	create_library()
endmacro(create_plugin)


## Build targets
#============================================================
add_framework(Core)
add_framework(Plugins)
add_framework(UnitTests)
add_framework(Data)

