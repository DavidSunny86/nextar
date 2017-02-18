# Find FBX
# expects:
# FBX_DIR - set to the root folder where include and lib reside
# defines:
# FBX_INCLUDE - as include path
# FBX_LIBRARY - as library path
# FBX_LIBRARY_DEBUG - as debug library path


if(APPLE)
	set(FBX_LIBDIR "gcc4/ub")
elseif(CMAKE_COMPILER_IS_GNUCXX)
	set(FBX_LIBDIR "gcc4")
elseif(MSVC80)
	set(FBX_LIBDIR "vs2005")
elseif(MSVC90)
	set(FBX_LIBDIR "vs2008")
elseif(MSVC10)
	set(FBX_LIBDIR "vs2010")
elseif(MSVC11)
	set(FBX_LIBDIR "vs2012")
elseif(MSVC12)
	set(FBX_LIBDIR "vs2013")
elseif(${MSVC_VERSION} GREATER 1800)
	set(FBX_LIBDIR "vs2015")
endif()

if(APPLE)
    # do nothing
elseif(CMAKE_CL_64)
	set(FBX_LIBDIR ${FBX_LIBDIR}/x64)
elseif(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(FBX_LIBDIR ${FBX_LIBDIR}/x64)
else()
	SET(FBX_LIBDIR ${FBX_LIBDIR}/x86)
endif()
	
if(APPLE)
	set(FBX_LIBNAME "libfbxsdk")
elseif(CMAKE_COMPILER_IS_GNUCXX)
	set(FBX_LIBNAME "fbxsdk")
else()
	set(FBX_LIBNAME "libfbxsdk")
endif()

set( FBX_SEARCH_PATHS
	    $ENV{FBX_DIR}
		"$ENV{ProgramW6432}/Autodesk/FBX/FBX SDK/2017.1"
		"$ENV{PROGRAMFILES}/Autodesk/FBX/FBX SDK/2017.1"
	    "$ENV{ProgramW6432}/Autodesk/FBX/FBX SDK/2015.2"
	    "$ENV{PROGRAMFILES}/Autodesk/FBX/FBX SDK/2015.2"
		"$ENV{ProgramW6432}/Autodesk/FBX/FBX SDK/2015.1"
	    "$ENV{PROGRAMFILES}/Autodesk/FBX/FBX SDK/2015.1"
	    /Applications/Autodesk/FBXSDK20152
		/Applications/Autodesk/FBXSDK20151
	)

find_path(FBX_INCLUDE "fbxsdk.h"
	    PATHS ${FBX_SEARCH_PATHS}
	    PATH_SUFFIXES "include")
find_library(FBX_LIBRARY ${FBX_LIBNAME}
	    PATHS ${FBX_SEARCH_PATHS}
	    PATH_SUFFIXES "lib/${FBX_LIBDIR}/release" "lib/${FBX_LIBDIR}")
find_library( FBX_LIBRARY_DEBUG ${FBX_LIBNAME}
	    PATHS ${FBX_SEARCH_PATHS}
	    PATH_SUFFIXES "lib/${FBX_LIBDIR}/debug")

if(APPLE)
find_path(FBX_BINARY "${FBX_LIBNAME}.dylib"
	    PATHS ${FBX_SEARCH_PATHS}
	    PATH_SUFFIXES "lib/${FBX_LIBDIR}/release" "lib/${FBX_LIBDIR}")
find_path(FBX_BINARY_DEBUG "${FBX_LIBNAME}.dylib"
	    PATHS ${FBX_SEARCH_PATHS}
	    PATH_SUFFIXES "lib/${FBX_LIBDIR}/debug" "lib/${FBX_LIBDIR}")
elseif(WIN32)
find_path(FBX_BINARY "${FBX_LIBNAME}.dll"
	    PATHS ${FBX_SEARCH_PATHS}
	    PATH_SUFFIXES "lib/${FBX_LIBDIR}/release" "lib/${FBX_LIBDIR}")
find_path(FBX_BINARY_DEBUG "${FBX_LIBNAME}.dll"
	    PATHS ${FBX_SEARCH_PATHS}
	    PATH_SUFFIXES "lib/${FBX_LIBDIR}/debug" "lib/${FBX_LIBDIR}")
elseif(UNIX)
find_path(FBX_BINARY "lib${FBX_LIBNAME}.so"
	    PATHS ${FBX_SEARCH_PATHS}
	    PATH_SUFFIXES "lib/${FBX_LIBDIR}/release" "lib/${FBX_LIBDIR}")
find_path(FBX_BINARY_DEBUG "lib${FBX_LIBNAME}.so"
	    PATHS ${FBX_SEARCH_PATHS}
	    PATH_SUFFIXES "lib/${FBX_LIBDIR}/debug" "lib/${FBX_LIBDIR}")
endif()

if(FBX_LIBRARY AND FBX_LIBRARY_DEBUG AND FBX_INCLUDE AND FBX_BINARY AND FBX_BINARY_DEBUG)
    set(FBX_FOUND "YES")
else()
    set(FBX_FOUND "NO")
endif()

