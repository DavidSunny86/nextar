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
#include <RenderPass.h>
#include <RenderConstants.h>
#include <RenderSystem.h>
#include <MemoryPool.h>

namespace nextar {

class _NexEngineAPI RenderManager: public Singleton<RenderManager>,
		public PluginService,
		public AllocGeneral {

	NEX_DECLARE_SERVICE_INTERFACE(RenderManager);
	NEX_LOG_HELPER(RenderManager);
public:

	/** indexing dependency, needs to be consequtive starting from 0 */
	enum ShaderLanguage : uint8 {
		SPP_GLSL,
		SPP_HLSL,
		SPP_COUNT,
		SSP_ALL = SPP_COUNT,
		SPP_UNKNOWN,
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

	static bool QueryService(const Config& config);

	virtual void Destroy() override;
	inline const RenderSettings& GetRenderSettings() const {
		return renderSettings;
	}

	inline const RenderQueueDescList& GetRenderQueueInfo() const {
		return renderQueues;
	}

	void AddRenderPassFactory(StringID name, RenderPass::CreateInstance ci) {
		renderSystemFactories[name] = ci;
	}

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

	inline StreamData& GetFullScreenQuad() {
		if (!fullScreenQuad.vertices.count)
			GenerateStreamDataForQuad();
		return fullScreenQuad;
	}

	void AddRenderStreamer(const String& name, RenderSystem::Streamer*);
	void RemoveRenderStreamer(const String& name);
	RenderSystem::Streamer* GetRenderStreamer(const String& name);

	RenderPass* CreateRenderPass(StringID name);

	virtual RenderSystemPtr CreateRenderSystem(const String& configName, Size viewDimensions);
	virtual RenderSystemPtr CreateRenderSystemImpl(const String& configName, Size viewDimensions) = 0;

	RenderSystemPtr GetActiveRenderSystem() const {
		return activeRenderSystem;
	}

	const StringUtils::WordList& GetGlobalShaderOptions() const {
		return shaderOptions;
	}

	ShaderUnitRenderInfo* AllocMaterialRenderInfo() {
		return reinterpret_cast<ShaderUnitRenderInfo*>(_materialRenderInfoPool.Alloc());
	}

	void FreeMaterialRenderInfo(ShaderUnitRenderInfo* info) {
		_materialRenderInfoPool.Free(info);
	}

protected:

	static void DestroyResources(void* renderSystem);
	static void CreateResources(void* renderSystem);

	virtual void DestroyResources();
	virtual void CreateResources();
		
	void GenerateStreamDataForQuad();
		
	typedef map<String, RenderSystem::Streamer*>::type RenderSystemStreamerMap;
	typedef map<StringID, RenderPass::CreateInstance>::type RenderPassFactoryMap;

	virtual RenderDriverPtr CreateDriverImpl(DriverCreationParams&) = 0;
	virtual void ConfigureImpl(const NameValueMap&) = 0;

	NEX_THREAD_MUTEX(accessLock);

	typedef MemoryPool<MEMCAT_MATH_CORE, NullMutex> PoolType;
	/* bool loadDefaultTexture; */

	PoolType _materialRenderInfoPool;
	/* */
	StreamData fullScreenQuad;
	/* Global render options */
	RenderSettings renderSettings;
	/* Render layers */
	RenderQueueDescList renderQueues;	
	// Default texture
	TextureAssetPtr defaultTexture;
	// active render system
	RenderSystemPtr activeRenderSystem;
	// Render pass factory map
	RenderPassFactoryMap renderSystemFactories;
	// Render streamers
	RenderSystemStreamerMap renderSystemStreamers;
	// Global shader options
	StringUtils::WordList shaderOptions;

};

}
/* namespace nextar */
#endif /* RENDERMANAGER_H_ */
