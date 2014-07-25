/*
 * PluginGL.cpp
 *
 *  Created on: 03-Oct-2013
 *      Author: obhi
 */

#include <PluginGL.h>

NEX_IMPLEMENT_PLUGIN(RenderOpenGL, RenderOpenGL::PluginGL);

namespace RenderOpenGL {

void DebugGL::CheckGL(const char* function, long line) {
	GLenum error;
	if ((error = glGetError()) == GL_NO_ERROR)
		return;
	OutStringStream errorStr;
	errorStr << "OpenGl error code: " << Convert::ToString((uint32) error) << " In: " << function << " Line: " << line;
	Warn(errorStr.str());
}

void PluginGL::Open() {

}

void PluginGL::LicenseRenewed() {
	renderManagerGl = NEX_NEW(RenderManagerGL());
}

bool PluginGL::LicenseExpired() {
	NEX_DELETE(renderManagerGl);
	return true;
}

void PluginGL::Close() {

}

void PluginGL::Dispose() {

}
}
