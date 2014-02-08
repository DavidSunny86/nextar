
#include "NexHeaders.h"
#include "VideoMode.h"
#include "BaseRenderContext.h"
#include "RenderWindow.h"
#include "NexThread.h"

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
			RenderWindowPtr win = CreateRenderWindow(contextCreationParams.defaultWindowWidth,
					contextCreationParams.defaultWindowHeight,
					contextCreationParams.fullScreen,
					&contextCreationParams.extraParams);
		}
		threadId = NEX_THREAD_ID();
	}

	RenderWindowPtr BaseRenderContext::CreateRenderWindow(uint32 width, uint32 height,
								  bool fullscreen, const NameValueMap* params) {
		RenderWindowPtr graphicsWindow = CreateRenderWindowImpl();
		if (graphicsWindow) {
			graphicsWindow->Create(width, height, fullscreen, params);
			PostWindowCreation(graphicsWindow);
			NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
			graphicsWindowList.push_back(graphicsWindow);
		}
		return graphicsWindow;
	}

	void BaseRenderContext::DestroyedRenderWindow(RenderWindowPtr which) {
		if (which) {
			PostWindowDestruction(which);
			NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
			graphicsWindowList.remove(which);
		}
	}

	RenderWindowPtr BaseRenderContext::GetRenderTarget(uint32 i) {
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

	void BaseRenderContext::BeginFrame(uint32 frame) {
		frameStats.frameID = frame;
	}

	void BaseRenderContext::EndFrame() {
	}
}
