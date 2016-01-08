//@ Abhishek Dey
//@ Nov 22 2007

#ifndef NEXTAR_STATIC_SHADOW
#define NEXTAR_STATIC_SHADOW

#include "mesh/iobject.h"
#include "graphics/cvertexbuffer.h"
#include "graphics/cindexbuffer.h"

namespace nextar
{

//@ shadow volume for static meshes
//@ can be generated from here
class cStaticShadow : public iObject
{
public:
	static iMaterial*	spkShadowRenderer;
};

};

#endif //NEXTAR_STATIC_SHADOW