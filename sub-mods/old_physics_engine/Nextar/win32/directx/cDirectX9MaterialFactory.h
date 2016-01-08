//@ Abhishek Dey
//@ Nov 02 2007

#ifndef DIRECTX9_MATERIAL_FACTORY
#define DIRECTX9_MATERIAL_FACTORY

#include <loaders/imaterialfactory.h>

namespace nextar
{

class cDirectX9MaterialFactory :
	public nextar::iMaterialFactory
{
public:
	cDirectX9MaterialFactory(void);
public:
	~cDirectX9MaterialFactory(void);
public:
	virtual iMaterial* create(s32 class_descx,u32 uKey);
};

}

#endif //DIRECTX9_MATERIAL_FACTORY