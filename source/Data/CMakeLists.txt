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
set(NEX_DATA_TEXTURES ${NEX_DATA}/Textures)
set(NEX_DATA_MESHES ${NEX_DATA}/Meshes)

file(GLOB NEX_DATA_SHADER_LIST ${NEX_DATA_SHADERS}/*)
file(GLOB NEX_DATA_CONFIG_LIST ${NEX_DATA_CONFIG}/*)
file(GLOB NEX_DATA_MESH_LIST ${NEX_DATA_MESHES}/*)

set(DATA_SRC_LIST 
	${NEX_DATA_SHADER_LIST}
	${NEX_DATA_CONFIG_LIST}
	${NEX_DATA_MESH_LIST}
)
# Add definition

# Compiler flags: GCC
#===========================================================

# Macros
#===========================================================
macro(prepare_data subDir)
	file(GLOB DATA_FILE_GLOB ${NEX_DATA}/${ARGV0}/*)
	set(DATA_SRC_LIST ${DATA_SRC_LIST} ${DATA_FILE_GLOB})
	source_group("[${ARGV0}]" FILES ${DATA_FILE_GLOB})
endmacro(prepare_data)

macro(copy_data subDir)
	add_custom_command(TARGET NE_Data POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${NEX_DATA}/${ARGV0}"
			${NEX_INSTALL}/Data/${ARGV0})
endmacro(copy_data)

# Add library
#===========================================================
prepare_data(Shaders)
prepare_data(Config)
prepare_data(Meshes)

add_custom_target(NE_Data ALL SOURCES ${DATA_SRC_LIST})

copy_data(Shaders)
copy_data(Config)
copy_data(Meshes)
