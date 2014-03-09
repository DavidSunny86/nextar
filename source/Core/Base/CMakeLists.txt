##---------------------------------------------------
# Nextar build file.
# : libnextech configuration file.
# Abhishek Dey
# 5:17 PM 10/1/2011
##---------------------------------------------------

# Source list
#===========================================================
set(NEXCORE_BASE_SRC ${NEX_ROOT}/Core/Base/src)

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

create_module(Base Core ${NEX_SRC_LIST}) 
add_specific_dependency(Base Core "" ${CMAKE_DL_LIBS})

# zlib
if(NEX_OS MATCHES "Windows")
 if((NOT ZLIB_ROOT) AND (NOT ENV{ZLIB_ROOT}))
  message("Defaulting to dependencies/zlib")
  set(ZLIB_ROOT ${NEX_DEPENDENCIES}/zlib)
 endif((NOT ZLIB_ROOT) AND (NOT ENV{ZLIB_ROOT}))
endif(NEX_OS MATCHES "Windows")

find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
    add_specific_dependency(Base Core ${ZLIB_INCLUDE_DIRS} ${ZLIB_LIBRARIES})
endif (ZLIB_FOUND)

## Platform specific dependencies
#===========================================================
# Linux
if ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    # package requirements
    # X11
    find_package(X11 REQUIRED)
    if (X11_FOUND)
        add_specific_dependency(Base Core ${X11_INCLUDE_DIR} ${X11_LIBRARIES})
    endif (X11_FOUND)
endif ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )


