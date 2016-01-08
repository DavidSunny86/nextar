
//@ Abhishek Dey
//@ Nov 22 2007

#ifndef NEXTAR_MESH_FACTORY_H
#define NEXTAR_MESH_FACTORY_H

#include <iobject.h>

namespace nextar
{
class X_Api iMeshFactory : public iFactory<iMeshCore>
{
protected:
	SharedSingleInterface(iMeshFactory);
public:
	
	iMeshFactory();
	virtual ~iMeshFactory();
	//@ load a mesh
	virtual iObject* load(iFileReader* file, u32 uKey = X_InvalidKey );
};

}

#endif //NEXTAR_MESH_FACTORY_H
