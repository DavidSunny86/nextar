
//@ Abhishek Dey
//@ Nov 23 2007

#include <loaders/ixuiskinfactory.h>

using namespace nextar;

SharedSingleInterfaceSetup(iXuiSkinFactory);

//@ ctor
iXuiSkinFactory::iXuiSkinFactory()
{
	SharedSingleInterfaceInit(iXuiSkinFactory);
}

//@ dtor
iXuiSkinFactory::~iXuiSkinFactory()
{
	SharedSingleInterfaceExit(iXuiSkinFactory);
}
