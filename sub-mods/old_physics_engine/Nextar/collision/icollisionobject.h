//@ Abhishek Dey
//@ Oct 21 2007
//@ Oct 24 2007

#ifndef NEXTAR_ICOLLISION_OBJECT_H
#define NEXTAR_ICOLLISION_OBJECT_H

#include "nodes/iscenenode.h"
#include "iobject.h"
#include "camera/icamera.h"
#include "physics/cphybody.h"
#include "physics/csurface.h"

namespace nextar
{
class iCollisionSpace;
// generic collision object
struct iCollisionObject : virtual public iObject
{
	enum Classes
	{
		Col_FirstColSpace,
		Col_QTree = Col_FirstColSpace,
		Col_LastColSpace = Col_QTree,
		Col_BBox,
		Col_Plane,
		Col_NumClasses
	};

	enum { 
		CollideEnabled			= 1 << 1,	// means eligible for collision in this update
		IsDirty					= 1 << 2,	// need update
		BadTransform			= 1 << 3,	// position and rotation not valid
		BadAABB					= 1 << 4,	// aabb_t not valid
		IsDynamic				= 1 << 5,	// can move in the world
		CanCollide				= 1 << 6,	// is collidable object, this enables disables collision
		// space related
		IsSpace					= 1 << 7,	// is a collision space object
		IsComplexSpace			= 1 << 8,	// contains objects at different levels
		IsSimpleSpace			= 1 << 9,	// contains a list of objects
		IsAPortal				= 1 << 10,
		MovedCollisionChecked	= 1 << 12,	// internal flag
		LastFlag				= 1 << 11,	// last flag indicator
	};
	
	enum ColRetCode
	{
		WasHandled,
		NeedsResolution
	};

//@ functions:

	//@ ctor
	iCollisionObject(iCollisionSpace* pkParent,bool isDyn = true);
	virtual ~iCollisionObject();

//	_n_inline void			setPosInWorld(f32 x,f32 y,f32 z) {
//		if(mpkWorld) { mpkWorld->pos[0] = x; mpkWorld->pos[1] = y; mpkWorld->pos[2] = z; validateMove(); }
//	}
	_n_inline void			setSurfacePtr(cSurface* pkS) { mpkSurface = pkS; }
	_n_inline cPhyBody*		getBody() { return mpkBody; }
	_n_inline iEntity*		querySceneNode() { return mpkImplementor; }
	_n_inline const aabb_t&	getBoundingBox() { return mkBBox; }
	_n_inline bool			isEnabled() { return muFlags & CollideEnabled; }
	_n_inline void			enableCollision() { if(muFlags & CanCollide) muFlags |= CollideEnabled; }
	_n_inline void			disableCollision() { muFlags &=~CollideEnabled; }
	_n_inline u32				getClass() { return muClass; }

	// if object is not culled return the mask for the children
	void attach(cPhyBody*);
	void detach();
	void validateMove();
	void computeTransform();

	void _bodyAdd(cPhyBody*);
	void _bodyRemove();
	u32 _InternalCullObject(const plane_t* planes,const byte* sign_bits,s32 count,u32 mask);
	u32 _InternalCullObject(const plane_t* planes,const byte* sign_bits,s32 count);

	virtual void computeAABB() {}
	virtual ColRetCode onAABBColliding(iCollisionObject*) { return NeedsResolution; }	// this is called when two aabbs collide

//@ data members:
	// frustum culling specific data
	s32					miLastViewPlane;	// plane_t coherency
	aabb_t				mkBBox;
	u32					muClass;
	u32					muFlags;
	transform*			mpkOffset; // this is only valid if the object is relative to parent
	transform*			mpkWorld;  // this ptr is equal to parent, if(parent is valid and no offset is set)
	cPhyBody*			mpkBody;
	iEntity*			mpkImplementor;
	cSurface*			mpkSurface;	// this must be valid
	iCollisionObject*	mpkBodyNext;
	iCollisionObject*	mpkNext;
	iCollisionObject*	mpkPrev;
	iCollisionSpace*	mpkParent;
	void*				mpkSpaceData;
//@ static members:
};

}

#endif //NEXTAR_ICOLLISION_OBJECT_H