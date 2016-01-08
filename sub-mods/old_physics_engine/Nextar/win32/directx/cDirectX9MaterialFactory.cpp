
#include <graphics/irenderer.h>
#include <win32/directx/cdirectx9materialfactory.h>
#include <win32/directx/directx9materials.h>

using namespace nextar;

cDirectX9MaterialFactory::cDirectX9MaterialFactory(void)
{
	RefObjectInit(cDirectX9MaterialFactory);
	X_Assert(X_SharedPtr(iRenderer));
	cDirectX9MaterialAlphaMtl::Initialize();
}

cDirectX9MaterialFactory::~cDirectX9MaterialFactory(void)
{
	cDirectX9MaterialAlphaMtl::Destroy();
}

iMaterial* cDirectX9MaterialFactory::create(s32 class_descx, u32 uKey)
{
	iMaterial* ret = 0;
	switch(class_descx)
	{
	case iMaterial::Solid:
		ret = new cDirectX9SolidMtl();
		break;
	case iMaterial::TransparentMtlAlpha:
		ret = new cDirectX9MaterialAlphaMtl();
		break;
	}
	if(ret && uKey !=X_InvalidKey)
	{
		ret->setKey(uKey);
		addType(ret);
	}
	return ret;
}
