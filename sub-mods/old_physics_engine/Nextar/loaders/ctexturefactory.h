
//@ Abhishek Dey
//@ September 03 2007
//@ September 17 2007

#include <loaders/itexturefactory.h>
#include <graphics/csoftwaretexture.h>
#include <graphics/itexture.h>

#ifndef XTEXTURELOADER_INSTANCE
#define XTEXTURELOADER_INSTANCE

namespace nextar
{

class cTextureFactory : public iTextureFactory
{
public:
	enum FileType { TGA, JPG, BMP, PNG, UNKNOWN_TYPE };
public:
	cTextureFactory();
	virtual ~cTextureFactory();

	virtual iObject* load(iFileReader* file,u32 assignKey);

private:
	FileType			_getFileType(iFileReader*);
	cSoftwareTexture*	_tgaLoader(iFileReader*);
	cSoftwareTexture*	_jpgLoader(iFileReader*);
	cSoftwareTexture*	_bmpLoader(iFileReader*);
	cSoftwareTexture*	_pngLoader(iFileReader*);
private:
};

}
#endif //XTEXTURELOADER_INSTANCE