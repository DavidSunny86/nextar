##---------------------------------------------------
# Nextar build file.
# Abhishek Dey
# : Plugin configuration file.
# 5:56 PM 9/25/2011
##---------------------------------------------------

## Configure paths
#============================================================
file(WRITE ${NEX_PLUGIN_CONFIG_FILE} "<plugins basePath='\${Bin}/plugins'>\n")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${NEX_LIB}/plugins)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${NEX_BIN}/plugins)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${NEX_BIN}/plugins)

## Configure paths
#============================================================
add_subdirectory(RenderOpenGL)
add_subdirectory(ShaderCompiler)
add_subdirectory(ImageLoader)
add_subdirectory(MeshLoader)

