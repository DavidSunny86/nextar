
#pragma once

#include "NexSys.h"
#include "RenderContext.h"
#include "VideoMode.h"
#include "ContextObject.h"

namespace nextar {

	class _NexExport BaseRenderContext : public RenderContext {

		NEX_LOG_HELPER(BaseRenderContext);

	public:
		BaseRenderContext(RenderDriver*);
		virtual ~BaseRenderContext();

		virtual void Create(const RenderDriver::ContextCreationParams& ctxParams);
		virtual RenderWindowPtr CreateRenderWindow(uint32 width, uint32 height,
								  bool fullscreen, const NameValueMap* params);
		virtual void DestroyedRenderWindow(RenderWindowPtr);

		virtual void SetVideoMode(uint32 videoModeIndex);

		/** @todo Context objects */
		virtual void RegisterObject(ContextObject*);
		virtual void UnregisterObject(ContextObject*);
		virtual void UpdateObject(ContextObject*, ContextObject::UpdateParamPtr);

		/* implement */
		virtual void CreateImpl(const RenderDriver::ContextCreationParams& ctxParams) = 0;
		virtual RenderWindowPtr CreateRenderWindowImpl() = 0;
		virtual void SetVideoModeImpl(const VideoMode& videoMode) = 0;

		/* helpers */
		uint32 GetVideoModeIndex(const VideoMode& vm) const;
		inline const VideoMode& GetCurrentMode() const {
			return videoModes[currentVideoMode];
		}

	protected:

		/* implement */
		virtual void PostWindowCreation(RenderWindow* window) = 0;
		virtual void PostWindowDestruction(RenderWindow* window) = 0;
		
		typedef list<RenderWindowPtr>::type GraphicsWindowList;
		NEX_THREAD_MUTEX(accessLock);

		uint32 originalVideoMode;
		uint32 currentVideoMode;

		RenderDriver* driver;
		VideoModeList videoModes;
		GraphicsWindowList graphicsWindowList;
	};
}
