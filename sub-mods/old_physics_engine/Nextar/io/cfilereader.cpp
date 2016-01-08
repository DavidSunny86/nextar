//@ Abhishek Dey
//@ 1/7/2007

#include <cfilereader.h>
#include <isystem.h>
#include <xio.h>

#ifndef X_WIN32
#	include <stdlib.h>
#endif

using namespace nextar;

cFileReader::cFileReader( const tchar* name ) : miSize(-1),miNameIdx(0)
{
	RefObjectInit( cFileReader );

	mpkFile = 0;
	if(_tfopen_s(&mpkFile, name, "rb")==0)
	{
		fseek(mpkFile,0,SEEK_END);
		miSize = ftell(mpkFile);
		fseek(mpkFile,0,SEEK_SET);

#ifdef X_WIN32
		tchar full[_MAX_PATH];
		if( _tfullpath( full, name, _MAX_PATH ) != NULL )
			mkFileName = full;
		else
			mkFileName = name;
#else
#ifdef UNICODE
#	error cannot resolve
#endif
		char full[260];
		realpath(name,full);
		/* TODO : find alternative to _fullpath */
		mkFileName = full;
#endif

		X_SharedPtr(iSystem)->getNextarFilePath(mkFileName);
		for(s32 i=0; i<mkFileName.strlen(); i++)
		{
			if(mkFileName[i] == N3DFileSeparator)
			{	miNameIdx = i+1; break; }
		}
	}
}

cFileReader::~cFileReader()
{
	if(mpkFile) fclose(mpkFile);
}

bool cFileReader::isEndOfFile()
{
	if(mpkFile)	return feof(mpkFile);
	return true;
}

s32 cFileReader::getCurrentSize()
{
	return miSize;
}

const tchar* cFileReader::getFileName()
{
	const tchar* sN = mkFileName.c_str();
	return &sN[miNameIdx];
}

const tchar* cFileReader::getFileFullName()
{
	return mkFileName.c_str();
}

bool cFileReader::readData( void* ptrBuffer, u32 numByte )
{
	if(mpkFile) return fread(ptrBuffer,1,numByte,mpkFile)==numByte;
	return false;
}

void cFileReader::seek( s32 nbytes, bool ft )
{
	if(mpkFile) fseek(mpkFile,nbytes,ft?SEEK_SET:SEEK_CUR);
}

s32 cFileReader::tell()
{
	if(mpkFile) return ftell(mpkFile);
	return -1;
}
