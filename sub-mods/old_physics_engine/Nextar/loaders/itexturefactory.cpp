
//@ Abhishek Dey
//@ September 03 2007

#include <loaders/itexturefactory.h>

using namespace nextar;

SharedSingleInterfaceSetup(iTextureFactory);

//@ ctor
iTextureFactory::iTextureFactory() : muDefTextureFlags(TCF_SPEED|TCF_MIPMAPS)
{
	SharedSingleInterfaceInit(iTextureFactory);
}

//@ dtor
iTextureFactory::~iTextureFactory()
{
	SharedSingleInterfaceExit(iTextureFactory);
}
