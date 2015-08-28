# Module created
# Dated: Sat Aug 22 17:06:28 IST 2015
#-------------------------------
# Build using ExternalProject module.
for_platform("Linux")
include(ExternalProject)
ExternalProject_Add(pbrt 
    # build
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
	CONFIGURE_COMMAND echo 'Configured...'
	BUILD_COMMAND make
	BUILD_IN_SOURCE 1
    # install - copy binaries
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ./bin ${NEX_INSTALL}/Bin/pbrt
)


