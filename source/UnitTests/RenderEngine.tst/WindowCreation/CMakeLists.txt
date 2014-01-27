##---------------------------------------------------
# Nextar build file.
# : File system unit tests.
# Abhishek Dey
##---------------------------------------------------
set(NEX_UT_RE_WINDOWCREATE ${NEX_UNIT_TESTS_RENDERENGINE}/WindowCreation)
set(NEX_UT_RE_WINDOWCREATE_INCLUDE ${NEX_UT_RE_WINDOWCREATE}/include)
set(NEX_UT_RE_WINDOWCREATE_SRC ${NEX_UT_RE_WINDOWCREATE}/src)

file(GLOB NEX_UT_RE_WINDOWCREATE_SRC_LIST ${NEX_UT_RE_WINDOWCREATE_SRC}/*.cpp)

include_directories(${NEX_UT_RE_WINDOWCREATE_INCLUDE})
# console applicaiton
add_definitions( -DNEX_CONSOLE_APP_ENTRY )
# target
add_executable(WindowCreation.tst ${NEX_UT_RE_WINDOWCREATE_SRC_LIST})
# libraries
target_link_libraries(WindowCreation.tst ${LIBNEXTECH})
