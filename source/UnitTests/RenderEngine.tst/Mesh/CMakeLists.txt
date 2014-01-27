##---------------------------------------------------
# Nextar build file.
# : File system unit tests.
# Abhishek Dey
##---------------------------------------------------
set(NEX_UT_RE_MESHTEST ${NEX_UNIT_TESTS_RENDERENGINE}/Mesh)
set(NEX_UT_RE_MESHTEST_INCLUDE ${NEX_UT_RE_MESHTEST}/include)
set(NEX_UT_RE_MESHTEST_SRC ${NEX_UT_RE_MESHTEST}/src)

file(GLOB NEX_UT_RE_MESHTEST_SRC_LIST ${NEX_UT_RE_MESHTEST_SRC}/*.cpp)

include_directories(${NEX_UT_RE_MESHTEST_INCLUDE})
# console applicaiton
add_definitions( -DNEX_CONSOLE_APP_ENTRY )
# target
add_executable(Mesh.tst ${NEX_UT_RE_MESHTEST_SRC_LIST})
# libraries
target_link_libraries(Mesh.tst ${LIBNEXTECH})
