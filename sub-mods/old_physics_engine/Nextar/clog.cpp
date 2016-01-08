/*FILE MARK
 DOC ORIGINAL NAME:	cLog.cpp
******************************************************************************************************
 AUTHOR:	Abhishek Dey
 CREATED:	1/1/2007		AT:		11:16:59 PM
END
******************************************************************************************************
*/


#include <clog.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>

namespace nextar
{

/************************************
./ cLog::cLog	
./ Decscription: ctor
************************************/
cLog::cLog( const char* file, bool tmp ) : mbTemp(tmp),mbOpen(0)
{
	mkFileStream.open(file);
	if( !mkFileStream.is_open() )
	{
		mkFileStream.open("run.log");
		/* if this fails, we have got a problem here */
	}
	if(mkFileStream.is_open()) mbOpen = true;
}

cLog::~cLog()
{
	if( mbOpen ) mkFileStream.close(); 
}

/************************************
./ cLog::print	
./ Decscription:	
./ Return type:	void 
./ param:  LOGERRORLEVEL	[]:		
./ param: const char* str	[]:		
./ param: ...	[]:		
************************************/
void cLog::print( LOGERRORLEVEL l, const char* str, ... )
{
	if( mbOpen )
	{
		static char buff[1024] = {0};
		/* CHECKOUT */
		va_list	val;
#ifdef UNIX
		va_start( val )
#else
		va_start( val, str );
#endif
		vsnprintf_s( buff, 1023, _TRUNCATE , str, val );
		mkFileStream << (l == LL_INFO ? "Info: " : (l == LL_WARNING ? "Warning: " : "Fatal Error: ")) << buff << "\n";
		ReportBugIf( l == LL_FATAL, buff, "unknown" ); 

		va_end( val );
	}
}

void cLog::printDateAndTime()
{
	if( mbOpen )
	{
		s8 tmpbuf1[128], tmpbuf2[128];	
		_tzset();
		_strtime_s( tmpbuf1 );
		_strdate_s( tmpbuf2 );
		mkFileStream << "Time: " << tmpbuf1 << "\tDate: " << tmpbuf2	<< "\n";
	}
}

}
