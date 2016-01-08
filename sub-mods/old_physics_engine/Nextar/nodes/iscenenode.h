//@ Abhishek Dey
//@ Oct 21 2007

#ifndef NEXTAR_ISCENENODE_H
#define NEXTAR_ISCENENODE_H

#include "iobject.h"

//@ Generic node class
//@ The Scene Structure:
//@		+ Our Scene is not completely a scene graph implementation.
//@		It however has a similar structure capeable of 
//@			1. Collision Detection (through iCollisionObject interface)
//@			2. Visiblity Culling for Rendering (implicit)
//@	The fact that we do not seperate the scene structure from collision structure
//@ is because both needs similar formulation for efficient management.
//@ Note that all nodes are not collision nodes. Lights, Sound are not regarded
//@ for collision.
//@ Every node is present in some space which implements iCollisionObject interface
namespace nextar
{

class  iCamera;
class  iSpace;	
struct iCollisionObject;
struct iMaterial;
class iEntity : virtual public iObject
{
public:
	enum RenderPriorityClass
	{
		RenderInit,				// starts rendering
		RenderSky,				// render the sky object
		RenderRoom,				// render solid room object, or polygons with zfill
		RenderSolid,			// unsorted solid render objects
		RenderSolidSort,		// sorted by material
		RenderTransparent,		// render transparent objects without sorting
		RenderTransparentSort,	// render back to front (sorted)
		RenderFinal				// end rendering objects
	};
protected:
	enum
	{
		Disabled			= 1 << 0,
		IsSpace				= 1 << 1,
		LastFlag			= 1 << 2,
	};
public:
	iEntity();
	virtual ~iEntity();
	// parent related utilities
	_n_inline iSpace* getParent() { return mpkParent; }
	_n_inline void	setParentRef(iSpace *pkP) { mpkParent = pkP; }
	_n_inline void	remParentRef() { mpkParent = 0; }
	void setParent(iSpace*);
	void remParent();
	// does it have any collision object, or does it implement collision
	// object
	virtual iCollisionObject* queryCollisionObject() { return 0; }
	virtual void onUpdate(iCamera*) {}
	virtual void onDraw() {}
	_n_inline RenderPriorityClass getRPC() { return meRPC; }
	_n_inline void setRPC(RenderPriorityClass rpc) { meRPC = rpc; }

protected:	
	iSpace*					mpkParent;
	u32						muFlags;
	u32						muVisitNumber;
	RenderPriorityClass		meRPC;
};

// these are certain abstract render priority classes used
class iSolidSortedNode : public iEntity
{
public:
	virtual iMaterial*	getBaseMaterial() = 0;
	virtual f32			getDistanceFromView(f32* viewpoint) = 0;	// input: vec4_t type object
};

}


#endif // NEXTAR_ISCENENODE_H