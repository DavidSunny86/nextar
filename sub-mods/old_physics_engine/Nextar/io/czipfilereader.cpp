//@ Abhishek Dey
//@ Sept 22 2007

#include "czipfilereader.h"
#include <stdio.h>

using namespace nextar;

cZipFileReader::cZipFileReader(const tchar *sName,ZZIP_DIR* pkDir) : mkName(sName), mpkFile(0), muFlags(0)
{
	mpkFile = zzip_file_open (pkDir, sName, ZZIP_CASELESS|ZZIP_ONLYZIP);
	if(mpkFile) muFlags |= IsValid;
	zzip_seek(mpkFile,0,SEEK_END);
	miSize = zzip_tell(mpkFile);
	zzip_seek(mpkFile,0,SEEK_SET);

}

cZipFileReader::~cZipFileReader()
{
	if(IsValid && mpkFile)
	{
		zzip_file_close(mpkFile);
	}
}

s32 cZipFileReader::getCurrentSize()
{
	return miSize;
}

bool cZipFileReader::isEndOfFile()
{
	return muFlags & IsEndOfFile;
}

bool cZipFileReader::readData(void *ptrBuffer, u32 numByte)
{
	s32 n = zzip_read(mpkFile, (char*)ptrBuffer, numByte);
	if(n<=0) muFlags |= IsEndOfFile;
	if(n==numByte)	return true;
	return false;
}

const tchar* cZipFileReader::getFileFullName()
{
	return mkName; // TODO: change to dir:/path/archieve/file.xxx format
}

const tchar* cZipFileReader::getFileName()
{
	return mkName;
}

s32 cZipFileReader::tell()
{
	return (s32)zzip_tell(mpkFile);
}

void cZipFileReader::seek(s32 nbyte, bool fromtop)
{
	zzip_seek(mpkFile,(zzip_off_t)nbyte,fromtop?SEEK_SET:SEEK_CUR);
}