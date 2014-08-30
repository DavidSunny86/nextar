##---------------------------------------------------
# Nextar build file.
# : Image Loader configuration
# Abhishek Dey
# 11/02/2013
##---------------------------------------------------

# Add definition
create_exec()
add_dependency(Core ProjectModel)
set(FBX_INCLUDE "C:/Program Files/Autodesk/FBX/FbxSdk/2012.1/include")
set(FBX_LIBRARIES "C:/Program Files/Autodesk/FBX/FbxSdk/2012.1/lib/vs2010/x64/fbxsdk-2012.1-mt.lib")
add_external_dependency("${FBX_INCLUDE}" "${FBX_LIBRARIES}")
# Add plugin



