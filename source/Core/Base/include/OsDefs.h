/* 
 * File:   OsDefs.h
 * Author: obhi
 *
 * Created on January 19, 2011, 8:08 PM
 */

#ifndef NEXTAR_OSDEFS_H
#define	NEXTAR_OSDEFS_H

#include <NexBase.h>

#define NEX_PLATFORM_APP_BEGIN()	NEXTAR_START_LEAK_TRACKING()
#define NEX_PLATFORM_APP_END(argC, argV, nexMainRet) \
	nexMainRet = NextarMain(argC,argV); \
	NEXTAR_STOP_LEAK_TRACKING(); \
	NEXTAR_UNK_TRACKED_OBJS(std::cerr); \
	NEXTAR_LEAK_CHECKS(std::cerr);


#if defined(NEX_WINDOWS)
# include "platform/win32/Win32Defs.h"
#elif defined(NEX_LINUX)
# include "platform/linux/LinuxDefs.h"
#else
#error OS/Dynamic Lib not defined
#endif


#define NEX_IMPLEMENT_PLATFORM_APP()	\
	NEX_PLATFORM_APP();

namespace nextar {

	class _NexBaseExport Platform {
	public:
		/* returns the location where the current exe resides */
		static String GetModuleLocation();
	};
}

#endif	/* NEXTAR_OSDEFS_H */

