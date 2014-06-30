#ifndef _RENDEROPENGL_H_
#define _RENDEROPENGL_H_

#include <RenderEngineHeaders.h>
#include <ConfigGL.h>
#include <IncludeGL.h>

using namespace ::nextar;

namespace RenderOpenGL {

class RenderContextGL;
class RenderDriverGL;

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

	GLV_MAX_VERSION
};

class DebugGL {
	NEX_LOG_HELPER(DebugGL)
	;
public:
	static void CheckGL();
};
}

#endif //_RENDEROPENGL_H_