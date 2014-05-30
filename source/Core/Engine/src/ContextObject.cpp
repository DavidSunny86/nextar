
#include <NexEngine.h>
#include <ContextObject.h>
#include <RenderManager.h>

namespace nextar {

	/***************************************
	 * GpuObjectTraits
	 ***************************************/
	ContextObject::ContextObject(Type type) :
			contextId(RenderManager::Instance().RequestObjectCreate(type)) {
	}

	ContextObject::~ContextObject() {
		RenderManager::Instance().RequestObjectDestroy(contextId);
	}

	void ContextObject::RequestUpdate(uint32 updateMsg, ContextObject::ContextParamPtr params) {
		RenderManager::Instance().RequestObjectUpdate(this, updateMsg, params);
	}
}
