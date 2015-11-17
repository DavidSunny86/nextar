##---------------------------------------------------
# Nextar build file.
# : libnextech configuration file.
# Abhishek Dey
# 5:17 PM 10/1/2011
##---------------------------------------------------

# Source list
#===========================================================
set(NEX_DATA_SHADERS_GLSL ${NEX_DATA}/Shaders/GLSL)
set(NEX_DATA_EFFECTS ${NEX_DATA}/Effects)
set(NEX_DATA_CONFIGS ${NEX_DATA}/Configs)
set(NEX_DATA_TEXTURES ${NEX_DATA}/Textures)
set(NEX_DATA_TEXTURE_ASSETS ${NEX_DATA}/Textures/Assets)
set(NEX_DATA_MESHES ${NEX_DATA}/Meshes)
set(NEX_DATA_MESH_ASSETS ${NEX_DATA}/Meshes/Assets)
set(NEX_DATA_MATERIALS ${NEX_DATA}/Materials)
set(NEX_DATA_MATERIAL_ASSETS ${NEX_DATA}/Materials/Assets)
set(NEX_DATA_SCRIPTS ${NEX_DATA}/Scripts)

file(GLOB_RECURSE NEX_DATA_SHADERS_GLSL_LIST ${NEX_DATA_SHADERS_GLSL}/*)
file(GLOB_RECURSE NEX_DATA_EFFECTS_LIST ${NEX_DATA_EFFECTS}/*)
file(GLOB_RECURSE NEX_DATA_CONFIG_LIST ${NEX_DATA_CONFIGS}/*)
file(GLOB_RECURSE NEX_DATA_TEXTURE_LIST ${NEX_DATA_TEXTURES}/*)
file(GLOB_RECURSE NEX_DATA_TEXTURE_ASSET_LIST ${NEX_DATA_TEXTURE_ASSETS}/*)
file(GLOB_RECURSE NEX_DATA_MESH_LIST ${NEX_DATA_MESHES}/*)
file(GLOB_RECURSE NEX_DATA_MESH_ASSET_LIST ${NEX_DATA_MESH_ASSETS}/*)
file(GLOB_RECURSE NEX_DATA_MATERIAL_LIST ${NEX_DATA_MATERIALS}/*)
file(GLOB_RECURSE NEX_DATA_MATERIAL_ASSET_LIST ${NEX_DATA_MATERIAL_ASSETS}/*)
file(GLOB_RECURSE NEX_DATA_SCRIPTS_LIST ${NEX_DATA_SCRIPTS}/*)

set(DATA_SRC_LIST 
	${NEX_DATA_SHADERS_GLSL_LIST}
	${NEX_DATA_EFFECTS_LIST}
	${NEX_DATA_CONFIG_LIST}
	${NEX_DATA_TEXTURE_LIST}
   ${NEX_DATA_TEXTURE_ASSET_LIST}
	${NEX_DATA_MESH_LIST}
   ${NEX_DATA_MESH_ASSET_LIST}
	${NEX_DATA_MATERIAL_LIST}
	${NEX_DATA_MATERIAL_ASSET_LIST}
	${NEX_DATA_SCRIPTS_LIST}
)
# Add definition

# Compiler flags: GCC
#===========================================================

# Macros
#===========================================================
macro(prepare_data subDir)
	file(GLOB_RECURSE DATA_FILE_GLOB ${NEX_DATA}/${ARGV0}/*)
	set(DATA_SRC_LIST ${DATA_SRC_LIST} ${DATA_FILE_GLOB})
	source_group("[${ARGV0}]" FILES ${DATA_FILE_GLOB})
endmacro(prepare_data)

macro(copy_data subDir)
	add_custom_command(TARGET NE_Data POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${NEX_DATA}/${ARGV0}"
			${NEX_INSTALL}/Data/${ARGV0}
		COMMENT "Copied ${NEX_INSTALL}/Data/${ARGV0}.")
endmacro(copy_data)

# Add library
#===========================================================
prepare_data(Shaders)
prepare_data(Configs)
prepare_data(Textures)
prepare_data("Textures/Assets")
prepare_data(Meshes)
prepare_data("Meshes/Assets")
prepare_data(Materials)
prepare_data("Materials/Assets")
prepare_data(Scripts/Materials)
prepare_data(Scripts/Shaders)
prepare_data(Scripts/Effects)
prepare_data(Scripts/ConstBuffers)
prepare_data(Scripts/NeoScriptUnits)

add_custom_target(NE_Data ALL 
	SOURCES ${DATA_SRC_LIST})

copy_data(Shaders)
copy_data(Configs)
copy_data(Textures)
copy_data("Textures/Assets")
copy_data(Meshes)
copy_data("Meshes/Assets")
copy_data(Materials)
copy_data("Materials/Assets")
copy_data(Scripts)
