//@ Abhishek Dey
//@ Nov 10 2007

//@ This is a debug test node, a plane node
//@ to check for physics simulation

#ifndef NEXTAR_GROUNDPLANE_NODE
#define NEXTAR_GROUNDPLANE_NODE
#include "collision/cplane.h"
#include "nodes/iscenenode.h"
#include "xvertex.h"
#include "graphics/imaterial.h"

namespace nextar
{

class cGroundPlane : public cPlane,public iSolidSortedNode
{
public:
	cGroundPlane(iSpace* pkParent);
	// to define a custom plane, provide 3 points, last point is auto generated
//	cGroundPlane(iSpace* pkParent,f32* point1,f32* point2,f32* point3);
	~cGroundPlane();

	virtual iMaterial*			getBaseMaterial() { return mpkBaseMaterial; }
	virtual f32					getDistanceFromView(f32* viewpoint);
	virtual iCollisionObject*	queryCollisionObject() { return this; }
	virtual void				onUpdate(iCamera*);
	virtual void				onDraw();

	// functions supported
	void	setMaterial(iMaterial*);
	void	tileTexture(s32 num_times);

protected:
	iMaterial*  mpkBaseMaterial;
	cVertexStd	mkVertices[4];
	u16			muIndices[6];
};

}
#endif //NEXTAR_GROUNDPLANE_NODE