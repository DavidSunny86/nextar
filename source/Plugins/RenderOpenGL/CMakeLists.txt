##---------------------------------------------------
# Nextar build file.
# : FX Compiler plugin configuration for nextar shaders.
# Abhishek Dey
# 5:56 PM 9/25/2011
##---------------------------------------------------

# Add definition
# Add plugin
create_plugin("RenderOpenGL" "" "Lifetime" "${NEX_VERSION}" "false")
add_dependency(Core Base)
add_dependency(Core Engine)

## Platform specific dependencies
#===========================================================
# Linux
if ( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
# package requirements Xrandr
find_package(X11 REQUIRED)
if (X11_Xrandr_FOUND)
	add_external_dependency("${X11_Xrandr_INCLUDE_DIR}" "${X11_Xrandr_LIB}")
endif (X11_Xrandr_FOUND)

elseif ( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )

endif ( )

## Find dependencies
#===========================================================
find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
	add_external_dependency("${OPENGL_INCLUDE_DIRS}" "${OPENGL_LIBRARIES}")
endif (OPENGL_FOUND)

