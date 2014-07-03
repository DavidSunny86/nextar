/*
 * IncludeGL.h
 *
 *  Created on: 17-Aug-2013
 *      Author: obhi
 */

#ifndef INCLUDEGL_H_
#define INCLUDEGL_H_

#if NEX_GLE_LINKING == NEX_GLE_LINKING_STATIC
#define GL_GLEXT_PROTOTYPES
#endif

#if defined (NEX_LINUX)
#   include <X11/X.h>    /* X11 constant (e.g. TrueColor) */
#   include <GL/glx.h>    /* this includes the necessary X headers */
#   include <GL/gl.h>
#   include <GL/glext.h>
#elif defined (NEX_WINDOWS)
#   include <windows.h>
#   include <GL/gl.h>
#   include <GL/glext.h>
#	include <GL/wglext.h>
#elif defined (NEX_MACOSX)
#   include <OpenGL/glx.h>    /* this includes the necessary X headers */
#   include <OpenGL/gl.h>
#else
#   error NOT Supported
#endif

#if defined (NEX_DEBUG)

#define GL_CHECK() RenderOpenGL::DebugGL::CheckGL(NEX_FUNCTION_NAME, NEX_SOURCEFILE_LINE)

#else

#define GL_CHECK();

#endif

#endif /* INCLUDEGL_H_ */
