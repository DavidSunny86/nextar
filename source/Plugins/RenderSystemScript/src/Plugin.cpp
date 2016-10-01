/*
 * Plugin.cpp
 *
 *  Created on: 30-Sep-2013
 *      Author: obhi
 */
#include <BaseHeaders.h>
#include <Plugin.h>
#include <RenderManager.h>
#include <RenderScriptContext.h>

NEX_IMPLEMENT_PLUGIN(RenderSystemScript, RenderSystemScript::Plugin);

namespace RenderSystemScript {

Plugin::Plugin() {
}

Plugin::~Plugin() {
}

void Plugin::Open() {
	Trace("Loaded render system script loader 1.0.");
	_streamer.RegisterDictionary();
	ApplicationContext::Instance().Subscribe(EngineApplicationContext::EVENT_RENDERMANAGER_CREATED,
			&RenderManagerCreated, this);
	ApplicationContext::Instance().Subscribe(EngineApplicationContext::EVENT_RENDERMANAGER_PRE_CLOSE,
				&RenderManagerDestroyed, this);
}

void Plugin::Close() {
	Trace("Unloading render system script loader 1.0.");
	_streamer.UnregisterDictionary();
	ApplicationContext::Instance().Unsubscribe(EngineApplicationContext::EVENT_RENDERMANAGER_CREATED,
			&RenderManagerCreated);
	ApplicationContext::Instance().Unsubscribe(EngineApplicationContext::EVENT_RENDERMANAGER_PRE_CLOSE,
				&RenderManagerDestroyed);

}

void Plugin::LicenseRenewed() {
}

bool Plugin::LicenseExpired() {
	return true;
}

void Plugin::Dispose() {
}

void Plugin::RenderManagerCreated(void* thisPointer) {
	if (RenderManager::InstancePtr())
		RenderManager::Instance().AddRenderStreamer("RSCRIPT",
				&(reinterpret_cast<Plugin*>(thisPointer)->_streamer));
}

void Plugin::RenderManagerDestroyed(void*) {
	if (RenderManager::InstancePtr())
		RenderManager::Instance().RemoveRenderStreamer("RSCRIPT");
}

} /* namespace ShaderCompiler */
