//@ Abhishek Dey
//@ Nov 22 2007

//@ This is a generic geometry
//@ node, world geometry can be
//@ represented by this node
#ifndef NEXTAR_GENERICGEOM_NODE
#define NEXTAR_GENERICGEOM_NODE

#include <cbbox.h>
#include <nodes/iscenenode.h>
#include <graphics/imaterial.h>
#include <mesh/cgenericmesh.h>

namespace nextar
{

class cGenericGeomNode : public iSolidSortedNode
{
public:

	virtual void				onUpdate(iCamera*);
	virtual void				onDraw();

	virtual iMaterial*			getBaseMaterial();
	virtual f32					getDistanceFromView(f32* viewpoint);

	virtual iCollisionObject*	queryCollisionObject() { return mpkCollision; }

protected:
	cGenericMesh*		mpkMesh;
	iCollisionObject*	mpkCollision;
	transform*			mpkWorld;	// if collision is valid, this is = mpkCollision->mpkWorld
};

}

#endif //NEXTAR_GENERICGEOM_NODE