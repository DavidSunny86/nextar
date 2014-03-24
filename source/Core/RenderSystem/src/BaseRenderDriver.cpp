
#include <RenderSystem.h>
#include <BaseRenderDriver.h>
#include <Config.h>
#include <RenderContext.h>

namespace nextar {

	BaseRenderDriver::BaseRenderDriver(void) : statusFlags(0) {
	}

	BaseRenderDriver::~BaseRenderDriver(void) {
	}

	RenderContextPtr BaseRenderDriver::AsyncCreateContext(const ContextCreationParams& ctxParams) {
		RenderContextPtr renderCtxPtr = CreateContextImpl(ctxParams);
		if (renderCtxPtr) {
			renderCtxPtr->Create(ctxParams);
			NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
			renderContexts.push_back(renderCtxPtr);
		}
		return renderCtxPtr;
	}

	RenderContextPtr BaseRenderDriver::AsyncGetContext(uint16 index) {
		NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
		return renderContexts[index];
	}

	void BaseRenderDriver::Configure(const Config& config) {
		const NameValueMap& renderConfigParams = config.GetSectionMap("Render");
		ConfigureImpl(renderConfigParams);
	}

	void BaseRenderDriver::Close() {
		NEX_THREAD_LOCK_GUARD_MUTEX(accessLock);
		renderContexts.clear();
	}
}
