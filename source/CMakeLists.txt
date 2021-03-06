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
set(NEX_ROOT ${CMAKE_CURRENT_LIST_DIR})
message("Current directory: ${NEX_ROOT}")
set(NEX_BUILD_DIR ${NEX_ROOT}/../build)
set(NEX_INSTALL ${NEX_ROOT}/../install)
set(NEX_LIB ${NEX_ROOT}/../lib)
set(NEX_DEPENDENCIES ${NEX_ROOT}/../dependencies)
set(NEX_DATA ${NEX_ROOT}/Data)
set(NEX_PLUGINS ${NEX_ROOT}/Plugins)
set(NEX_UNIT_TESTS ${NEX_ROOT}/UnitTests)
set(NEX_BASE_ROOT ${NEX_ROOT}/Core/Base)

## Module path
if(NOT NEX_CONFIG_VERBOSE)
	set(NEX_CONFIG_VERBOSE TRUE CACHE BOOL "Outputs cmake parse.")
endif()

message("System: ${CMAKE_SYSTEM_NAME}")
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
 #set(NEX_BIN ${NEX_BIN_BASE}/release)
 set(NEX_BUILD "Release")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
 #set(NEX_BIN ${NEX_BIN_BASE}/debug)
 set(NEX_BUILD "Debug")
# append compiler flags here
else()
 #set(NEX_BIN ${NEX_BIN_BASE}/release)
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

## Configurable Options
#============================================================
option(NEX_DEBUG "Turn on debuginfo." ON)
option(NEX_STORE_IN_LITTLEENDIN "Serialize data in little endian." ON)
option(NEX_USE_PTMALLOC "Use PtMalloc instead of NedAlloc." OFF)
option(NEX_SUPPORT_MULTITHREADING "Use multithreading." ON)
option(NEX_USE_TBB "Use Intel Thread Building Blocks." OFF)
option(NEX_TASK_SCHEDULAR_TRACES "Allow schedular traces." OFF)
option(NEX_CROSS_COMPILE "Cross compile for different architecture." ON)
option(NEX_UNITY_BUILD "Unity build." ON)
option(NEX_USE_VECTOR_INSTRUCTIONS "Use vector instructions for math library." ON)
option(NEX_PREFER_SPEED_OVER_ACCURACY "Prefer speed over accuracy." ON)
set(NEX_UNITY_FILE_LIMIT 40 CACHE STRING "Number of files per unit in unity build")
set(NEX_ARCHITECTURE_NAME "x64" CACHE STRING "Architecture chosen for cross compilation.")
set(NEX_SUPPORTED_ARCHITECTURE_LIST "x86;x64;ia64;ppc;alpha")
set_property(CACHE NEX_ARCHITECTURE_NAME PROPERTY STRINGS ${NEX_SUPPORTED_ARCHITECTURE_LIST})
set(NEX_VECTOR_MATH_TYPE_NAME "Generic" CACHE STRING "Vector instructions to compile with.")
set(NEX_VECTOR_MATH_TYPE_NAME_LIST "Generic;sse2;sse3;sse4;sse4.1;sse4.2;3dnow;vmx128")
set_property(CACHE NEX_VECTOR_MATH_TYPE_NAME PROPERTY STRINGS ${NEX_VECTOR_MATH_TYPE_NAME_LIST})

if(${NEX_UNITY_BUILD})
	message("Unity build is on.")
endif()

#if (NEX_CROSS_COMPILE)
	string(TOUPPER ${NEX_ARCHITECTURE_NAME} NEX_ARCHITECTURE)
#endif (NEX_CROSS_COMPILE)

#if (NEX_CROSS_COMPILE)
	string(TOUPPER ${NEX_VECTOR_MATH_TYPE_NAME} NEX_VECTOR_MATH_TYPE_NAME)
	string(REPLACE "." "_" NEX_VECTOR_MATH_TYPE_NAME ${NEX_VECTOR_MATH_TYPE_NAME})
#endif (NEX_CROSS_COMPILE)

