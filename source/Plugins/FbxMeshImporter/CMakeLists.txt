##---------------------------------------------------
# Nextar build file.
# : Image Loader configuration
# Abhishek Dey
# 11/02/2013
##---------------------------------------------------

# Add definition
create_plugin("FbxMeshImporter" "" "Lifetime" "${NEX_VERSION}" "project" "false")
add_dependency(Core ProjectModel)
find_package(FBX REQUIRED)
if (FBX_FOUND)
	add_definitions(-DFBXSDK_SHARED)
	add_external_dependency("${FBX_INCLUDE}" "${FBX_LIBRARY}")
	copy_binary_to_out("${FBX_BINARY}")
endif(FBX_FOUND)

# Add plugin



