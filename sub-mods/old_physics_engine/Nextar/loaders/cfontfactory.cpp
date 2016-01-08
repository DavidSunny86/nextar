//@ Abhishek Dey
//@ Nov 23 2007

#include <loaders/cfontfactory.h>
#include <io/ifilereader.h>
#include <common/xfontfile_s.h>
#include <graphics/irenderer.h>

using namespace nextar;
SharedSingleInterfaceSetup(cFontFactory);

cFontFactory::cFontFactory() {
	SharedSingleInterfaceInit(cFontFactory);
}

cFontFactory::~cFontFactory() {
	SharedSingleInterfaceExit(cFontFactory);
}

iObject* cFontFactory::load(nextar::iFileReader *pkRead, u32 uKey)
{
	iXuiFont* pkFont = 0;
	bool bResult = false;
	if( checkHeader(pkRead,Magic_FontType) )
	{		
		pkFont = X_SharedPtr(iRenderer)->createFont();
		bResult = pkFont->_createFromFile( pkRead );
		if(!bResult)
		{
			pkFont->release();
			return 0;
		}
		if( uKey != X_InvalidKey)
		{
			pkFont->setKey(uKey);
			add(pkFont);
		}
	}
	return pkFont;
}