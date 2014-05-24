
#pragma once

#include <NexBase.h>
#include <ContextObject.h>
#include <StreamData.h>
#include <RenderDriver.h>

namespace nextar {

	struct FrameStats {
		uint32 frameID;
		uint32 polygonCount;
		uint32 shaderSwitches;
		uint32 renderTargetsUsed;
		uint32 textureFetches;
		uint32 elapsedMilliSeconds;
	};

	enum class ClearFlags {
		CLEAR_COLOR = 1 << 0,
		CLEAR_DEPTH = 1 << 1,
		CLEAR_STENCIL = 1 << 2,
		CLEAR_ALL = CLEAR_COLOR|CLEAR_DEPTH|CLEAR_STENCIL
	};

	NEX_ENUM_FLAGS(ClearFlags, uint16);

	struct RenderInfo {
		RenderTarget* rt;
		ClearFlags clearFlags;
		Color clearColor;
		float clearDepth;
		uint16 clearStencil;
	};

	typedef list<RenderTargetPtr>::type RenderTargetList;

	class _NexEngineAPI RenderContext : public Referenced<RenderContext, AllocGraphics> {
	public:
		RenderContext(void);
		virtual ~RenderContext(void);

		/* Called by the render driver when a context is created */
		virtual void Create(const RenderDriver::ContextCreationParams& ctxParams) = 0;
		/**
		 * Common Properties that can be passed in params are:
		 * WindowLeft:int, WindowTop:int, WindowTitle:string, IsMainWindow:bool,
		 * VideoMode:class VideoMode, ExitOnClose:bool
		 * */
		virtual RenderWindow* CreateRenderWindow(uint32 width, uint32 height,
								  bool fullscreen, NameValueMap* params) = 0;

		/* Called by the window to indicate it was destroyed */
		virtual void DestroyedRenderWindow(RenderWindow*) = 0;
		virtual RenderTargetList& GetRenderTargetList() = 0;
		virtual void SetVideoMode(uint32 videoModeIndex) = 0;
		virtual void BeginFrame(uint32 frame) = 0;
		virtual void EndFrame() = 0;
		virtual FrameStats GetFrameStats() = 0;
		virtual void BeginRender(RenderInfo*) = 0;
		virtual void EndRender() = 0;

		virtual void SwitchShader(uint16 pass, CommitContext&, ShaderAsset*) = 0;
		virtual void Draw(StreamData*, CommitContext&) = 0;

		virtual void RegisterObject(ContextObject*) = 0;
		virtual void UnregisterObject(ContextObject*) = 0;
		virtual void UpdateObject(ContextObject*, ContextObject::UpdateParamPtr) = 0;
	};

}
