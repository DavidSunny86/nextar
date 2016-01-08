//@ Abhishek Dey
//@ Nov 2 2007

//@ This is a debug test node, a light node
//@ to allow lighting, more robust model
//@ will be later provided, to use with lightmaps

#ifndef NEXTAR_BASICLIGHT_NODE
#define NEXTAR_BASICLIGHT_NODE

#include <iscenenode.h>
#include <clight.h>

namespace nextar
{

class cBasicLight : public iEntity
{

protected:
	cLight	mkLight;
public:

	cBasicLight(iSpace* pkParent,cLight::Type,const f32* direction = 0,f32 radius=100.f,u32 color = 0xffffffff);
	virtual void onUpdate(iCamera*);
};

}

#endif //NEXTAR_BASICLIGHT_NODE