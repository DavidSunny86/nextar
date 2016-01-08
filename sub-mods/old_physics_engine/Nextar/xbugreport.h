/*VARCHECK*/
/*FILE MARK
 DOC ORIGINAL NAME:	XBugReport.h
******************************************************************************************************
 AUTHOR:	Abhishek Dey
 CREATED:	10/29/2006		AT:		12:47:47 PM
END
******************************************************************************************************
*/ 
#ifndef __XBUGREPORT_H__
#define __XBUGREPORT_H__

namespace nextar
{

X_Api void ReportBugDlg( const char* bugstr, const char* funcname, const char* modulename, const char* file, int line );

}

#endif //__XBUGREPORT_H__