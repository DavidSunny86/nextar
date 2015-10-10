/*
 * ExtensionsGL.h
 *
 *  Created on: 17-Aug-2013
 *      Author: obhi
 */

#ifndef EXTENSIONSGL_H_
#define EXTENSIONSGL_H_

#include <NexBase.h>
#include <RenderOpenGL.h>

namespace RenderOpenGL {

#define DECL_FUNC_TYPE(W) PFNGL##W##PROC
#define DECL_FUNC_NAME(W) Gl##W
#define DECL_FUNC_NAME_X(W) _Gl##W
#define DECL_FUNC_TABLE_NAME(maj, min) CoreFuncTableVersion##maj##min

#define DECL_COND_START_VERSION(maj, min, prevmaj, prevmin) \
	class DECL_FUNC_TABLE_NAME(maj, min) { \
		NEX_LOG_HELPER(CoreFuncTableVersion##maj##min);
	protected:
#define DECL_CORE_EXTENSION(funcptr, fucnname) \
	    	DECL_FUNC_TYPE(funcptr) DECL_FUNC_NAME(fucnname);
#define DECL_COND_END_VERSION(maj, min) \
				void InitializeFunctionPointers(); \
				DECL_FUNC_TABLE_NAME(maj, min)();	\
	};

#define DECL_COND_START_EXT(pref, what)
#define DECL_EXTENSION(funcptr, fucnname)
#define DECL_COND_END_EXT(what)

#include <ExtListGL.h>

#undef DECL_COND_START_VERSION
#undef DECL_COND_END_VERSION
#undef DECL_COND_START_EXT
#undef DECL_COND_END_EXT
#undef DECL_EXTENSION
#undef DECL_CORE_EXTENSION

class ExtensionHelper {
public:
	static bool IsSupported(const char* ext_name, const char* p);
	static void RequiredExtensionNotFound(const char* what);
}

class BaseExtensionsGL {
	NEX_LOG_HELPER(BaseExtensionsGL);
protected:

	void InitializeFunctionPointers();
	BaseExtensionsGL();

#define DECL_COND_START_VERSION(maj, min, prevmaj, prevmin)
#define DECL_CORE_EXTENSION(funcptr, fucnname)
#define DECL_COND_END_VERSION(maj, min)
#define DECL_COND_START_EXT(pref, what) \
			bool NEX_TOKEN_PASTE(GLE_, what);
#define DECL_EXTENSION(funcptr, fucnname) \
			    	DECL_FUNC_TYPE(funcptr) DECL_FUNC_NAME(fucnname);
#define DECL_COND_END_EXT(what)

#include <ExtListGL.h>

#undef DECL_COND_START_VERSION
#undef DECL_COND_END_VERSION
#undef DECL_COND_START_EXT
#undef DECL_COND_END_EXT
#undef DECL_EXTENSION
#undef DECL_CORE_EXTENSION

	PFNGLGETSTRINGIPROC glGetStringi;
};

} /* namespace RenderOpenGL */
#endif /* EXTENSIONSGL_H_ */
