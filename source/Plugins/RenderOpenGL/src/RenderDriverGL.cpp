#include <RenderOpenGL.h>
#include <RenderDriverGL.h>
#include <RenderContext_AllHeaders.h>

namespace RenderOpenGL {

RenderDriverGL::RenderDriverGL(int32 _gpuIndex) :
		gpuIndex(_gpuIndex) {
}

RenderDriverGL::~RenderDriverGL() {
	renderContexts.clear();
}

VersionGL RenderDriverGL::ExtractVersion(const char* version) {
	InStringStream strm(version);
	uint32 maj, min;
	char dot;
	strm >> maj >> dot >> min;
	return ExtractVersion((uint16) maj, (uint16) min);
}

RenderDriverGL* RenderDriverGL::CreateDriverGL(int32 gpuIndex) {
	return NEX_NEW(RenderDriverGL(gpuIndex));
}

VersionGL RenderDriverGL::ExtractVersion(uint16 maj, uint16 min) {
	switch (maj) {
	case 1:
		switch (min) {
		case 3:
			return RenderOpenGL::GLV_1_3;
		case 4:
			return RenderOpenGL::GLV_1_4;
		case 5:
			return RenderOpenGL::GLV_1_5;
		}
		break;
	case 2:
		switch (min) {
		case 0:
			return RenderOpenGL::GLV_2_0;
		case 1:
			return RenderOpenGL::GLV_2_1;
		}
		break;
	case 3:
		switch (min) {
		case 0:
			return RenderOpenGL::GLV_3_0;
		case 1:
			return RenderOpenGL::GLV_3_1;
		case 2:
			return RenderOpenGL::GLV_3_2;
		case 3:
			return RenderOpenGL::GLV_3_3;
		}
		break;
	case 4:
		switch (min) {
		case 0:
			return RenderOpenGL::GLV_4_0;
		case 1:
			return RenderOpenGL::GLV_4_1;
		case 2:
			return RenderOpenGL::GLV_4_2;
		case 3:
			return RenderOpenGL::GLV_4_3;
		case 4:
			return RenderOpenGL::GLV_4_4;
		case 5:
			return RenderOpenGL::GLV_4_5;
		}
		break;
	}
	if (maj < 1 || (maj == 1 && min < 4))
		return GLV_INVALID;
	else
		return GLV_MAX_VERSION;
}

RenderContextPtr RenderDriverGL::CreateContextImpl(
		const RenderDriver::ContextCreationParams& params) {
	//@todo Determine context version and create opengl context.
	RenderContext_Base_GL* rcGL = nullptr;
	VersionGL version = ExtractVersion(params.reqOpenGLVersionMajor, params.reqOpenGLVersionMinor);
	switch(version) {
	default:
	case RenderOpenGL::GLV_4_0:
		rcGL = NEX_NEW(RenderContext_4_0_GL(this)); break;
	case RenderOpenGL::GLV_4_1:
		rcGL = NEX_NEW(RenderContext_4_1_GL(this)); break;
	case RenderOpenGL::GLV_4_2:
		rcGL = NEX_NEW(RenderContext_4_2_GL(this)); break;
	case RenderOpenGL::GLV_4_3:
		rcGL = NEX_NEW(RenderContext_4_3_GL(this)); break;
	case RenderOpenGL::GLV_4_4:
		rcGL = NEX_NEW(RenderContext_4_4_GL(this)); break;
	case RenderOpenGL::GLV_4_5:
		rcGL = NEX_NEW(RenderContext_4_5_GL(this)); break;
	}
	return Assign(rcGL);
}
}
