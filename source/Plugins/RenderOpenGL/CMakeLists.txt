##---------------------------------------------------
# Nextar build file.
# : FX Compiler plugin configuration for nextar shaders.
# Abhishek Dey
# 5:56 PM 9/25/2011
##---------------------------------------------------

set(NEXOGL_ROOT ${NEX_PLUGINS}/RenderOpenGL)
set(NEXOGL_INCLUDE ${NEXOGL_ROOT}/include)
set(NEXOGL_SRC ${NEXOGL_ROOT}/src)

## Source files
if ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )

file(GLOB NEXOGL_SRC_LIST 
	${NEXOGL_SRC}/*.cpp 
	${NEXOGL_SRC}/glx/*.cpp
)

elseif ( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )

file(GLOB NEXOGL_SRC_LIST 
	${NEXOGL_SRC}/*.cpp 
	${NEXOGL_SRC}/wgl/*.cpp
)

endif ( )

## Project
add_library(RenderOpenGL SHARED ${NEXOGL_SRC_LIST})

## Platform specific dependencies
#===========================================================
# Linux
if ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )

# package requirements Xrandr
find_package(X11 REQUIRED)
if (X11_Xrandr_FOUND)
 include_directories( ${X11_Xrandr_INCLUDE_DIR} )
 target_link_libraries(RenderOpenGL ${X11_Xrandr_LIB} )
endif (X11_Xrandr_FOUND)

elseif ( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )

endif ( )

include_directories(${NEXOGL_INCLUDE})
target_link_libraries(RenderOpenGL ${LIBNEXTECH})

## Find dependencies
#===========================================================
find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
 include_directories( ${OPENGL_INCLUDE_DIRS} )
 target_link_libraries(RenderOpenGL ${OPENGL_LIBRARIES})
endif (OPENGL_FOUND)

# Add plugin
add_plugin("RenderOpenGL" "" "Lifetime" "${NEX_VERSION}" "false")

