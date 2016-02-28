#include <NexRenderEngine.h>
#include <VideoMode.h>
#include <BaseRenderContext.h>
#include <RenderWindow.h>
#include <RenderWindowImpl.h>
#include <NexThread.h>

namespace nextar {

BaseRenderContext::BaseRenderContext(RenderDriver* _driver) :
		driver(_driver), originalVideoMode(-1), currentVideoMode(-1) {
}

BaseRenderContext::~BaseRenderContext(void) {
}

void BaseRenderContext::DestroyAllWindows() {
	std::for_each(renderWindows.begin(), renderWindows.end(), [] (RenderWindow* value) {
		value->Destroy();
		NEX_DELETE(value);
	});
	renderWindows.clear();
}

void BaseRenderContext::Close() {
	PreCloseImpl();
	CloseImpl();
	PostCloseImpl();
}

void BaseRenderContext::PreCloseImpl() {
	graphicsWindowList.clear();
	DestroyAllWindows();
}

void BaseRenderContext::PostCloseImpl() {
}

void BaseRenderContext::Create(
		const RenderDriver::ContextCreationParams& contextCreationParams) {
	CreateImpl(contextCreationParams);
	// default window present?
	if (contextCreationParams.createDefaultWindow) {
		RenderWindow* win = CreateRenderWindow(
				contextCreationParams.defaultWindowWidth,
				contextCreationParams.defaultWindowHeight,
				contextCreationParams.fullScreen,
				&contextCreationParams.extraParams);
	}
	threadId = NEX_THREAD_ID();
}

RenderWindow* BaseRenderContext::CreateRenderWindow(uint32 width, uint32 height,
bool fullscreen, const NameValueMap* params) {
	RenderWindow* graphicsWindow = CreateRenderWindowImpl();
	if (graphicsWindow) {
		graphicsWindow->Create(width, height, fullscreen, params);
		PostWindowCreation(graphicsWindow);
		NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
		RenderTargetPtr rt = Assign(
				static_cast<RenderWindowImpl*>(graphicsWindow->GetImpl()));
		graphicsWindowList.push_back(rt);
		renderWindows.push_back(graphicsWindow);
	}
	return graphicsWindow;
}

void BaseRenderContext::DestroyedRenderWindow(RenderWindow* which) {
	if (which) {
		PostWindowDestruction(which);
		NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
		RenderTargetPtr rt = Bind(
				static_cast<RenderWindowImpl*>(which->GetImpl()));
		graphicsWindowList.remove(rt);
	}
}

RenderTargetList& BaseRenderContext::GetRenderTargetList() {
	return graphicsWindowList;
}

uint32 BaseRenderContext::GetVideoModeIndex(const VideoMode& current) const {
	for (size_t i = 0; i < videoModes.size(); ++i) {
		if (current == videoModes[i]) {
			return (uint32) i;
		}
	}
	return currentVideoMode;
}

void BaseRenderContext::SetVideoMode(uint32 vidMod) {
	if (currentVideoMode != vidMod) {
		if (vidMod == (uint32)-1)
			vidMod = originalVideoMode;
		SetVideoModeImpl(videoModes[vidMod]);
		currentVideoMode = vidMod;
	}
}

void BaseRenderContext::BeginFrame(const FrameTimer& frameTimer) {
	frameStats.frameID = frameTimer.GetFrameNumber();
	frameStats.renderTargetsUsed = 0;
	frameStats.polygonCount = 0;
	frameStats.shaderSwitches = 0;
	frameStats.textureFetches = 0;
	// todo while profiling this will be fetched and populated
	frameStats.elapsedMilliSeconds = 0;
}

void BaseRenderContext::EndFrame() {
}

void BaseRenderContext::BeginRender(RenderInfo* ri, ClearFlags cf) {
	if (currentTarget != ri->rt) {
		currentTarget = ri->rt;
		SetCurrentTarget(ri->rt);
		frameStats.renderTargetsUsed++;
	}
	if (Test(cf)) {
		Clear(ri->info, cf);
	}
}

void BaseRenderContext::EndRender() {
	// SetCurrentTarget(0);
	// currentTarget = nullptr;
}

void BaseRenderContext::UpdateObject(ContextObject* object,
		uint32 updateMsg,
		ContextObject::ContextParamPtr updateParam) {
	NEX_ASSERT(threadId == std::this_thread::get_id());
#if NEX_MULTIGPU_BUILD
#error Not implemented
#else
	ContextObject::View* view = reinterpret_cast<ContextObject::View*>(
			object->GetContextID());
	view->Update(this, updateMsg, updateParam);
#endif
}

FrameStats BaseRenderContext::GetFrameStats() {
	return frameStats;
}

}
