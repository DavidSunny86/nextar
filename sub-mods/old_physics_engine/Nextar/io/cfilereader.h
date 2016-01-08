//@ Abhishek Dey
//@ 1/7/2007

#ifndef __FILEREADER_H__
#define __FILEREADER_H__

#include <ifilereader.h>
#include <string/cstr.h>
#include <stdio.h>

namespace nextar
{

class cFileReader : public iFileReader
{
	cStr		mkFileName; // The full file name
	s32			miNameIdx;	// HACK: the index indicates where file name begins in the filename string
	FILE*		mpkFile;	// File ptr
	s32			miSize;		// File size

public:

	//@ Important: The input string or the filename should be OS specific
	//@ but the filename is internally converted to nextar format.
	cFileReader( const tchar* name );
	virtual ~cFileReader();

	_n_inline bool isValid() { return mpkFile?true:false; }

	virtual bool isEndOfFile();
	virtual bool readData( void* ptrBuffer, u32 numByte );
	virtual void seek( s32 nbyte, bool fromtop );
	virtual s32 tell();
	virtual s32 getCurrentSize();
	virtual const tchar* getFileFullName();
	virtual const tchar* getFileName();
};

}


#endif //__FILEREADER_H__