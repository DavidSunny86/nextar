
#include <NexEngine.h>
#include <ContextObject.h>
#include <RenderManager.h>

namespace nextar {

	/***************************************
	 * GpuObjectTraits
	 ***************************************/
	ContextObject::ContextObject() {
		RequestCreate();
	}

	ContextObject::~ContextObject() {
		RequestDestroy();
	}

	void ContextObject::RequestCreate() {
		RenderManager::Instance().RequestObjectCreate(this);
	}

	void ContextObject::RequestDestroy() {
		RenderManager::Instance().RequestObjectDestroy(this);
	}

	void ContextObject::RequestUpdate(ContextObject::UpdateParamPtr params) {
		RenderManager::Instance().RequestObjectUpdate(this, params);
	}
}