set(NEX_ENDIAN LITTLE)
set(NEX_VER_MAJOR 1)
set(NEX_VER_MINOR 0)
set(NEX_VER_REVISION 3)

set(NEX_VERSION "${NEX_VER_MAJOR}.${NEX_VER_MINOR}.${NEX_VER_REVISION}")

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
cmake_policy(SET CMP0054 NEW)
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
	set(NEX_PLATFORM_NAME "win")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(NEX_OS "Linux")
	set(NEX_PLATFORM_NAME "linux")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(NEX_OS "MacOSX")
	set(NEX_PLATFORM_NAME "osx")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

## Platform identifier
#============================================================
set(NEX_PLATFORM_ID "${NEX_PLATFORM_NAME}_${NEX_ARCHITECTURE_NAME}")
if(NEX_OS MATCHES "Windows")
	set(CMAKE_LIBRARY_ARCHITECTURE "${NEX_ARCHITECTURE_NAME}")
endif()

## Platform specific stuff
#============================================================
if(NEX_OS MATCHES "Windows")
  set(CMAKE_PREFIX_PATH ${NEX_DEPENDENCIES})
endif()

## Create plugin configuration file
#============================================================
set(NEX_PLUGIN_CONFIG_FILE ${NEX_INSTALL}/Bin/Plugins.xml)
set(NEX_INSTALL_PLUGIN OFF)
## Create nextar configuration file
#============================================================
# copy_binary_to_output
#============================================================
macro(copy_binary_to_out binFolder)
	add_custom_command(TARGET "${NEXTAR_TARGET}" POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${ARGV0}"
			${NEX_INSTALL}/Bin)
endmacro(copy_binary_to_out)
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
# Generated file list
#============================================================
macro(add_gen_file_src)
	foreach(FileInList ${ARGN})
		list(APPEND NEX_GEN_SRC_FILES ${FileInList})
	endforeach()
endmacro(add_gen_file_src)
#============================================================
# Create dummy generated files
#============================================================
macro(create_gen_files)
	if (NEX_GEN_SRC_FILES)
		file(MAKE_DIRECTORY ${NEXTAR_TARGET_PATH}/gen)
	endif()
	foreach(FileInList ${NEX_GEN_SRC_FILES})
		# create a dummy file as we will have prob with target otherwise
		# @todo need a cleanup macro in case files are not overwirtten
		if (NOT EXISTS ${FileInList})
			file(WRITE ${FileInList} "")
			set(NEX_CLEAN_UP_FILE_LIST "${NEX_CLEAN_UP_FILE_LIST};${FileInList}" CACHE INTERNAL "")
		endif()
	endforeach()
endmacro(create_gen_files)
#============================================================
# Pre build step
#============================================================
macro(add_pre_build_command)
	if (NOT NEX_PRE_BUILD_STEP)
		set(NEX_PRE_BUILD_STEP TRUE)
		set(NEX_PRE_BUILD_TARGET ${NEXTAR_BINARY_NAME}_PRE_BUILD)
		set(NEX_PRE_BUILD_DEPENDENCIES "")
	endif()
	set(NE_PCOMMAND ${ARGV1})
	separate_arguments(NE_PCOMMAND)
	add_custom_command(
		OUTPUT ${ARGV2}
		COMMAND ${NE_PCOMMAND}
		DEPENDS ${ARGV0}
	)
	list(APPEND NEX_PRE_BUILD_DEPENDENCIES ${ARGV2})
