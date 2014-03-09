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

	class ExtensionsGL {
		NEX_LOG_HELPER(ExtensionsGL);
	public:

		ExtensionsGL();


#define DECL_FUNC_TYPE(W) PFNGL##W##PROC
#define DECL_FUNC_NAME(W) Gl##W
#define DECL_FUNC_NAME_X(W) _Gl##W
#define DECL_COND_START_VERSION(what)
#define DECL_COND_END_VERSION(what)
#define DECL_COND_START_EXT(pref, what) \
		bool NEX_TOKEN_PASTE(GLE_, what);

#define DECL_COND_END_EXT(what)

#define DECL_EXTENSION(funcptr, fucnname) \
    	DECL_FUNC_TYPE(funcptr) DECL_FUNC_NAME(fucnname);

#include <ExtListGL.h>

#undef DECL_COND_START_VERSION
#undef DECL_COND_END_VERSION
#undef DECL_COND_START_EXT
#undef DECL_COND_END_EXT
#undef DECL_EXTENSION

	protected:

		void _ReadyExtensions(uint16 major, uint16 minior);
		static bool IsSupported(const char* ext_name, const char* p);
		static void RequiredExtensionNotFound(const char* what);

		VersionGL GLE_version;

	};
} /* namespace RenderOpenGL */
#endif /* EXTENSIONSGL_H_ */
