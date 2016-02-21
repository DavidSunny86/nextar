/*
 * Plugin.cpp
 *
 *  Created on: 30-Sep-2013
 *      Author: obhi
 */
#include <BaseHeaders.h>
#include <Plugin.h>
#include <RenderManager.h>

NEX_IMPLEMENT_PLUGIN(RenderSystemScript, RenderSystemScript::Plugin);

namespace RenderSystemScript {

Plugin::Plugin() {
}

Plugin::~Plugin() {
}

void Plugin::Open() {
	Trace("Loaded render system script loader 1.0.");
	_streamer.RegisterDictionary();
	RenderManager::Instance().AddRenderStreamer("RSS", &_streamer);

}

void Plugin::Close() {
	Trace("Unloading render system script loader 1.0.");
	RenderManager::Instance().RemoveRenderStreamer("RSS");
	_streamer.UnregisterDictionary();
}

void Plugin::LicenseRenewed() {
}

bool Plugin::LicenseExpired() {
	return true;
}

void Plugin::Dispose() {
}
} /* namespace ShaderCompiler */
