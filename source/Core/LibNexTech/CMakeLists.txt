##---------------------------------------------------
# Nextar build file.
# : libnextech configuration file.
# Abhishek Dey
# 5:17 PM 10/1/2011
##---------------------------------------------------

# Source list
#===========================================================
set(NEXTECH_SRC ${NEXTECH_ROOT}/src)

if(NEX_OS MATCHES "Windows")
 set(NEXTECH_PLATFORM_SRC ${NEXTECH_ROOT}/src/win32)
elseif(NEX_OS MATCHES "Linux")
 set(NEXTECH_PLATFORM_SRC ${NEXTECH_ROOT}/src/linux)
elseif(NEX_OS MATCHES "MacOSX")
 set(NEXTECH_PLATFORM_SRC ${NEXTECH_ROOT}/src/macosx)
endif(NEX_OS MATCHES "Windows")

file(GLOB NEX_SRC_LIST 
	${NEXTECH_SRC}/*.cpp
	${NEXTECH_PLATFORM_SRC}/*.cpp
	)

# Add definition
add_definitions( -DNEXTAR_BASE_EXPORTS )

# Compiler flags: GCC
#===========================================================

# Set flags
#===========================================================

# Add library
#===========================================================
add_library(${LIBNEXTECH} SHARED ${NEX_SRC_LIST})

## Find dependencies
#===========================================================
# dlld
target_link_libraries(${LIBNEXTECH} ${CMAKE_DL_LIBS})

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
 target_link_libraries(${LIBNEXTECH} ${ZLIB_LIBRARIES}) 
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
 target_link_libraries(${LIBNEXTECH} ${X11_LIBRARIES})
endif (X11_FOUND)
endif ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )


