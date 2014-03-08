#ifndef NEXTAR_INCLUDE_WINDOWS_H
#define NEXTAR_INCLUDE_WINDOWS_H
// this is the std method of including windows.h
// into any nextar project.
//
#ifdef NEXTAR_WIN_LEAN
#	define WIN32_LEAN_AND_MEAN
#endif

#ifdef NEXTAR_XP_BUILD
#	define _WIN32_WINNT	0x0601
#endif

#ifndef NOMINMAX
#       define NOMINMAX 1
#endif

#include <Windows.h>
#ifdef NEXTAR_NEED_WINX
#	include <windowsx.h>
#endif

#endif //NEXTAR_INCLUDE_WINDOWS_H