endmacro(add_pre_build_command)
#============================================================
# Pre build execute
#============================================================
macro(add_pre_build_execute)
	if (NOT NEX_PRE_BUILD_STEP)
		set(NEX_PRE_BUILD_STEP TRUE)
		set(NEX_PRE_BUILD_TARGET ${NEXTAR_BINARY_NAME}_PRE_BUILD)
		add_custom_target(${NEX_PRE_BUILD_TARGET} COMMAND echo Building - ${NEX_PRE_BUILD_TARGET})
	endif()
	foreach(arg ${ARGN})
		MATH(EXPR NEX_PRE_BUILD_STEP_COUNTER "${NEX_PRE_BUILD_STEP_COUNTER}+1")
		set(NE_PRE_BUILD_SUB ${NEX_PRE_BUILD_TARGET}_${NEX_PRE_BUILD_STEP_COUNTER})
		set(NE_PCOMMAND ${arg})
		separate_arguments(NE_PCOMMAND)
		add_custom_target(${NE_PRE_BUILD_SUB} COMMAND ${NE_PCOMMAND})
		add_dependencies(${NEX_PRE_BUILD_TARGET} ${NE_PRE_BUILD_SUB})
	endforeach()
endmacro(add_pre_build_execute)
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
# include_path
#============================================================
macro(include_path path)
	if(EXISTS ${ARGV0})
		include_directories(${ARGV0})
		list(APPEND NEXTAR_HEADER_LIST "${ARGV0}/*.h")
	endif()
endmacro(include_path)

#============================================================
# pre_configure
#============================================================
macro(pre_configure)
	set(NEXTAR_HEADER_LIST "")
	include_path(${NEX_ROOT}/${NEXTAR_FRAMEWORK}/Commons/include)
	include_path(${NEX_ROOT}/${NEXTAR_FRAMEWORK}/Commons/local_include)
	include_path(${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE_PATH}/Commons/include)
	include_path(${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE_PATH}/Commons/local_include)
	include_path(${NEXTAR_TARGET_PATH}/include)
	include_path(${NEXTAR_TARGET_PATH}/local_include)
	include_path(${NEXTAR_TARGET_PATH}/gen)
	if(EXISTS "${NEXTAR_TARGET_PATH}/include/platform")
		if(NEX_OS MATCHES "Windows")
			include_path("${NEXTAR_TARGET_PATH}/include/platform/win32")
		elseif(NEX_OS MATCHES "Linux")
			include_path("${NEXTAR_TARGET_PATH}/include/platform/linux")
		elseif(NEX_OS MATCHES "MacOSX")
			include_path("${NEXTAR_TARGET_PATH}/include/platform/macosx")
		endif(NEX_OS MATCHES "Windows")
	endif()
	if(EXISTS "${NEXTAR_TARGET_PATH}/local_include/platform")
		if(NEX_OS MATCHES "Windows")
			include_path("${NEXTAR_TARGET_PATH}/local_include/platform/win32")
		elseif(NEX_OS MATCHES "Linux")
			include_path("${NEXTAR_TARGET_PATH}/local_include/platform/linux")
		elseif(NEX_OS MATCHES "MacOSX")
			include_path("${NEXTAR_TARGET_PATH}/local_include/platform/macosx")
		endif(NEX_OS MATCHES "Windows")
	endif()
	if(EXISTS "${NEXTAR_TARGET_PATH}/local_include/Config.h.cmake")
		configure_file(
			"${NEXTAR_TARGET_PATH}/local_include/Config.h.cmake"
			"${NEXTAR_TARGET_PATH}/include/${NEXTAR_CONFIG_NAME}Config.h"
		)
	endif()
	file(GLOB HEADER_GROUP_FILES
		${NEXTAR_HEADER_LIST}
		)
	# file(REMOVE_RECURSE ${NEXTAR_TARGET_PATH}/gen) - no remove
	create_gen_files()
endmacro(pre_configure)
#============================================================
# clean_unity_file
#============================================================
macro(clean_unity_file index)
	if (EXISTS "${NEX_UNITY_SRC_PATH}/c${ARGV0}.cpp")
		file(WRITE "${NEX_UNITY_SRC_PATH}/c${ARGV0}.cpp" "")
	endif()
endmacro(clean_unity_file)
#============================================================
# unity_file
#============================================================
macro(unity_file index path)
	file(APPEND "${NEX_UNITY_SRC_PATH}/c${ARGV0}.cpp" "#include \"${ARGV1}\"\n")
	list(APPEND NEXTAR_SRC_FILES_UNITY "${NEX_UNITY_SRC_PATH}/c${ARGV0}.cpp")
