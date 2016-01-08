//@ Abhishek Dey
//@ September 03 2007
//@ September 17 2007

#include <loaders/ctexturefactory.h>
#include <cstr.h>
#include <graphics/irenderer.h>
#include <graphics/itexture.h>

// get file type
using namespace nextar;

struct TxMapper
{
	const tchar* ext;
	cTextureFactory::FileType type;
};

TxMapper sExts[] = 
{
	{X_Text("tga"),cTextureFactory::TGA},
	{X_Text("jpg"),cTextureFactory::JPG},
	{X_Text("jpeg"),cTextureFactory::JPG},
	{X_Text("bmp"),cTextureFactory::BMP},
	{X_Text("png"),cTextureFactory::PNG},
	{0,cTextureFactory::UNKNOWN_TYPE}
};

// ctor
cTextureFactory::cTextureFactory()
{
	RefObjectInit(cTextureFactory);
}

// dtor
cTextureFactory::~cTextureFactory()
{
}

// returns the file type from extension
cTextureFactory::FileType cTextureFactory::_getFileType(iFileReader* pkR)
{
	cStr kName = pkR->getFileName();
	for(s32 i=0; sExts[i].type != cTextureFactory::UNKNOWN_TYPE; i++)
	{
		if(kName.strpos(sExts[i].ext) != -1)
			return sExts[i].type;
	}
	return UNKNOWN_TYPE;
}

// load from file
iObject* cTextureFactory::load(nextar::iFileReader *file,u32 uKey)
{
	cSoftwareTexture* pkT = 0;
	switch(_getFileType(file))
	{
	case cTextureFactory::TGA:
		pkT = _tgaLoader(file);break;
	case cTextureFactory::JPG:
		pkT = _jpgLoader(file);break;
	case cTextureFactory::BMP:
		pkT = _bmpLoader(file);break;
	case cTextureFactory::PNG:
		pkT = _pngLoader(file);break;
	}
	if(pkT)
	{
		iTexture* pkTexture = X_SharedPtr(iRenderer)->createTexture(pkT,muDefTextureFlags);
		X_Assert(pkTexture);
		if(uKey != X_InvalidKey)
		{
			pkTexture->setKey(uKey);
			add(pkTexture);
		}
		pkT->release();
		return pkTexture;
	}
	return 0;
}
