##---------------------------------------------------
# Nextar build file.
# Abhishek Dey
# : Plugin configuration file.
# 5:56 PM 9/25/2011
##---------------------------------------------------

## Configure paths
#============================================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${NEX_LIB}/plugins)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${NEX_BIN}/plugins)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${NEX_BIN}/plugins)

file(WRITE ${NEX_PLUGIN_CONFIG_FILE} "<plugins basePath='\${Bin}/plugins'>\n")

## Configure paths
#============================================================
add_module(RenderOpenGL)
add_module(ShaderCompiler)
add_module(ImageLoader)
add_module(MeshLoader)

file(APPEND ${NEX_PLUGIN_CONFIG_FILE} "</plugins>\n")
