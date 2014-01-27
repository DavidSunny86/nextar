##---------------------------------------------------
# Nextar build file.
# : FX Compiler plugin configuration for nextar shaders.
# Abhishek Dey
# 5:56 PM 9/25/2011
##---------------------------------------------------

set(NEXSHADERCOMPILER_ROOT ${NEX_PLUGINS}/ShaderCompiler)
set(NEXSHADERCOMPILER_INCLUDE ${NEXSHADERCOMPILER_ROOT}/include)
set(NEXSHADERCOMPILER_SRC ${NEXSHADERCOMPILER_ROOT}/src)

## Source files
file(GLOB NEXSHADERCOMPILER_SRC_LIST 
	${NEXSHADERCOMPILER_SRC}/*.cpp 
)

## Project
add_library(ShaderCompiler SHARED ${NEXSHADERCOMPILER_SRC_LIST})

## Platform specific dependencies
#===========================================================
# Linux
include_directories(${NEXSHADERCOMPILER_INCLUDE})
target_link_libraries(ShaderCompiler ${LIBNEXTECH})

# Add plugin
add_plugin("ShaderCompiler" "" "Lifetime" "${NEX_VERSION}" "false")

