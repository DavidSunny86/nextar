//@ Abhishek Dey
//@ Sept 22 2007

#ifndef CZIPFILEREADER_NEXTAR
#define CZIPFILEREADER_NEXTAR

#include <ifilereader.h>
#include <cstr.h>
// 3rd party library zziplib
#include <zzip/zzip.h>

namespace nextar
{

class cZipFileReader : public iFileReader
{
public:
	enum { IsValid = 1, IsEndOfFile = 2 };

	cZipFileReader(const tchar*,ZZIP_DIR*);
protected:
	virtual ~cZipFileReader();
public:
	//@ tells if the end of file is reached 
	virtual bool isEndOfFile();
	//@ reads data from the file
	virtual bool readData( void* ptrBuffer, u32 numByte );
	//@ set the file pos from current or start
	virtual void seek( s32 nbyte, bool fromtop );
	//@ returns the pos of file from start in bytes
	virtual s32 tell();
	//@ gets u the current file size
	virtual s32 getCurrentSize();
	//@ returns the full file name ( along with path )
	virtual const tchar* getFileFullName();
	//@ returns only the file name
	virtual const tchar* getFileName();
	// is valid
	_n_inline bool isValid() { return muFlags & IsValid; }
private:

	ZZIP_FILE* mpkFile;
	cStr	mkName;
	u32		muFlags;
	s32		miSize;
};

}


#endif //CZIPFILEREADER_NEXTAR