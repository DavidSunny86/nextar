//@ Abhishek Dey
//@ Nov 23 2007

#include <loaders/iloaderfactory.h>
#include <io/ifilesystem.h>
#include <common/xfile_s.h>
using namespace nextar;

//@ check header
bool iLoaderFactory::checkHeader(nextar::iFileReader *pkRead,u32 code)
{
	file_header kHeader = {0};
	if( pkRead->readData( &kHeader, sizeof(kHeader) ) )
	{		
		if( kHeader.code == code && kHeader.version <= X_VERSION_REAL )
			return true;
	}
	return false;
}

//@ common op
iObject* iLoaderFactory::load(const tchar* sFileName,u32 uKey)
{
	iObject* pkT = get(uKey);
	if(pkT) return pkT;

	iFileReader* pkRead = X_SharedPtr(iFileSystem)->openFile(sFileName);
	if( pkRead )
	{
		pkT = load(pkRead,uKey);
		pkRead->release();
		return pkT;
	}
	return 0;
}