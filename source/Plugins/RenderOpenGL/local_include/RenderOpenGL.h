#ifndef _RENDEROPENGL_H_
#define _RENDEROPENGL_H_

#include <RenderEngineHeaders.h>
#include <ConfigGL.h>
#include <IncludeGL.h>

using namespace ::nextar;

namespace RenderOpenGL {

class RenderContext_Base_GL;
class RenderDriverGL;

class UniformBufferGL;
class GpuBufferViewGL;
class TextureViewGL;
class PassViewGL;
class RenderBufferViewGL;
class RenderTextureViewGL;
class MultiRenderTargetViewGL;

struct RasterStateGL;
struct BlendStateGL;
struct DepthStencilStateGL;
struct VertexSemanticGL;
struct VertexAttribGL;
struct PixelFormatGl;


/** These are the supported GLE Versions,
 *  the system may cap to a version and set a minimum
 *  version requirement.
 */
enum VersionGL {
	GLV_INVALID,
	GLV_1_3,
	GLV_1_4,
	GLV_1_5,
	GLV_2_0,
	GLV_2_1,
	GLV_3_0,
	GLV_3_1,
	GLV_3_2,
	GLV_3_3,
	GLV_4_0,
	GLV_4_1,
	GLV_4_2,
	GLV_4_3,
	GLV_4_4,
	GLV_4_5,

	GLV_MAX_VERSION
};

#if defined (NEX_DEBUG)
class DebugGL {
	NEX_LOG_HELPER(DebugGL);
	static bool s_Enabled;
public:
	static void Enable() {
		s_Enabled = true;
	}
	static void Disable() {
		s_Enabled = false;
	}
	static void CheckGL(const char* function, long line);
};
#endif

struct PixelFormatGl {
	bool isCompressed : 1;
	bool isDepthSencil : 1;
	uint8 pixelSize;
	PixelFormat textureFormat;
	GLenum attachmentType;
	GLenum internalFormat;
	GLenum sourceFormat;
	GLenum dataType;
};
}

#endif //_RENDEROPENGL_H_