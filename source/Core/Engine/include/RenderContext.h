#ifndef RENDERCONTEXT_H_
#define RENDERCONTEXT_H_

#include <NexEngine.h>
#include <RenderDriver.h>
#include <Pass.h>
#include <CommitContext.h>
#include <RenderPass.h>
#include <RenderTarget.h>

namespace nextar {

struct FrameStats {
	uint32 frameID;
	uint32 polygonCount;
	uint32 shaderSwitches;
	uint32 renderTargetsUsed;
	uint32 textureFetches;
	uint32 elapsedMilliSeconds;
};

class StreamData;
class _NexEngineAPI RenderContext: public Referenced<RenderContext,
		AllocGeneral> {
public:
	RenderContext(void);
	virtual ~RenderContext(void);

	/* Called by the render driver when a context is created */
	virtual void Create(
			const RenderDriver::ContextCreationParams& ctxParams) = 0;
	virtual void Close() = 0;
	/**
	 * Common Properties that can be passed in params are:
	 * WindowLeft:int, WindowTop:int, WindowTitle:string, IsMainWindow:bool,
	 * VideoMode:class VideoMode, ExitOnClose:bool
	 * */
	virtual RenderWindow* CreateRenderWindow(uint32 width, uint32 height,
			bool fullscreen, const NameValueMap* params) = 0;

	/* Called by the window to indicate it was destroyed */
	virtual void DestroyedRenderWindow(RenderWindow*) = 0;
	virtual RenderTargetList& GetRenderTargetList() = 0;
	virtual void SetVideoMode(uint32 videoModeIndex) = 0;
	virtual void BeginFrame(const FrameTimer& frameTimer) = 0;
	virtual void EndFrame() = 0;
	virtual FrameStats GetFrameStats() = 0;
	virtual void BeginRender(RenderInfo*, ClearFlags cf) = 0;
	virtual void EndRender() = 0;
	virtual void Copy(RenderTarget* src, FrameBuffer srcFb, RenderTarget* dest, FrameBuffer destFb) = 0;

	virtual void SwitchPass(CommitContext&, Pass::View*) = 0;
	virtual void Draw(StreamData*, CommitContext&) = 0;
	virtual ContextID RegisterObject(ContextObject::Type type, uint32 hint) = 0;
	virtual void UnregisterObject(ContextID) = 0;
	virtual void UpdateObject(ContextObject*, uint32 updateMsg,
			ContextObject::ContextParamPtr) = 0;
	virtual void SetViewport(const CommitContext& c) = 0;

#if NEX_MULTIGPU_BUILD
	virtual ContextObject::View* GetView(const ContextObject*) = 0;
#else
	inline ContextObject::View* GetView(const ContextObject* object) {
		NEX_ASSERT(object);
		return reinterpret_cast<ContextObject::View*>(object->GetContextID());
	}
#endif
};

}

#endif //RENDERCONTEXT_H_
