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
message("@Todo Make all libraries optional."
  "Use a config.h for selective inclusion")
if(NEX_OS MATCHES "Windows")
 # JPEG
 if(NOT LIBJPEG_ROOT)
  set(LIBJPEG_ROOT ${NEX_DEPENDENCIES}/libjpeg)
 endif(NOT LIBJPEG_ROOT)
 # PNG
  if(NOT PNG_ROOT)
  set(PNG_NAMES libpng16)
  set(PNG_ROOT ${NEX_DEPENDENCIES}/libpng)
 endif(NOT PNG_ROOT)
endif(NEX_OS MATCHES "Windows")

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

