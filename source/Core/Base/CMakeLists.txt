##---------------------------------------------------
# Nextar build file.
# : libnextech configuration file.
# Abhishek Dey
# 5:17 PM 10/1/2011
##---------------------------------------------------

# Source list
#===========================================================
set(NEXCORE_BASE_SRC ${NEX_CORE}/Base/src)
set(NEXCORE_BASE_INCLUDE ${NEX_CORE}/Base/include)

if(NEX_OS MATCHES "Windows")
 set(NEXTECH_PLATFORM_SRC ${NEXCORE_BASE_SRC}/platform/win32)
elseif(NEX_OS MATCHES "Linux")
 set(NEXTECH_PLATFORM_SRC ${NEXCORE_BASE_SRC}/platform/linux)
elseif(NEX_OS MATCHES "MacOSX")
 set(NEXTECH_PLATFORM_SRC ${NEXCORE_BASE_SRC}/platform/macosx)
endif(NEX_OS MATCHES "Windows")

file(GLOB NEX_SRC_LIST 
	${NEXCORE_BASE_SRC}/*.cpp
	${NEXTECH_PLATFORM_SRC}/*.cpp
	)

# Add definition

## Configuration for libnextech
#============================================================
configure_file(
   "${NEX_BASE_INCLUDE}/config/Config.h.cmake"
   "${NEX_BASE_INCLUDE}/config/Config.h"
)

# Compiler flags: GCC
#===========================================================

# Set flags
#===========================================================

# Include 
#===========================================================
include_directories(${NEXCORE_BASE_INCLUDE})

# Add library
#===========================================================
add_library(NexBase SHARED ${NEX_SRC_LIST})

## Find dependencies
#===========================================================
# dlld
target_link_libraries(NexBase ${CMAKE_DL_LIBS})

# zlib
if(NEX_OS MATCHES "Windows")
 if((NOT ZLIB_ROOT) AND (NOT ENV{ZLIB_ROOT}))
  message("Defaulting to dependencies/zlib")
  set(ZLIB_ROOT ${NEX_DEPENDENCIES}/zlib)
 endif((NOT ZLIB_ROOT) AND (NOT ENV{ZLIB_ROOT}))
endif(NEX_OS MATCHES "Windows")

find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
 include_directories( ${ZLIB_INCLUDE_DIRS} )
 target_link_libraries(NexBase ${ZLIB_LIBRARIES}) 
endif (ZLIB_FOUND)

## Platform specific dependencies
#===========================================================

# Linux
if ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
# package requirements
# X11
find_package(X11 REQUIRED)
if (X11_FOUND)
 include_directories( ${X11_INCLUDE_DIR} )
 target_link_libraries(NexBase ${X11_LIBRARIES})
endif (X11_FOUND)
endif ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )


