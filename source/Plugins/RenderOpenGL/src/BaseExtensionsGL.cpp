/*
 * ExtensionsGL.cpp
 *
 *  Created on: 17-Aug-2013
 *      Author: obhi
 */

#include <RenderOpenGL.h>
#include <BaseExtensionsGL.h>
#include <RenderDriverGL.h>
#include <RenderContext_Base_GL.h>

namespace RenderOpenGL {

#define DECL_FUNC_GL_NAME(W) gl##W
#define DECL_FUNC_GL_STRING(W) NEX_MAKE_TEXT(gl##W)
#define DECL_EXT_NAME(Pref,W)  NEX_MAKE_TEXT(Pref##W)

#define DECL_COND_START_VERSION(maj, min, pmaj, pmin) \
	DECL_FUNC_TABLE_NAME(maj, min)::DECL_FUNC_TABLE_NAME(maj, min)() {
#define DECL_COND_END_VERSION(maj, min) }
#define DECL_CORE_EXTENSION(funcptr, fucnname) \
    	DECL_FUNC_NAME(fucnname) = 0;
#define DECL_COND_START_EXT(pref, what)
#define DECL_COND_END_EXT(what)
#define DECL_EXTENSION(funcptr, fucnname)

#include <ExtListGL.h>

#undef DECL_COND_START_VERSION
#undef DECL_COND_END_VERSION
#undef DECL_COND_START_EXT
#undef DECL_COND_END_EXT
#undef DECL_EXTENSION
#undef DECL_CORE_EXTENSION


bool ExtensionHelper::IsSupported(const char* ext_name, const char* p) {
	int ext_name_len;
	ext_name_len = (uint32)std::strlen(ext_name);

	while (*p) {
		int n = (int)std::strcspn(p, " ");
		if ((ext_name_len == n) && (std::strncmp(ext_name, p, n) == 0)) {
			return true;
		}
		p += (n + 1);
	}
	return false;
}

void ExtensionHelper::RequiredExtensionNotFound(const char* what) {
	Error(String("Required extensions not found: ") + what);
	NEX_THROW_FatalError(EXCEPT_APPINIT_FAILED);
}

BaseExtensionsGL::BaseExtensionsGL() {
	glGetStringi = nullptr;

#define DECL_COND_START_VERSION(maj, min, pmaj, pmin)
#define DECL_CORE_EXTENSION(funcptr, fucnname)
#define DECL_COND_END_VERSION(maj, min)
#define DECL_COND_START_EXT(pref, what)
#define DECL_COND_END_EXT(what) NEX_TOKEN_PASTE(GLE_, what) = false;
#define DECL_EXTENSION(funcptr, fucnname) DECL_FUNC_NAME(fucnname) = nullptr;
#include <ExtListGL.h>
#undef DECL_COND_START_VERSION
#undef DECL_COND_END_VERSION
#undef DECL_COND_START_EXT
#undef DECL_COND_END_EXT
#undef DECL_EXTENSION
#undef DECL_CORE_EXTENSION

}

void BaseExtensionsGL::InitializeFunctionPointers() {

	glGetStringi = (PFNGLGETSTRINGIPROC)RenderContext_Base_GL::GetExtension("glGetStringi");
	if (!glGetStringi) {
		Error("Unable to retrieve extensions as glGetStringi was not found!");
		return;
	}

	//********************************************************************
	// Determine supported extensions
	//********************************************************************
#define DECL_COND_START_VERSION(ma, mi, pma, pmi)
#define DECL_COND_END_VERSION(ma, mi)
#define DECL_CORE_EXTENSION(a, b)
#define DECL_COND_START_EXT(pref, what)   if(!std::strcmp(DECL_EXT_NAME(pref, what), extensions)) NEX_TOKEN_PASTE(GLE_, what) = true;
#define DECL_COND_END_EXT(what)
#define DECL_EXTENSION(a, b)

	GLint n;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	GL_CHECK();
	for (GLint i = 0; i < n; ++i) {
		const char* extensions = (const char*)glGetStringi(GL_EXTENSIONS, i);
		GL_CHECK();
#include <ExtListGL.h>

	}

#undef DECL_COND_START_VERSION
#undef DECL_COND_END_VERSION
#undef DECL_COND_START_EXT
#undef DECL_COND_END_EXT
#undef DECL_EXTENSION
#undef DECL_CORE_EXTENSION
	//********************************************************************
	// Retrieve function pointers
	//********************************************************************
#define DECL_COND_START_VERSION(ma, mi, pma, pmi)
#define DECL_COND_END_VERSION(ma, mi)
#define DECL_CORE_EXTENSION(a, b)

#define DECL_COND_START_EXT(pref, what)   \
		if (NEX_TOKEN_PASTE(GLE_, what)) {   \
			bool failed = false;\


#define DECL_COND_END_EXT(what) \
		if(failed)                  \
			ExtensionHelper::RequiredExtensionNotFound("GL Extension: " NEX_MAKE_TEXT(what));\
				} else { \
			Trace("OpenGL extension: " NEX_MAKE_TEXT(what) " not supported!");\
				}

#define DECL_EXTENSION(a, b) \
		DECL_FUNC_NAME(b) = (DECL_FUNC_TYPE(a)) \
		RenderContext_Base_GL::GetExtension(DECL_FUNC_GL_STRING(b)); \
		if(!DECL_FUNC_NAME(b)) {                              \
			failed = true;                                    \
				}

#include <ExtListGL.h>

#undef DECL_COND_START_VERSION
#undef DECL_COND_END_VERSION
#undef DECL_COND_START_EXT
#undef DECL_COND_END_EXT
#undef DECL_EXTENSION
#undef DECL_CORE_EXTENSION

}

#define DECL_COND_START_EXT(pref, what)
#define DECL_COND_END_EXT(what)
#define DECL_EXTENSION(a, b)
#define DECL_COND_START_VERSION(ma, mi, pma, pmi)  \
	void DECL_FUNC_TABLE_NAME(ma, mi)::InitializeFunctionPointers() { \
		bool failed = false;
#define DECL_COND_END_VERSION(ma, mi)         \
		if(failed)                              \
			ExtensionHelper::RequiredExtensionNotFound("Unable to find functions: " NEX_MAKE_TEXT(ma) "." NEX_MAKE_TEXT(mi));\
		}
#define DECL_CORE_EXTENSION(a, b) \
		DECL_FUNC_NAME(b) = (DECL_FUNC_TYPE(a)) \
		RenderContext_Base_GL::GetExtension(DECL_FUNC_GL_STRING(b)); \
		if(!DECL_FUNC_NAME(b)) {                              \
			failed = true;                                    \
				}

#include <ExtListGL.h>

#undef DECL_COND_START_VERSION
#undef DECL_COND_END_VERSION
#undef DECL_COND_START_EXT
#undef DECL_COND_END_EXT
#undef DECL_EXTENSION
#undef DECL_CORE_EXTENSION

}
