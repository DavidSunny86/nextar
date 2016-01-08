//
// File Name :: oBugReport.cpp
// Author    :: Abhishek Dey
// Created   :: 10/7/2006 5:05:17 PM
//-------------------------------------------------------------------------------------




#include <xtypes.h>
#include <xbugreport.h>
#include <crtdbg.h>

#ifdef X_WIN32
# include <windows.h>
#endif

namespace nextar
{

#ifdef _MSC_VER
/******************************************************************************
./ ReportBugDlg
******************************************************************************/
X_Api void ReportBugDlg( const char* bugstr, const char* funcname,  const char* modulename, const char* file, int line )
{
#ifdef X_WIN32
	OutputDebugString( bugstr );
#endif
	_CrtDbgReport( _CRT_ERROR, file, line, modulename, "Bug Found in Function: %s\nBug String: %s\n", funcname, bugstr );
}

#else

X_Api void ReportBugDlg( const char* bugstr, const char* funcname,  const char* modulename, const char* file, int line )
{
#ifdef X_WIN32
	OutputDebugString( bugstr );
#endif
	// how to do linux reporting??
}

#endif

}