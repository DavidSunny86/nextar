##---------------------------------------------------
# Nextar build file.
# Abhishek Dey
# 5:56 PM 9/25/2011
# : Plugin configuration file.
##---------------------------------------------------

## Configure paths
#============================================================
set(NEX_UT_RENDERENGINE ${PROJECT_SOURCE_DIR}/UnitTests/RenderEngine.tst)
set(NEX_UT_RENDERENGINE_COMMONS ${NEX_UT_RENDERENGINE}/Commons)
set(NEX_UT_RENDERENGINE_COMMONS_SRC ${NEX_UT_COMMONS_SRC};${NEX_UT_RENDERENGINE_COMMONS}/src/*.cpp)
set(NEX_UT_RENDERENGINE_COMMONS_INCLUDE ${NEX_UT_COMMONS_INCLUDE};${NEX_UT_RENDERENGINE_COMMONS}/include)

add_subdirectory(WindowCreation)
add_subdirectory(Mesh)