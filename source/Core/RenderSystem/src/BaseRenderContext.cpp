
#include <RenderSystem.h>
#include <VideoMode.h>
#include <BaseRenderContext.h>
#include <RenderWindow.h>
#include <RenderWindowImpl.h>
#include <NexThread.h>

namespace nextar {

	BaseRenderContext::BaseRenderContext(RenderDriver* _driver) : driver(_driver),
			originalVideoMode(-1), currentVideoMode(-1) {
	}

	BaseRenderContext::~BaseRenderContext(void) {
	}

	void BaseRenderContext::Create(const RenderDriver::ContextCreationParams& contextCreationParams) {
		CreateImpl(contextCreationParams);
				// default window present?
		if (contextCreationParams.createDefaultWindow) {
			RenderWindow* win = CreateRenderWindow(contextCreationParams.defaultWindowWidth,
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
			RenderTargetPtr rt = Bind(static_cast<RenderWindowImpl*>(graphicsWindow->GetImpl()));
			graphicsWindowList.push_back(rt);
		}
		return graphicsWindow;
	}

	void BaseRenderContext::DestroyedRenderWindow(RenderWindow* which) {
		if (which) {
			PostWindowDestruction(which);
			NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
			RenderTargetPtr rt = Bind(static_cast<RenderWindowImpl*>(which->GetImpl()));
			graphicsWindowList.remove(rt);
		}
	}

	RenderTargetPtr BaseRenderContext::GetRenderTarget(uint32 i) {
		return graphicsWindowList[i];
	}

	RenderTargetList& BaseRenderContext::GetRenderTargetList() {
		return graphicsWindowList;
	}

	uint32 BaseRenderContext::GetVideoModeIndex(const VideoMode& current) const {
		for (size_t i = 0; i < videoModes.size(); ++i) {
			if (current == videoModes[i]) {
				return (uint32)i;
			}
		}
		return currentVideoMode;
	}

	void BaseRenderContext::SetVideoMode(uint32 vidMod) {
		if (currentVideoMode != vidMod) {
			SetVideoModeImpl(videoModes[vidMod]);
			currentVideoMode = vidMod;
		}
	}

	void BaseRenderContext::BeginFrame(uint32 frame, uint32 t) {
		frameStats.frameID = frame;
		frameStats.renderTargetsUsed = 0;
		frameStats.polygonCount = 0;
		frameStats.shaderSwitches = 0;
		frameStats.textureFetches = 0;
		frameStats.startTime = t;
	}

	void BaseRenderContext::EndFrame(uint32 t) {
		frameStats.endTime = t;
	}

	void BaseRenderContext::BeginRender(RenderInfo* ri) {
		if (currentTarget != ri->rt) {
			currentTarget = ri->rt;
			SetCurrentTarget(ri->rt);
			frameStats.renderTargetsUsed++;
		}
		if (ri->clearFlags) {
			Clear(ri->clearColor, ri->clearDepth, ri->clearStencil, ri->clearFlags);
		}
	}

	void BaseRenderContext::EndRender() {
		// todo Set a flag marking the current render target is pending a reset?
	}

	void BaseRenderContext::SwitchShader(uint16 pass, CommitContext& ctx, ShaderAsset* shader) {
		ctx.shader = shader;
		Pass* passPtr = shader->GetPass(pass);
		if (activePass != passPtr) {
			SetActivePass(ctx.pass = activePass = passPtr);
		}
	}



}
