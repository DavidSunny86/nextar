/*
 * RenderManager.h
 *
 *  Created on: 15-Aug-2013
 *      Author: obhi
 */

#ifndef RENDERMANAGER_H_
#define RENDERMANAGER_H_

#include <NexBase.h>
#include <NexBase.h>
#include <RenderDriver.h>
#include <NameValueMap.h>
#include <GpuProgram.h>
#include <RenderSystem.h>
#include <RenderConstants.h>

namespace nextar {

	class _NexEngineAPI RenderManager : public Singleton<RenderManager>,
		public AllocGraphics {
		NEX_LOG_HELPER(RenderManager);
	public:

		enum ShaderProgramLanguage {
			SPP_GLSL,
			SPP_HLSL
		};

		enum Flags {
			RM_HAS_SINGLE_CONTEXT = 1 << 0,
			RM_SHARED_DATA_ACROSS_COTEXTS = 1 << 1,
		};

		struct DriverCreationParams {
			bool createDefaultContext;
			uint32 gpuIndex;
			RenderDriver::ContextCreationParams defaultContextParams;
		};

		struct RenderSettings {
			bool syncPresent;
			/* capturing render targets will not return the immediate data
			 * but the previous frame's data.  */
			bool asyncCapture;
		};

		RenderManager();
		virtual ~RenderManager();

		inline const RenderSettings& GetRenderSettings() const {
			return renderSettings;
		}

		/* */
		virtual void Configure(const Config&);
		virtual RenderDriverPtr AsyncCreateDriver(DriverCreationParams&);
		virtual void Close();
		virtual void RegisterRenderContext(RenderContextPtr&);

		virtual void NotifyObjectCreated(ContextObject*);
		virtual void NotifyObjectDestroyed(ContextObject*);
		virtual void NotifyObjectUpdated(ContextObject*, ContextObject::UpdateParamPtr);

		/** This function might alter the actual layer indexes which are referred to
		 * by materials. So, this should be called at the start of engine initialization and
		 * not modified later. */
		virtual void AddRenderQueue(const String& name, uint16 priority, RenderQueueFlags flags);
		virtual const RenderQueueDescList& GetRenderQueueInfo() const;

		virtual RenderSystemList& GetRenderSystems();
		/* Implement */
		virtual ShaderProgramLanguage GetProgramLanguage() = 0;
		virtual String GetInfo() = 0;
		/* Create objects */
		virtual GpuProgram* CreateProgram(GpuProgram::Type type) = 0; //todo
		virtual RenderTarget* CreateRenderTarget(bool textureTarget = true) = 0;
		virtual MultiRenderTarget* CreateMultiRenderTarget() = 0;
		virtual Pass* CreatePass(StringID name) = 0;

		/* Render a single frame for a specific window, should be called
		   from the thread this window was created in */
		virtual void RenderFrame(uint32 frameNumber);
		
	protected:

		virtual void RenderAllTargets(RenderContext* rc, uint32 frame, bool callPresent);
		virtual void PresentSwapChains(RenderContext* rc);

		typedef vector<RenderDriverPtr>::type RenderDriverList;
		typedef vector<RenderContextPtr>::type RenderContextList;
		typedef vector<ContextObject*>::type GpuObjectTraitsList;
		typedef map<String, uint16>::type RenderLayerNameMap;
				

		virtual RenderDriverPtr CreateDriverImpl(DriverCreationParams&) = 0;
		virtual void ConfigureImpl(const NameValueMap&) = 0;
		virtual void CloseImpl() = 0;

		NEX_THREAD_MUTEX(accessLock);
		/* Global render options */
		RenderSettings renderSettings;
		/* Registered render systems */
		RenderSystemList renderSystems;
		/* Render layers */
		RenderQueueDescList renderQueues;
		/* List of objects needs readying */
		GpuObjectTraitsList objectsToReady;
		bool usingMultiGpuSetup;
		/* The first registered context is always the primary context, other
		 * contexts are present if usingMultiGpuSetup is true */
		RenderContextPtr primaryContext;
		RenderContextList activeContexts;
		RenderDriverList renderDrivers;
	};
} /* namespace nextar */
#endif /* RENDERMANAGER_H_ */
