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
add_module(SimpleBehaviours)
<<<<<<< HEAD
=======
add_module(PBRenderEngine)
>>>>>>> 14d8c5bb1272811606af0c2487b2480748153031
## EndAdd

file(APPEND ${NEX_PLUGIN_CONFIG_FILE} "</plugins>\n")