endmacro(unity_file)
#============================================================
# config_unity
#============================================================
macro(post_configure)
	if(${NEX_UNITY_BUILD})
		set(NEX_UNITY_SRC_PATH ${NEX_BUILD_DIR}/${NEXTAR_TARGET_RELPATH})
		set(NEXTAR_SRC_FILES_UNITY "")
		set(i 0)
		set(fc 0)
		# create the unity file
		clean_unity_file(${i})
		foreach(FileName ${SOURCE_GROUP_FILES})
			unity_file(${i} ${FileName})
			math(EXPR fc "${fc}+1")
			if ( fc GREATER ${NEX_UNITY_FILE_LIMIT} )
				set(fc 0)
				math(EXPR i "${i}+1")
				clean_unity_file(${i})
			endif()
		endforeach()
		set(NEXTAR_SRC_FILES ${NEXTAR_SRC_FILES_UNITY} ${SOURCE_GROUP_FILES} ${HEADER_GROUP_FILES})
	else()
		set(NEXTAR_SRC_FILES ${SOURCE_GROUP_FILES} ${HEADER_GROUP_FILES})
	endif()
endmacro(post_configure)
#============================================================
# config_src
#============================================================
macro(config_src)
	pre_configure()
	set(NEXTAR_SRC_LIST "")
	if (EXISTS "${NEX_ROOT}/${NEXTAR_FRAMEWORK}/Commons/src")
		list(APPEND NEXTAR_SRC_LIST "${NEX_ROOT}/${NEXTAR_FRAMEWORK}/Commons/src/*.cpp")
	endif()
	if (EXISTS "${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE_PATH}/Commons/src")
		list(APPEND NEXTAR_SRC_LIST "${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE_PATH}/Commons/src/*.cpp")
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

	file(GLOB SOURCE_GROUP_FILES
		${NEXTAR_SRC_LIST}
		)
	list(APPEND SOURCE_GROUP_FILES ${NEX_GEN_SRC_FILES})
	post_configure()
endmacro(config_src)

#============================================================
# Add_Framework
#============================================================
macro(add_framework frameWorkName)
	set(NEXTAR_FRAMEWORK ${ARGV0})
	set(NEXTAR_PACKAGE_LIST "${ARGN}")
	add_subdirectory("${NEX_ROOT}/${ARGV0}")
endmacro(add_framework)

#============================================================
# For_Platform
#============================================================
macro(for_platform platformNames)
	if( (NOT ("${ARGV0}" MATCHES "All") ) AND
		(NOT (";${ARGV0};" MATCHES ";${NEX_OS};") ) )
		return()
	endif()
endmacro(for_platform)

#============================================================
# clear_variables
#============================================================
macro(clear_variables)
	set(NEX_PRE_BUILD_STEP FALSE)
	set(NEX_PRE_BUILD_STEP_COUNTER 0)
	set(NEX_GEN_SRC_FILES "")
endmacro(clear_variables)
#============================================================
# Add_Module
#============================================================
macro(add_module moduleName)
	clear_variables()
	set(NEXTAR_MODULE ${ARGV0})
	set(NEXTAR_BINARY_NAME ${NEXTAR_MODULE})
	list(FIND NEXTAR_PACKAGE_LIST ${NEXTAR_MODULE} NEX_MODULE_ADDED)
	if ( (";${NEXTAR_PACKAGE_LIST};" MATCHES ";${NEXTAR_MODULE};") OR
		 (";${NEXTAR_PACKAGE_LIST};" MATCHES ";All;") OR
		 NOT(NEXTAR_PACKAGE_LIST) )
		if(EXISTS "${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE}")
			set(NEXTAR_MODULE_PATH ${NEXTAR_MODULE})
			add_subdirectory(${NEXTAR_MODULE})
		elseif(EXISTS "${NEX_ROOT}/${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE}.tst")
			set(NEXTAR_MODULE_PATH ${NEXTAR_MODULE}.tst)
			add_subdirectory(${NEXTAR_MODULE_PATH})
		endif()
	endif()
