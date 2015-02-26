#pragma once

#include <NexRenderEngine.h>
#include <RenderContext.h>
#include <VideoMode.h>
#include <ContextObject.h>

namespace nextar {

class _NexRenderAPI BaseRenderContext: public RenderContext {

	NEX_LOG_HELPER(BaseRenderContext)
	;

public:
	BaseRenderContext(RenderDriver*);
	virtual ~BaseRenderContext();

	virtual void Create(const RenderDriver::ContextCreationParams& ctxParams);
	virtual RenderWindow* CreateRenderWindow(uint32 width, uint32 height,	bool fullscreen, const NameValueMap* params);
	virtual void DestroyedRenderWindow(RenderWindow*);
	virtual RenderTargetList& GetRenderTargetList();
	virtual void SetVideoMode(uint32 videoModeIndex);
	virtual void BeginRender(RenderInfo*);
	virtual void EndRender();
	virtual void Close();

	/** todo Context objects */
	virtual ContextID RegisterObject(ContextObject::Type type, uint32 hint) = 0;
	virtual void UnregisterObject(ContextID) = 0;
	virtual void UpdateObject(ContextObject*, uint32 updateMsg,
			ContextObject::ContextParamPtr);

	/* implement */
	virtual void CreateImpl(
			const RenderDriver::ContextCreationParams& ctxParams) = 0;
	virtual RenderWindow* CreateRenderWindowImpl() = 0;
	virtual void SetVideoModeImpl(const VideoMode& videoMode) = 0;
	virtual void Draw(StreamData*, CommitContext&) override = 0;
	virtual void SetCurrentTarget(RenderTarget*) = 0;
	virtual void Clear(Color& c, float depth, uint16 stencil,
			ClearFlags flags) = 0;

	/* helpers */
	uint32 GetVideoModeIndex(const VideoMode& vm) const;
	inline const VideoMode& GetCurrentMode() const {
		return videoModes[currentVideoMode];
	}

	virtual void BeginFrame(const FrameTimer& frameTimer) override;
	virtual void EndFrame() override;
	virtual FrameStats GetFrameStats();

	void DestroyAllWindows();
protected:

	inline RenderTarget* GetCurrentTarget() {
		return currentTarget;
	}

	virtual void PreCloseImpl();
	virtual void PostCloseImpl();
	virtual void CloseImpl() = 0;
	/* implement */
	virtual void PostWindowCreation(RenderWindow* window) = 0;
	virtual void PostWindowDestruction(RenderWindow* window) = 0;

	NEX_THREAD_MUTEX(accessLock);

	typedef list<RenderWindow*>::type RenderWindowList;

	RenderWindowList renderWindows;
	uint32 originalVideoMode;
	uint32 currentVideoMode;
	/* thread this context is bound to, 0 if none */
	std::thread::id threadId;

	RenderTarget* currentTarget;
	FrameStats frameStats;
	RenderDriver* driver;
	VideoModeList videoModes;
	RenderTargetList graphicsWindowList;
};
}
