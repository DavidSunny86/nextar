#pragma once

#include <NexBase.h>
#include <Config.h>

namespace nextar {

/**
 * One instance per gpu
 **/
class _NexEngineAPI RenderDriver: public Referenced<RenderDriver, AllocGeneral> {
public:

	/* Driver info */
	struct DriverInfo {
		/* true if full screen anti aliasing is supported */
		bool fsaa;
		/* true if multi sampling is supported */
		bool multiSampling;
		/* Render API */
		String renderApi;
		/* Card manufacturer  */
		String manufacturer;
		/* Version string */
		String version;
		/* Max texture size */
		uint32 maxTexSize;
		/* Max clip planes*/
		uint32 maxClipPlanes;
		/* Max texture stages */
		uint32 maxTextureStages;
		// GL_MAX_ARRAY_TEXTURE_LAYERS_EXT
		uint32 maxTextureArrayLayers;
	};

	struct ContextCreationParams {
		bool deferredContext; // @todo deferredContexts are not registered right now
		bool createDefaultWindow;
		bool fullScreen;
		bool stereo;

		uint16 monitorIndex;
		uint16 multiSamples;
		uint16 depthBits;
		uint16 stencilBits;

		uint16 defaultWindowWidth;
		uint16 defaultWindowHeight;

		uint16 reqOpenGLVersionMajor;
		uint16 reqOpenGLVersionMinor;

		/* if we are sharing data then the index of the context we are sharing data with;
		 * set to negetive value if not used. */
		int16 sharedContextIndex;
		NameValueMap extraParams;
	};

	/** Configure */
	virtual void Configure(const Config& config) = 0;
	/** Close driver */
	virtual void Close() = 0;

	/** Return driver description string */
	virtual DriverInfo& GetDriverInfo() = 0;
	/** Context */
	virtual RenderContextPtr AsyncCreateContext(const ContextCreationParams& ctxParams) = 0;
	virtual RenderContextPtr AsyncGetContext(uint16 index) = 0;

	RenderDriver(void);
	virtual ~RenderDriver(void);
};
}
