
#ifndef NEXTAR_MESH_FACTORY_IMPLEMENTATION
#define NEXTAR_MESH_FACTORY_IMPLEMENTATION

#include "loaders/imeshfactory.h"

namespace nextar
{

class cMeshFactory : public iMeshFactory
{
public:
	virtual iMeshCore* load(iFileReader* file, u32 classDesc, u32 uKey = X_InvalidKey );
};

}

#endif //NEXTAR_MESH_FACTORY_IMPLEMENTATION