//@ Abhishek Dey
//@ Nov 23 2007

#include <win32/directx/cdirectx9uiskinfactory.h>
#include <win32/directx/cdirectx9uiskin.h>

using namespace nextar;

cDirectX9XuiSkinFactory::cDirectX9XuiSkinFactory(void)
{
	RefObjectInit(cDirectX9XuiSkinFactory);
}

cDirectX9XuiSkinFactory::~cDirectX9XuiSkinFactory(void)
{
}

iObject* cDirectX9XuiSkinFactory::load(iFileReader* file, u32 uKey)
{
	iObject* obj = new cDirectX9UISkin(file);
	if(uKey != X_InvalidKey)
	{
		obj->setKey(uKey);
		add(obj);
	}
	return obj;
}