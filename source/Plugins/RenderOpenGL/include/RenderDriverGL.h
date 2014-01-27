
#pragma once

#include "RenderOpenGL.h"

namespace RenderOpenGL {

	class RenderDriverGL : public nextar::BaseRenderDriver {

	public:
		RenderDriverGL(int32 gpuIndex);
		virtual ~RenderDriverGL();

		inline int GetGpuIndex() {
			return gpuIndex;
		}

		static VersionGL ExtractVersion(const char* version);
		static VersionGL ExtractVersion(uint16 major, uint16 minor);
		/* create a new context */
		virtual RenderContextPtr CreateContextImpl(const ContextCreationParams& ctxParams);

		/* */
		static RenderDriverGL* CreateDriverGL(int32 gpuIndex);
		static void* GetExtension(const char* name);

		/* */
		virtual void ConfigureImpl(const NameValueMap& params) {
			/* @todo configure driver */
		}

	protected:

		virtual RenderContextGL* CreateContextImpl() = 0;

		int32 gpuIndex;
	};
}
