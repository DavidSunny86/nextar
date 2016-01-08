//@ Abhishek Dey
//@ Oct 25 2007

//@ This is a debug test node, a cube node
//@ to check for physics simulation

#ifndef NEXTAR_CCUBE_NODE
#define NEXTAR_CCUBE_NODE
#include <cbbox.h>
#include <iscenenode.h>
#include <xvertex.h>
#include <imaterial.h>

namespace nextar
{

class cCube : public cBBox,public iSolidSortedNode
{
public:
	cCube(iSpace* pkParent,f32 x,f32 y,f32 z,f32 density = 5.0f);
	~cCube();

	virtual iMaterial*			getBaseMaterial() { return mpkBaseMaterial; }
	virtual f32					getDistanceFromView(f32* viewpoint);
	virtual iCollisionObject*	queryCollisionObject() { return this; }
	virtual void				onUpdate(iCamera*);
	virtual void				onDraw();

	// functions supported
	void	setMaterial(iMaterial*);
	void	setColor(s32 c);

protected:
	iMaterial*  mpkBaseMaterial;
	cVertexStd	mkVertices[24];
	u16			muIndices[36];
};

}
#endif //NEXTAR_CCUBE_NODE