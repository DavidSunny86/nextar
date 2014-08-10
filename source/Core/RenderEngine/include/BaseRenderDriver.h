#pragma once

#include <NexRenderEngine.h>
#include <RenderDriver.h>

namespace nextar {

class _NexRenderAPI BaseRenderDriver: public RenderDriver {

public:
	BaseRenderDriver();
	virtual ~BaseRenderDriver();

	virtual DriverInfo& GetDriverInfo() {
		return driverInfo;
	}

	virtual void Configure(const Config&);
	virtual void Close();

	virtual RenderContextPtr AsyncCreateContext(
			const ContextCreationParams& ctxParams);
	virtual RenderContextPtr AsyncGetContext(uint16 index);

	/* Implement */
	virtual RenderContextPtr CreateContextImpl(
			const ContextCreationParams& ctxParams) = 0;
	virtual void ConfigureImpl(const NameValueMap& params) = 0;

protected:

	typedef vector<RenderContextPtr>::type RenderContextList;

	NEX_THREAD_MUTEX(accessLock);
	uint32 statusFlags;

	DriverInfo driverInfo;
	RenderContextList renderContexts;
};

}