endmacro(add_module)

#============================================================
# Add_Unit_Test
#============================================================
macro(add_unit_test test)
	clear_variables()
	set(NEXTAR_UT ${ARGV0})
	set(NEXTAR_BINARY_NAME ${NEXTAR_MODULE}_${NEXTAR_UT})
	add_subdirectory(${ARGV0})
endmacro(add_unit_test)
#============================================================
# Place_bin
#============================================================
macro(place_bin library)
	if (${ARGV0})
		add_library(${NEXTAR_TARGET} SHARED ${NEXTAR_SRC_FILES})
	else (${ARGV0})
		add_executable(${NEXTAR_TARGET} ${NEXTAR_SRC_FILES})
	endif()

	if (NEX_INSTALL_PLUGIN)
		set(NEX_INSTALL_DIR ${NEX_INSTALL}/Bin/plugins)
	else (NEX_INSTALL_PLUGIN)
		set(NEX_INSTALL_DIR ${NEX_INSTALL}/Bin)
	endif()
	if (NEX_PRE_BUILD_STEP)
		separate_arguments(NEX_PRE_BUILD_DEPENDENCIES)
		message("C:  " ${NEX_PRE_BUILD_DEPENDENCIES})
		add_custom_target(${NEX_PRE_BUILD_TARGET} DEPENDS ${NEX_PRE_BUILD_DEPENDENCIES})
		add_dependencies(${NEXTAR_TARGET} ${NEX_PRE_BUILD_TARGET})
	endif()
	#add_custom_command(TARGET ${NEXTAR_TARGET} POST_BUILD
	#	COMMAND ${CMAKE_COMMAND} -E copy_directory
	#		$<TARGET_FILE_DIR:${NEXTAR_TARGET}>
	#		"${NEX_INSTALL_DIR}")
	set_target_properties(${NEXTAR_TARGET} PROPERTIES
					  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${NEX_INSTALL_DIR}
					  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${NEX_INSTALL_DIR}
			  LIBRARY_OUTPUT_DIRECTORY_DEBUG ${NEX_INSTALL_DIR}
					  LIBRARY_OUTPUT_DIRECTORY_RELEASE ${NEX_INSTALL_DIR})

	source_group("[Sources]" FILES ${SOURCE_GROUP_FILES})
	source_group("[Headers]" FILES ${HEADER_GROUP_FILES})
	if (${NEX_UNITY_BUILD})
		set_source_files_properties(${SOURCE_GROUP_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
	endif()
	set_source_files_properties(${HEADER_GROUP_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
endmacro(place_bin)

#============================================================
# Create_Module
#============================================================
macro(create_library)
	set(NEXTAR_TARGET NE_${NEXTAR_FRAMEWORK}.${NEXTAR_BINARY_NAME})
	set(NEXTAR_TARGET_RELPATH ${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE})
	set(NEXTAR_TARGET_PATH ${NEX_ROOT}/${NEXTAR_TARGET_RELPATH})
	set(NEXTAR_CONFIG_NAME ${NEXTAR_MODULE})
	target_to_var(${NEXTAR_TARGET})
	set(NEXTAR_LIBRARY_DEFINE "${TARGET_TO_VAR}")
	config_src()
	message("--------------------------------------")
	message("-->> Create Module..: ${NEXTAR_TARGET}")
	place_bin(TRUE)
endmacro(create_library)

#============================================================
# Create_Exec
#============================================================
macro(create_exec)
	set(NEXTAR_TARGET NE_${NEXTAR_FRAMEWORK}.${NEXTAR_BINARY_NAME})
	set(NEXTAR_TARGET_RELPATH ${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE})
	set(NEXTAR_TARGET_PATH ${NEX_ROOT}/${NEXTAR_TARGET_RELPATH})
	set(NEXTAR_CONFIG_NAME ${NEXTAR_MODULE})
	config_src()
	message("--------------------------------------")
	message("-->> Create Exec..: ${NEXTAR_TARGET}")
	set(NEX_INSTALL_DIR ${NEX_INSTALL})
	place_bin(FALSE)
endmacro(create_exec)

#============================================================
# Create_UT
#============================================================
macro(create_ut consoleApp)
	set(NEXTAR_TARGET "NE_${NEXTAR_FRAMEWORK}.${NEXTAR_BINARY_NAME}.tst")
	set(NEXTAR_TARGET_RELPATH ${NEXTAR_FRAMEWORK}/${NEXTAR_MODULE_PATH}/${NEXTAR_UT})
	set(NEXTAR_TARGET_PATH ${NEX_ROOT}/${NEXTAR_TARGET_RELPATH})
	set(NEXTAR_CONFIG_NAME ${NEXTAR_UT})
	if(${ARGV0})
		add_definitions( -DNEX_CONSOLE_APP_ENTRY )
	endif()
	config_src()
	message("--------------------------------------")
	message("-->> Create Test..: ${NEXTAR_TARGET}")
	place_bin(FALSE)
endmacro(create_ut)

#============================================================
# Create_Plugin
#============================================================
macro(create_plugin pluginName pluginPath type build context optional services)
	set(NEX_INSTALL_PLUGIN ON)
	create_library()
	if (ARGV1 MATCHES "")
		set(PLUG_SUBPATH "${NEXTAR_TARGET}")
	else()
		set(PLUG_SUBPATH "${ARGV1}/${NEXTAR_TARGET}")
	endif()
	file(APPEND ${NEX_PLUGIN_CONFIG_FILE}
	"${NEX_PLUGIN_CONFIG}"
	 " <plugin name='${ARGV0}'>\n"
	 "  <target>${PLUG_SUBPATH}</target>\n"
	 "  <type>${ARGV2}</type>\n"
	 "  <build>${ARGV3}</build>\n"
	 "  <context>${ARGV4}</context>\n"
	 "  <optional>${ARGV5}</optional>\n"
	 "  <services>${ARGV6}</services>\n"
	 " </plugin>\n")

	set(NEX_INSTALL_PLUGIN OFF)
endmacro(create_plugin)

#===========================================================
## variables
set(NEX_CLEAN_UP_FILE_LIST "" CACHE INTERNAL "")
#============================================================
# initialize_build_sys
#============================================================
macro(initialize_build_sys)
endmacro(initialize_build_sys)

#============================================================
# finalize_build_sys
#============================================================
macro(finalize_build_sys)
	# cleanup the dummy generated files
	message("-->> Cleaning up...")
	foreach(FileInList ${NEX_CLEAN_UP_FILE_LIST})
		if (EXISTS ${FileInList})
			file(REMOVE ${FileInList})
		endif()
	endforeach()
endmacro(finalize_build_sys)
## Build Options:
#============================================================
set(NEX_PACKAGE_TYPE "All" CACHE STRING "Package configuration.")
## Build targets
#============================================================
file(GLOB NEX_AVAILABLE_PACKAGES "${NEX_ROOT}/CMake/Scripts/BuildConfig_*.txt")
set(NEX_PACKAGE_LIST "")
foreach(FileName ${NEX_AVAILABLE_PACKAGES})
	string(REGEX MATCH "_([^.]*)[.]+" PackageName ${FileName})
	list(APPEND NEX_PACKAGE_LIST "${CMAKE_MATCH_1}")
endforeach()
set_property(CACHE NEX_PACKAGE_TYPE PROPERTY STRINGS ${NEX_PACKAGE_LIST})

## Begin here
initialize_build_sys()
## Build Include
include(${NEX_ROOT}/CMake/Scripts/BuildConfig_${NEX_PACKAGE_TYPE}.txt)
## Cleanup
finalize_build_sys()

## @urgent 1. Platform specific build option, a module may or may not be build on certain platform. eg: for_platform
## @urgent 2. Anything apart from this script should only use macros
## @todo 3. Files may not use CMakeLists.txt as name, may just be included to remove dep from cmake.
