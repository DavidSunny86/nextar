/*
 * ExtensionsGL.cpp
 *
 *  Created on: 17-Aug-2013
 *      Author: obhi
 */

#include "RenderOpenGL.h"
#include "ExtensionsGL.h"
#include "RenderDriverGL.h"

namespace RenderOpenGL {

#define DECL_FUNC_GL_NAME(W) gl##W
#define DECL_FUNC_GL_STRING(W) NEX_MAKE_TEXT(gl##W)
#define DECL_EXT_NAME(Pref,W)  NEX_MAKE_TEXT(Pref##W)

    ExtensionsGL::ExtensionsGL() {
    	GLE_ARB_multisample = false;
    	GLE_ARB_depth_clamp = false;
    	GLE_version = GLV_INVALID;

    	GlBlendEquationSeparate = 0;
    	GlBlendFuncSeparate = 0;


#define DECL_COND_START_VERSION(what)
#define DECL_COND_END_VERSION(what)
#define DECL_COND_START_EXT(pref, what)
#define DECL_COND_END_EXT(what) \
		NEX_TOKEN_PASTE(GLE_, what) = false;
#define DECL_EXTENSION(funcptr, fucnname) \
    	DECL_FUNC_NAME(fucnname) = 0;

#include "ExtListGL.h"

#undef DECL_COND_START_VERSION
#undef DECL_COND_END_VERSION
#undef DECL_COND_START_EXT
#undef DECL_COND_END_EXT
#undef DECL_EXTENSION

    }

    bool ExtensionsGL::IsSupported(const char* ext_name, const char* p) {
        int ext_name_len;
        ext_name_len = std::strlen(ext_name);

        while (*p) {
            int n = std::strcspn(p, " ");
            if ((ext_name_len == n) && (std::strncmp(ext_name, p, n) == 0)) {
                return true;
            }
            p += (n + 1);
        }
        return false;
    }


    void ExtensionsGL::RequiredExtensionNotFound(const char* what) {
        Error(String("Required extensions not found: ") + what);
        NEX_THROW_FatalError(EXCEPT_APPINIT_FAILED);
    }

    void ExtensionsGL::_ReadyExtensions(uint16 major, uint16 minor) {

        VersionGL minimum = RenderDriverGL::ExtractVersion(major, minor);
        const char* version = (const char*) glGetString(GL_VERSION);
        // extract version info
        GLE_version = RenderDriverGL::ExtractVersion(version);
        if(GLE_version == GLV_INVALID || GLE_version < minimum) {
            Error("OpenGL version too old!");
            NEX_THROW_FatalError(EXCEPT_APPINIT_FAILED);
        }

        // get extensions
        const char* extensions = (const char*) glGetString(GL_EXTENSIONS);

#define DECL_COND_START_VERSION(what) if(GLE_version >= what) { \
		bool failed = false;

#define DECL_COND_END_VERSION(what)         \
		if(failed)                              \
			RequiredExtensionNotFound("GL Version: " NEX_MAKE_TEXT(what));\
		} else { \
			Trace("OpenGL extension: " NEX_MAKE_TEXT(what) " not supported!");\
		}

#define DECL_COND_START_EXT(pref, what)   \
		if (IsSupported(DECL_EXT_NAME(pref, what), extensions)) {   \
			bool failed = false;\


#define DECL_COND_END_EXT(what) \
		if(failed)                  \
			RequiredExtensionNotFound("GL Extension: " NEX_MAKE_TEXT(what));\
		} else { \
			Trace("OpenGL extension: " NEX_MAKE_TEXT(what) " not supported!");\
		}


#define DECL_EXTENSION(a, b) \
		DECL_FUNC_NAME(b) = (DECL_FUNC_TYPE(a)) \
		RenderDriverGL::GetExtension(DECL_FUNC_GL_STRING(b)); \
		if(!DECL_FUNC_NAME(b)) {                              \
			failed = true;                                    \
		}

#include "ExtListGL.h"

#undef DECL_COND_START_VERSION
#undef DECL_COND_END_VERSION
#undef DECL_COND_START_EXT
#undef DECL_COND_END_EXT
#undef DECL_EXTENSION

    }
}



