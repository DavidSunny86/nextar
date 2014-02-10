##---------------------------------------------------
# Nextar build file.
# Abhishek Dey
# 5:56 PM 9/25/2011
# : Plugin configuration file.
##---------------------------------------------------

## Configure paths
#============================================================
set(NEX_UT_LIBNEXTECH ${PROJECT_SOURCE_DIR}/UnitTests/LibNexTech.tst)
set(NEX_UT_LIBNEXTECH_COMMONS ${NEX_UT_LIBNEXTECH}/Commons)
set(NEX_UT_LIBNEXTECH_COMMONS_SRC ${NEX_UT_COMMONS_SRC};${NEX_UT_LIBNEXTECH_COMMONS}/src/*.cpp)
set(NEX_UT_LIBNEXTECH_COMMONS_INCLUDE ${NEX_UT_COMMONS_INCLUDE};${NEX_UT_LIBNEXTECH_COMMONS}/include)

add_subdirectory(FileSystem)
