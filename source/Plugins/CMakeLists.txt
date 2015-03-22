##---------------------------------------------------
# Nextar build file.
# Abhishek Dey
# : Plugin configuration file.
# 5:56 PM 9/25/2011
##---------------------------------------------------

## Configure paths
#============================================================
file(WRITE ${NEX_PLUGIN_CONFIG_FILE} "<plugins basePath='\${Bin}/plugins'>\n")

#============================================================
## StartAdd
add_module(RenderOpenGL)
add_module(ImageLoader)
add_module(MeshLoader)
add_module(MaterialLoader)
add_module(ShaderLoader)
add_module(ShaderCompiler)
add_module(MaterialCompiler)
add_module(FbxMeshImporter)
add_module(InputService)
## EndAdd

file(APPEND ${NEX_PLUGIN_CONFIG_FILE} "</plugins>\n")
