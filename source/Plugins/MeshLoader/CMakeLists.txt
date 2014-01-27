##---------------------------------------------------
# Nextar build file.
# : Image Loader configuration
# Abhishek Dey
# 11/02/2013
##---------------------------------------------------

set(NEXMESHLD_ROOT ${NEX_PLUGINS}/MeshLoader)
set(NEXMESHLD_INCLUDE ${NEXMESHLD_ROOT}/include)
set(NEXMESHLD_SRC ${NEXMESHLD_ROOT}/src)

## Source files
file(GLOB NEXMESHLD_SRC_LIST 
	${NEXMESHLD_SRC}/*.cpp 
)

## Project
add_library(MeshLoader SHARED ${NEXMESHLD_SRC_LIST})

## Platform specific dependencies
#===========================================================


include_directories(${NEXMESHLD_INCLUDE})
target_link_libraries(MeshLoader ${LIBNEXTECH})

## Find dependencies
#===========================================================

# Add plugin
add_plugin("MeshLoader" "" "Lifetime" "${NEX_VERSION}" "false")


