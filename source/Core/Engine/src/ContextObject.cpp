
#include <NexEngine.h>
#include <ContextObject.h>
#include <RenderManager.h>

namespace nextar {

	/***************************************
	 * GpuObjectTraits
	 ***************************************/
	ContextObject::ContextObject() {
		NotifyCreated();
	}

	ContextObject::~ContextObject() {
		NotifyDestroyed();
	}

	void ContextObject::NotifyCreated() {
		RenderManager::Instance().NotifyObjectCreated(this);
	}

	void ContextObject::NotifyDestroyed() {
		RenderManager::Instance().NotifyObjectDestroyed(this);
	}

	void ContextObject::NotifyUpdated(ContextObject::UpdateParamPtr params) {
		RenderManager::Instance().NotifyObjectUpdated(this, params);
	}
}
