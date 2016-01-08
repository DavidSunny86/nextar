//@ Abhishek Dey
//@ Nov 02 2007
//@ Nov 23 2007

#ifndef IMATERIAL_LOADER_H
#define IMATERIAL_LOADER_H

#include <iobject.h>
#include <io/ifilereader.h>
#include <graphics/imaterial.h>
#include <loaders/iloaderfactory.h>


namespace nextar
{

class X_Api iMaterialFactory : public iFactory<iMaterial>
{
public:
protected:
	SharedSingleInterface(iMaterialFactory);
public:
	iMaterialFactory();
	virtual ~iMaterialFactory();
	//@ load a material from an open file handle. If an invalid key is
	//@ assigned the material is not added to the repository.
	virtual iObject* load(iFileReader* file, u32 uKey);	//not implemented yet
	//@ create a material from class description
	virtual iMaterial* create(s32 class_descx,u32 uKey) = 0;
};

}

#endif //IMATERIAL_LOADER_H