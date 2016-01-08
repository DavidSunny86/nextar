/*VARCHECK*/
/*FILE MARK
 DOC ORIGINAL NAME:	cLog.h
******************************************************************************************************
 AUTHOR:	Abhishek Dey
 CREATED:	1/1/2007		AT:		11:07:33 PM
END
******************************************************************************************************
*/
#ifndef __XLOG_H__
#define __XLOG_H__

#include <fstream>
#include <xtypes.h>
#include <cstr.h>

namespace nextar
{

enum LOGERRORLEVEL
{
	LL_INFO,
	LL_WARNING,
	LL_FATAL
};

//@ cLog does not support UNICODE text 
class X_Api cLog
{

public:
	
	cLog( const char*, bool temp );
	~cLog();

	//@ printf style print
	void  print( LOGERRORLEVEL, const char* str, ... );
	//@ outputs the date and time.
	void  printDateAndTime();

private:

	std::ofstream	mkFileStream;
	bool			mbTemp;
	bool			mbOpen;
};

}


#endif //__XLOG_H__