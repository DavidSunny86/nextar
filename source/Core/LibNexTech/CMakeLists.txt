##---------------------------------------------------
# Nextar build file.
# : libnextech configuration file.
# Abhishek Dey
# 5:17 PM 10/1/2011
##---------------------------------------------------

# Source list
#===========================================================
set(NEXTECH_SRC ${NEXTECH_ROOT}/src)

file(GLOB NEX_SRC_LIST ${NEXTECH_SRC}/*.cpp)

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
find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
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


