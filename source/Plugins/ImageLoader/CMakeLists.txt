##---------------------------------------------------
# Nextar build file.
# : Image Loader configuration
# Abhishek Dey
# 11/02/2013
##---------------------------------------------------

# Add definition
create_plugin("ImageLoader" "" "Lifetime" "${NEX_VERSION}" "false")
add_dependency(Core Engine)

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
 add_external_dependency("${LIBJPEG_INCLUDE_DIRS}" "${LIBJPEG_LIBRARIES}")
endif (LIBJPEG_FOUND)

find_package(PNG REQUIRED)
if (PNG_FOUND)
 if (NOT PNG_INCLUDE_DIRS)
	set(PNG_INCLUDE_DIRS "${PNG_PNG_INCLUDE_DIR}")
 endif()
 if (NOT PNG_LIBRARIES)
	set(PNG_LIBRARIES "${PNG_LIBRARY}")
 endif()
 add_external_dependency("${PNG_INCLUDE_DIRS}" "${PNG_LIBRARIES}")
endif (PNG_FOUND)
add_definitions(${PNG_DEFINITIONS})

