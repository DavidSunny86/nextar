/* 
 * File:   OsDefs.h
 * Author: obhi
 *
 * Created on January 19, 2011, 8:08 PM
 */

#ifndef NEXTAR_OSDEFS_H
#define	NEXTAR_OSDEFS_H

#include <BaseTypes.h>

#define NEX_PLATFORM_APP_BEGIN()	
#define NEX_PLATFORM_APP_END(argC, argV, nexMainRet) \
	nexMainRet = NextarMain(argC,argV); \
	

#if defined(NEX_WINDOWS)
# include "Win32Defs.h"
#elif defined(NEX_LINUX)
# include "LinuxDefs.h"
#else
#error OS/Dynamic Lib not defined
#endif

#define NEX_IMPLEMENT_PLATFORM_APP()	\
	NEX_PLATFORM_APP();

namespace nextar {

class _NexBaseAPI Platform {
public:
	/* returns the location where the current exe resides */
	static String GetModuleLocation();
	static void OutputDebug(const char*);
};
}

#endif	/* NEXTAR_OSDEFS_H */

