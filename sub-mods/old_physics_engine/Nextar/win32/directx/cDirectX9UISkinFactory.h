//@ Abhishek Dey
//@ Nov 23 2007

#ifndef DIRECTX9_SKIN_FACTORY
#define DIRECTX9_SKIN_FACTORY

#include <loaders/ixuiskinfactory.h>

namespace nextar
{

class cDirectX9XuiSkinFactory :
	public nextar::iXuiSkinFactory
{
public:
	cDirectX9XuiSkinFactory(void);
public:
	~cDirectX9XuiSkinFactory(void);
public:
	virtual iObject* load(iFileReader* file, u32 uKey);
};

}

#endif //DIRECTX9_MATERIAL_FACTORY