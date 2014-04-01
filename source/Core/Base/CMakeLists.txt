##---------------------------------------------------
# Nextar build file.
# : libnextech configuration file.
# Abhishek Dey
# 5:17 PM 10/1/2011
##---------------------------------------------------

# Source list
#===========================================================

create_library()
add_external_dependency("" ${CMAKE_DL_LIBS})

# zlib
if(NEX_OS MATCHES "Windows")
 if((NOT ZLIB_ROOT) AND (NOT ENV{ZLIB_ROOT}))
  message("Defaulting to dependencies/zlib")
  set(ZLIB_ROOT ${NEX_DEPENDENCIES}/zlib)
 endif((NOT ZLIB_ROOT) AND (NOT ENV{ZLIB_ROOT}))
endif(NEX_OS MATCHES "Windows")

find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
    add_external_dependency(${ZLIB_INCLUDE_DIRS} ${ZLIB_LIBRARIES})
endif (ZLIB_FOUND)

## Platform specific dependencies
#===========================================================
# Linux
if ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    # package requirements
    # X11
    find_package(X11 REQUIRED)
    if (X11_FOUND)
        add_external_dependency(${X11_INCLUDE_DIR} ${X11_LIBRARIES})
    endif (X11_FOUND)
endif ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )

