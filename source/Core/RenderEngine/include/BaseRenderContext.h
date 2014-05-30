
#pragma once

#include <RenderEngine.h>
#include <RenderContext.h>
#include <VideoMode.h>
#include <ContextObject.h>

namespace nextar {

	class _NexRenderAPI BaseRenderContext : public RenderContext {

		NEX_LOG_HELPER(BaseRenderContext);

	public:
		BaseRenderContext(RenderDriver*);
		virtual ~BaseRenderContext();

		virtual void Create(const RenderDriver::ContextCreationParams& ctxParams);
		virtual RenderWindow* CreateRenderWindow(uint32 width, uint32 height,
								  bool fullscreen, const NameValueMap* params);
		virtual void DestroyedRenderWindow(RenderWindow*);
		virtual RenderTargetList& GetRenderTargetList();
		virtual void SetVideoMode(uint32 videoModeIndex);
		virtual void BeginRender(RenderInfo*);
		virtual void EndRender();
		virtual void SwitchShader(uint16 pass, CommitContext&, ShaderAsset*);

		/** todo Context objects */
		virtual void RegisterObject(ContextObject*);
		virtual void UnregisterObject(ContextObject*);
		virtual void UpdateObject(ContextObject*, ContextObject::ContextParamPtr);

		/* implement */
		virtual void CreateImpl(const RenderDriver::ContextCreationParams& ctxParams) = 0;
		virtual RenderWindow* CreateRenderWindowImpl() = 0;
		virtual void SetVideoModeImpl(const VideoMode& videoMode) = 0;
		virtual void Draw(StreamData*, CommitContext&) override = 0;
		virtual void SetCurrentTarget(RenderTarget*) = 0;
		virtual void Clear(Color& c, float depth, uint16 stencil, ClearFlags flags) = 0;
		virtual void SetActivePass(Pass* pass) = 0;

		/* helpers */
		uint32 GetVideoModeIndex(const VideoMode& vm) const;
		inline const VideoMode& GetCurrentMode() const {
			return videoModes[currentVideoMode];
		}
		
		virtual void BeginFrame(uint32 frame);
		virtual void EndFrame();
		virtual FrameStats GetFrameStats();
	
	protected:

		/* implement */
		virtual void PostWindowCreation(RenderWindow* window) = 0;
		virtual void PostWindowDestruction(RenderWindow* window) = 0;
				
		NEX_THREAD_MUTEX(accessLock);

		typedef unordered_map<StringRef, ConstantBuffer*>::type ConstantBufferMap;

		uint32 originalVideoMode;
		uint32 currentVideoMode;
		/* thread this context is bound to, 0 if none */
		std::thread::id threadId;

		Pass* activePass;
		RenderTarget* currentTarget;
		FrameStats frameStats;
		RenderDriver* driver;
		VideoModeList videoModes;
		RenderTargetList graphicsWindowList;
	};
}
