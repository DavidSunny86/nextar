##---------------------------------------------------
# Nextar build file.
# : Image Loader configuration
# Abhishek Dey
# 11/02/2013
##---------------------------------------------------

set(NEXIMGLD_ROOT ${NEX_PLUGINS}/ImageLoader)
set(NEXIMGLD_INCLUDE ${NEXIMGLD_ROOT}/include)
set(NEXIMGLD_SRC ${NEXIMGLD_ROOT}/src)

## Source files
file(GLOB NEXIMGLD_SRC_LIST 
	${NEXIMGLD_SRC}/*.cpp 
)

## Project
add_library(ImageLoader SHARED ${NEXIMGLD_SRC_LIST})

## Platform specific dependencies
#===========================================================


include_directories(${NEXIMGLD_INCLUDE})
target_link_libraries(ImageLoader ${LIBNEXTECH})

## Find dependencies
#===========================================================
find_package(LIBJPEG REQUIRED)
if (LIBJPEG_FOUND)
 include_directories( ${LIBJPEG_INCLUDE_DIRS} )
 target_link_libraries(ImageLoader ${LIBJPEG_LIBRARIES})
endif (LIBJPEG_FOUND)

find_package(PNG REQUIRED)
if (PNG_FOUND)
 include_directories( ${PNG_INCLUDE_DIRS} )
 target_link_libraries(ImageLoader ${PNG_LIBRARIES})
endif (PNG_FOUND)
add_definitions(${PNG_DEFINITIONS})

# Add plugin
add_plugin("ImageLoader" "" "Lifetime" "${NEX_VERSION}" "false")

