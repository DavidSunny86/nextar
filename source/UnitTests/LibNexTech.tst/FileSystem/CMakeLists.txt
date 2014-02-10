##---------------------------------------------------
# Nextar build file.
# : File system unit tests.
# Abhishek Dey
##---------------------------------------------------
set(NEX_UT_NEXTECH_FILESYS ${NEX_UT_LIBNEXTECH}/FileSystem)
set(NEX_UT_NEXTECH_FILESYS_INCLUDE ${NEX_UT_NEXTECH_FILESYS}/include)
set(NEX_UT_NEXTECH_FILESYS_SRC ${NEX_UT_NEXTECH_FILESYS}/src)

file(GLOB NEX_UT_NEXTECH_FILESYS_SRC_LIST ${NEX_UT_LIBNEXTECH_COMMONS_SRC};${NEX_UT_NEXTECH_FILESYS_SRC}/*.cpp)

include_directories(${NEX_UT_LIBNEXTECH_COMMONS_INCLUDE};${NEX_UT_NEXTECH_FILESYS_INCLUDE})
# console applicaiton
add_definitions( -DNEX_CONSOLE_APP_ENTRY )
# target
add_executable(FileSys.tst ${NEX_UT_NEXTECH_FILESYS_SRC_LIST})
# libraries
target_link_libraries(FileSys.tst ${LIBNEXTECH})
