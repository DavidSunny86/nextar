/*
 * RenderManager.h
 *
 *  Created on: 15-Aug-2013
 *      Author: obhi
 */

#ifndef RENDERMANAGER_H_
#define RENDERMANAGER_H_

#include <NexBase.h>
#include <RenderDriver.h>
#include <NameValueMap.h>
#include <RenderSystem.h>
#include <RenderConstants.h>

namespace nextar {

class _NexEngineAPI RenderManager: public Singleton<RenderManager>,
		public AllocGraphics {
	NEX_LOG_HELPER(RenderManager)
	;
public:

	/** indexing dependency, needs to be consequtive starting from 0 */
	enum ShaderLanguage : uint8 {
		SPP_GLSL,
		SPP_HLSL,
		SPP_COUNT,
		SPP_UNKNOWN = SPP_COUNT,
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

	inline const RenderSystemList& GetRenderSystems() const {
		return renderSystems;
	}

	inline const RenderQueueDescList& GetRenderQueueInfo() const {
		return renderQueues;
	}

	void AddRenderSystemFactory(const String& name, RenderSystem::CreateInstance ci) {
		renderSystemFactories[name] = ci;
	}

	void AddRenderSystem(const String& name, const Config& cfg);

	virtual void AddRenderSystem(RenderSystem* rs);
	virtual void RemoveRenderSystem(RenderSystem* rs);

	/* */
	virtual void Configure(const Config&);
	virtual RenderDriverPtr AsyncCreateDriver(DriverCreationParams&) = 0;
	virtual RenderDriverPtr AsyncGetDriver(uint32 index) = 0;
	virtual void Close() = 0;

	virtual void RegisterRenderContext(RenderContextPtr&) = 0;

	virtual ContextID RequestObjectCreate(ContextObject::Type, uint32 hint) = 0;
	virtual void RequestObjectDestroy(ContextID) = 0;
	virtual void RequestObjectUpdate(ContextObject*, uint32 updateMsg,
			ContextObject::ContextParamPtr) = 0;

	/** This function might alter the actual layer indexes which are referred to
	 * by materials. So, this should be called at the start of engine initialization and
	 * not modified later. */
	virtual void AddRenderQueue(const StringID name, uint16 priority,
			RenderQueueFlags flags);
	
	/* Implement */
	virtual ShaderLanguage GetProgramLanguage() = 0;
	virtual String GetInfo() = 0;

	/* Render a single frame for a specific window, should be called
	 from the thread this window was created in */
	virtual void RenderFrame(const FrameTimer& frameTimer) = 0;

	/* Returns the default texture */
	inline TextureAssetPtr GetDefaultTexture() const {
		return defaultTexture;
	}

protected:

	static void DestroyResources(void* renderSystem);
	static void CreateResources(void* renderSystem);

	void DestroyResources();
	void CreateResources();
		
	typedef map<String, RenderSystem::CreateInstance>::type RenderSystemFactoryMap;

	virtual RenderDriverPtr CreateDriverImpl(DriverCreationParams&) = 0;
	virtual void ConfigureImpl(const NameValueMap&) = 0;

	NEX_THREAD_MUTEX(accessLock);
	/* Global render options */
	RenderSettings renderSettings;
	/* Registered render systems */
	RenderSystemList renderSystems;
	/* Render layers */
	RenderQueueDescList renderQueues;	
	// Default texture
	TextureAssetPtr defaultTexture;
	// Render systems
	RenderSystemFactoryMap renderSystemFactories;
};

}
/* namespace nextar */
#endif /* RENDERMANAGER_H_ */
