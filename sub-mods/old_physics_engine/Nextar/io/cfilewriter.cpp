

#include <cfilewriter.h>
#include <isystem.h>
#include <xio.h>

#ifndef X_WIN32
#	include <stdlib.h>
#endif

using namespace nextar;

/* ctor */
cFileWriter::cFileWriter( const tchar* name, bool bApp )
{
	RefObjectInit( cFileWriter );
	mpkFile = 0;
	_tfopen_s(&mpkFile, name, bApp ? X_Text("ab") : X_Text("wb"));

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

cFileWriter::~cFileWriter()
{
	if(mpkFile) fclose(mpkFile);
}

s32 cFileWriter::getCurrentSize()
{
	return tell();
}

const tchar* cFileWriter::getFileName()
{
	const tchar* sN = mkFileName.c_str();
	return &sN[miNameIdx];
}

const tchar* cFileWriter::getFileFullName()
{
	return mkFileName.c_str();
}

bool cFileWriter::writeData( void* ptrBuffer, u32 numByte )
{
	if(mpkFile) return fwrite(ptrBuffer,1,numByte,mpkFile)==numByte;
	return false;
}

void cFileWriter::seek( s32 nbytes, bool ft )
{
	if(mpkFile) fseek(mpkFile,nbytes,ft?SEEK_SET:SEEK_CUR);
}

s32 cFileWriter::tell()
{
	if(mpkFile) return ftell(mpkFile);
	return -1;
}
