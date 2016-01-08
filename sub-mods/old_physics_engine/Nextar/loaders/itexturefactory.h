
//@ Abhishek Dey
//@ September 03 2007

#ifndef ITEXTURE_LOADER_H
#define ITEXTURE_LOADER_H

#include <iobject.h>
#include <loaders/iloaderfactory.h>
#include <graphics/itexture.h>

namespace nextar
{

class X_Api iTextureFactory : public iFactory<iTexture>
{
protected:
	u32				muDefTextureFlags;
	SharedSingleInterface(iTextureFactory);
public:
	
	iTextureFactory();
	virtual ~iTextureFactory();
	//@ get default texture flags
	_n_inline u32  getFlags() { return muDefTextureFlags; }
	_n_inline void setFlags(u32 f) { muDefTextureFlags = f; }
	_n_inline void orFlags(u32 f) { muDefTextureFlags |= f; }
	_n_inline void andFlags(u32 f) { muDefTextureFlags &= f; }
};

}


#endif //ITEXTURE_LOADER_H