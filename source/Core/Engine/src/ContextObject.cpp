#include <NexEngine.h>
#include <ContextObject.h>
#include <RenderManager.h>

namespace nextar {

/***************************************
 * ContextObject
 ***************************************/

ContextObject::ContextObject() : contextId(0) {
}

ContextObject::ContextObject(ContextObject&& other) :
	contextId(std::move(other.contextId)) {
	other.contextId = 0;
}

ContextObject::ContextObject(Type type, uint32 hint) :
		contextId(RenderManager::Instance().RequestObjectCreate(type, hint)) {
}

ContextObject::~ContextObject() {
	RenderManager::Instance().RequestObjectDestroy(contextId);
}

void ContextObject::RequestUpdate(uint32 updateMsg,
		ContextObject::ContextParamPtr params) {
	if (contextId)
		RenderManager::Instance().RequestObjectUpdate(this, updateMsg, params);
	else {
		Warn("The object is not constructed!.");
	}
}

ContextObject& ContextObject::operator = (ContextObject&& other) {
	if (contextId)
		RenderManager::Instance().RequestObjectDestroy(contextId);
	contextId = std::move(other.contextId);
	other.contextId = 0;
	return *this;
}

}
