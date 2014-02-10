##---------------------------------------------------
# Nextar build file.
# : libnextech configuration file.
# Abhishek Dey
# 5:17 PM 10/1/2011
##---------------------------------------------------

# Source list
#===========================================================
set(NEX_DATA_SHADERS ${NEX_DATA}/Shaders)
set(NEX_DATA_CONFIG ${NEX_DATA}/Config)

file(GLOB NEX_DATA_SHADERS_LIST ${NEX_DATA_SHADERS}/*)
file(GLOB NEX_DATA_CONFIG_LIST ${NEX_DATA_CONFIG}/*)

set(DATA_SRC_LIST 
	${NEX_DATA_SHADERS_LIST}
	${NEX_DATA_CONFIG_LIST}
)
# Add definition

# Compiler flags: GCC
#===========================================================

# Set flags
#===========================================================

# Add library
#===========================================================
add_custom_target(Data ALL SOURCES ${DATA_SRC_LIST})

