/*
 * PluginGL.cpp
 *
 *  Created on: 03-Oct-2013
 *      Author: obhi
 */

#include <PluginGL.h>

NEX_IMPLEMENT_PLUGIN(RenderOpenGL){
return &(RenderOpenGL::PluginGL::pluginObject);
}

namespace RenderOpenGL {

PluginGL PluginGL::pluginObject;

void DebugGL::CheckGL() {
	GLenum error;
	if ((error = glGetError()) == GL_NO_ERROR)
		return;
	Warn(String("OpenGl error code: ") + Convert::ToString((uint32) error));
}

void PluginGL::Open() {

}

void PluginGL::LicenseRenewed() {
	ApplicationContext::GetSingleton().RegisterRenderManager(&renderManagerGl);
}

bool PluginGL::LicenseExpired() {
	ApplicationContext::GetSingleton().UnregisterRenderManager(
			&renderManagerGl);
	return true;
}

void PluginGL::Close() {

}

void PluginGL::Dispose() {

}
}
