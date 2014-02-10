##---------------------------------------------------
# Nextar build file.
# Abhishek Dey
# : Plugin configuration file.
# 5:56 PM 9/25/2011
##---------------------------------------------------

## Configure paths
#============================================================
set(NEX_UT_COMMONS ${PROJECT_SOURCE_DIR}/UnitTests/Commons)
set(NEX_UT_COMMONS_SRC ${NEX_UT_COMMONS}/src/*.cpp)
set(NEX_UT_COMMONS_INCLUDE ${NEX_UT_COMMONS}/include)

add_subdirectory(LibNexTech.tst)
add_subdirectory(RenderEngine.tst)
