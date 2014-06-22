#ifndef NEXTAR_IMPLEMENT_APPLICATION
#define NEXTAR_IMPLEMENT_APPLICATION

#include <OsDefs.h>

#ifdef NEX_MSVC
#define NEX_ARG_C	__argc
#define NEX_ARG_V	__argv
#else
extern int _argc;
extern char** _argv;
#define NEX_ARG_C	_argc
#define NEX_ARG_V	_argv
#endif

#define NEX_IMPLEMENT_APP()			\
	NEX_IMPLEMENT_PLATFORM_APP()

namespace nextar {
}

#endif // NEXTAR_IMPLEMENT_APPLICATION