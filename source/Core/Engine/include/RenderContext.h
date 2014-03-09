
#pragma once

#include <NexBase.h>
#include <ContextObject.h>
#include <StreamData.h>

namespace nextar {

	struct FrameStats {
		uint32 frameID;
		uint32 polygonCount;
		uint32 shaderSwitches;
		uint32 renderTargetsUsed;
		uint32 textureFetches;
	};

	struct RenderInfo {
		RenderTarget* rt;
		uint16 clearFlags;
		Color clearColor;
		float clearDepth;
		uint16 clearStencil;
	};

	enum class ClearFlags {
		CLEAR_COLOR = 1 << 0,
		CLEAR_DEPTH = 1 << 1,
		CLEAR_STENCIL = 1 << 2,
		CLEAR_ALL = CLEAR_COLOR|CLEAR_DEPTH|CLEAR_STENCIL
	};

	typedef list<RenderTargetPtr>::type RenderTargetList;

	class RenderContext : public Referenced<RenderContext, AllocGraphics> {
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
		virtual RenderWindowPtr CreateRenderWindow(uint32 width, uint32 height,
								  bool fullscreen, NameValueMap* params) = 0;

		/* Called by the window to indicate it was destroyed */
		virtual void DestroyedRenderWindow(RenderWindowPtr&) = 0;
		/* Get the i'th window created. */
		virtual RenderWindowPtr GetRenderTarget(uint32 i) = 0;
		virtual RenderTargetList& GetRenderTargetList() = 0;
		virtual void SetVideoMode(uint32 videoModeIndex) = 0;
		virtual void BeginFrame(uint32 frame) = 0;
		virtual void EndFrame() = 0;
		virtual FrameStats GetFrameStats() = 0;

		virtual void SwitchShader(uint16 pass, ShaderAsset*) = 0;
		virtual void Draw(StreamData*) = 0;

		virtual void RegisterObject(ContextObject*) = 0;
		virtual void UnregisterObject(ContextObject*) = 0;
		virtual void UpdateObject(ContextObject*, ContextObject::UpdateParamPtr) = 0;
	};

}
