
#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <ifilewriter.h>
#include <cstr.h>
#include <stdio.h>

namespace nextar
{

class cFileWriter : public iFileWriter
{
private:

	cStr		mkFileName;
	s32			miNameIdx;
	FILE*		mpkFile;

public:

	//@ Important: The input string or the filename should be OS specific
	//@ but the filename is internally converted to nextar format.
	cFileWriter( const tchar* name, bool bApp );
	virtual ~cFileWriter();

	virtual bool			writeData( void* ptrBuffer, u32 numByte );
	virtual void			seek( s32 numbt, bool fromtop );
	virtual s32				tell();
	virtual s32				getCurrentSize();
	virtual const tchar*	getFileName();
	virtual const tchar*	getFileFullName();

	_n_inline bool isValid() { return mpkFile?true:false; }
};

}


#endif /* cFileWriter */