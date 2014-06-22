#include <NexEngine.h>
#include <Viewport.h>
#include <Camera.h>
#include <RenderManager.h>

namespace nextar {

Viewport::Viewport(Camera* cam, RenderTarget* rt, float x, float y, float width,
		float height, int32 priority, uint32 flags, const String& optName,
		Viewport* nextVp) {
	this->next = nextVp;
	this->clearStencil = 0;
	this->clearDepth = 1;
	this->clearMask = 0;
	this->camera = cam;
	this->renderTarget = rt;
	this->topleftX = x;
	this->topleftY = y;
	this->width = width;
	this->height = height;
	this->flags = flags;
	this->priority = priority;
	this->name = optName;
}

Viewport::~Viewport() {
}

void Viewport::AddCallback(ViewportCallback* cbk) {
	callbacks.push_back(cbk);
}

void Viewport::RemoveCallback(ViewportCallback* cbk) {
	BestErase(callbacks, std::find(callbacks.begin(), callbacks.end(), cbk));
}

void Viewport::FirePreupdate() {
	ViewportCallbackList::iterator en = callbacks.end();
	for (ViewportCallbackList::iterator i = callbacks.begin(); i != en; ++i)
		(*i)->PreUpdate(this);
}

void Viewport::FirePostupdate() {
	ViewportCallbackList::iterator en = callbacks.end();
	for (ViewportCallbackList::iterator i = callbacks.begin(); i != en; ++i)
		(*i)->PostUpdate(this);
}

void Viewport::PushPrimitives(uint32 frameNumber) {

	traversal.camera = camera;
	traversal.frameNumber = frameNumber;
	//traversal.visibleBoundsInfo = &(camera->GetBoundsInfo());
	//traversal.visibilityMask = camera->GetVisibilityMask();
	traversal.visibilitySet = &visibleSet;
	camera->Visit(traversal);
	// if we registered any offscreen viewports, lets call push on those
	for (auto &s : offscreen) {
		s->PushPrimitives(frameNumber);
	}
}

void Viewport::CommitPrimitives(RenderContext* renderCtx, uint32 frameNumber) {

	// if we had offscreen vp lets commit them first
	for (auto &s : offscreen) {
		s->CommitPrimitives(renderCtx, frameNumber);
	}

	visibleSet.SortSet();

	commitContext.renderContext = renderCtx;
	commitContext.viewport = this;
	commitContext.frameNumber = frameNumber;
	commitContext.visibiles = &visibleSet;

	for (auto &r : renderSystems) {
		r->Commit(commitContext);
	}
}

void Viewport::Render(RenderContext* renderCtx, uint32 frameNumber) {
	RenderManager& rm = RenderManager::Instance();
	visibleSet.Prepare();
	PushPrimitives(frameNumber);
	FirePreupdate();
	CommitPrimitives(renderCtx, frameNumber);
	FirePostupdate();
}
}
