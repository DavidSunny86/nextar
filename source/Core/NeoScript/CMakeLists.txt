##---------------------------------------------------
# Nextar build file.
# : libnextech configuration file.
# Abhishek Dey
# 5:17 PM 10/1/2011
##---------------------------------------------------


# Add definition
# prebuild step
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
set(BisonOutput ${CMAKE_CURRENT_LIST_DIR}/gen/parse.neo.cpp)
set(FlexOutput ${CMAKE_CURRENT_LIST_DIR}/gen/lex.neo.cpp)
if(BISON_FOUND AND FLEX_FOUND)
  add_pre_build_execute(
   "${BISON_EXECUTABLE} --output=${BisonOutput} --language=C++ ${CMAKE_CURRENT_LIST_DIR}/src/neo_parser.by"
   "${FLEX_EXECUTABLE} --outfile=${FlexOutput} ${CMAKE_CURRENT_LIST_DIR}/src/neo_lexer.l")
  add_gen_file_src(${BisonOutput} ${FlexOutput})
endif()
# build doesnt know if these are created yet
create_library()
add_chained_dependency(Core Base)
