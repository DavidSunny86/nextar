
//@ Abhishek Dey
//@ Nov 02 2007

#include <loaders/imaterialfactory.h>
#include <io/ifilesystem.h>

using namespace nextar;

SharedSingleInterfaceSetup(iMaterialFactory);

//@ ctor
iMaterialFactory::iMaterialFactory()
{
	SharedSingleInterfaceInit(iMaterialFactory);
}

//@ dtor
iMaterialFactory::~iMaterialFactory()
{
	SharedSingleInterfaceExit(iMaterialFactory);
}

iObject* iMaterialFactory::load(nextar::iFileReader *file, u32 uKey)
{
	// this i will specify later
	return 0;
}