//@ Obi De
//@ Jan 1 08
#ifndef NEXTAR_WIN32_OSDEFS_H
#define NEXTAR_WIN32_OSDEFS_H

//---------------------------------------
//@ Implement application
//---------------------------------------
#ifdef NEX_WINDOWS

#include "IncWnds.h"

#define DYN_LIB_TYPE HMODULE
#define NEX_DYN_LOAD(name)          LoadLibrary(name)
#define NEX_DYN_ERR()               "<unknown>"
#define NEX_DYN_FREE(libptr)        FreeLibrary(libptr)
#define NEX_DYN_LOAD_SYMBOL(module,name)   GetProcAddress(module,name)
#define NEX_MODULE_NAME(name)			   name + ".dll"

#ifndef NEX_CONSOLE_APP_ENTRY
#define NEX_PLATFORM_APP()                              				\
int NextarMain(int argc,char* argv[]);                                      \
/* do include windows.h */                                                  \
int APIENTRY WinMain(HINSTANCE hInstance,                                   \
		   HINSTANCE hPrevInstance,                                      \
		   LPSTR    lpCmdLine,                                           \
		   int       nCmdShow)                                           \
{                                                                                \
	(void)hInstance;                                                         \
	(void)hPrevInstance;                                                     \
	(void)lpCmdLine;                                                         \
	(void)nCmdShow;                                                          \
	NEX_PLATFORM_APP_BEGIN();												 \
	int ret;																\
	NEX_PLATFORM_APP_END(NEX_ARG_C, NEX_ARG_V, ret);												\
	return ret;                                  							\
}

#else
#define NEX_PLATFORM_APP()                              				\
int NextarMain(int argc,char* argv[]);                                      \
int main(int argc,char* argv[]) {										\
	NEX_PLATFORM_APP_BEGIN();												 \
	int ret;																\
	NEX_PLATFORM_APP_END(argc, argv, ret);												\
	return ret;                                  							\
}

#endif

#define NEX_PLUGIN_OS_DEFINITION                                                 \
extern "C" BOOL WINAPI                                                           \
DllMain (HINSTANCE , DWORD , LPVOID )                                            \
{                                                                                \
	return TRUE;                                                             \
}

#if defined(NEX_MSVC)
#define FOPEN_W(file, path, mode)	_wfopen_s(&file, (const wchar_t*)(path), (mode));
#else
#define FOPEN_W(file, path, mode) file = _wfopen((const wchar_t*)(path), (mode));
#endif

static_assert(sizeof(wchar_t)==sizeof(char16_t), "wchar_t does not match char16_t size!, need to change code");

#endif
#endif //NEXTAR_WIN32_OSDEFS_H
