//@ Abhishek Dey
//@ Oct 24 2007

#include "collision/icollisionobject.h"
#include "xalloccache.h"
#include "xmemory.h"
#include "collision/icollisionspace.h"

using namespace nextar;

extern u32 suFrustMaskLUT[];

// ctor
iCollisionObject::iCollisionObject(iCollisionSpace* pkParent,bool isPlaceable) :
mpkParent(0),mpkSpaceData(0),mpkOffset(0),mpkWorld(0),miLastViewPlane(0),
mpkBody(0),mpkNext(0),mpkPrev(0),mpkImplementor(0),mpkBodyNext(0)
{
	// this is an abstract class
	muClass = -1;
	mflags = BadAABB|CanCollide;
//	mpkBody = 0;
//	mpkBodyNext = 0;
	if(isPlaceable)
	{
		mflags = (BadAABB|CanCollide|IsDynamic);
		mpkWorld = allocTransform();
		nRSetIdentity(mpkWorld->rot);
		nStoreDwords(mpkWorld->pos,4,0);
	}
	// zero all
	if(pkParent) pkParent->addObject(this);
	NR_ASSERT(mpkParent==pkParent);
}

iCollisionObject::~iCollisionObject()
{
	if(mpkWorld) {
		if(!mpkBody || &mpkBody->mkWorld != mpkWorld )
			freeTransform(mpkWorld);
	}
	if(mpkParent) mpkParent->remObject(this);
}

//@ validate move
void iCollisionObject::validateMove()
{
	// mark its pr as bad
	mflags |= BadAABB;
	if( mpkOffset )
		mflags |= BadTransform;
	
	if(mpkParent)
		mpkParent->moveObject(this);
}

void iCollisionObject::attach(cPhyBody *t)
{
	// assert
	NR_ASSERT(mflags & IsDynamic);
	NR_ASSERT(t);
	if(!mpkBody)	{ freeTransform(mpkWorld); }
	if(mpkBody!=t)
	{
		if(mpkOffset)	{ freeTransform(mpkOffset); mpkOffset = 0; }
		// attaching to a body
		_bodyRemove();
		_bodyAdd(t);
		mpkWorld = &mpkBody->mkWorld;
	}
}

void iCollisionObject::detach()
{
	// detaching from a body
	if(mpkBody) 
	{ 
		if(mpkOffset)	
		{ 
			if(mflags & BadTransform)
				computeTransform();
			freeTransform(mpkOffset); 
			mpkOffset = 0; 
		}
		else
		{
			mpkWorld = allocTransform();
			xQ4CpyMac(mpkWorld->pos,mpkBody->mkWorld.pos);
			xM12CpyMac(mpkWorld->rot,mpkBody->mkWorld.rot);
		}
		_bodyRemove();
	}
}

void iCollisionObject::_bodyAdd(nextar::cPhyBody *b)
{
	NR_ASSERT(mflags & IsDynamic);	//@ static objects dont need body
	NR_ASSERT(b);
	mpkBody = b;
	mpkBodyNext = b->mpkObjects;
	b->mpkObjects = this;
}

void iCollisionObject::_bodyRemove()
{
	if(mpkBody)
	{
		iCollisionObject* t1=0;iCollisionObject* t2 = mpkBody->mpkObjects;
		while(t2)
		{
			if(t2==this)
			{
				if(t1) t1->mpkBodyNext = mpkBodyNext;
				else mpkBody->mpkObjects = mpkBodyNext;
				mpkBody = 0;
				return;
			}
			t1 = t2;
			t2 = t2->mpkBodyNext;
		}
		// if we have reached here we have a problem
		X_ReportBug( "Attempted detach on body not attached to...!","iCollisionObject::detach");
	}
}
//@ culling
u32 iCollisionObject::_InternalCullObject(const plane_t* pkP,const byte* puSign,s32 count,u32 mask)
{
	NR_ASSERT(count <= iCamera::MaxPlaneSet);

	if( mask & (iCamera::MaskPlane1|iCamera::MaskPlane2|
				iCamera::MaskPlane3|iCamera::MaskPlane4|
				iCamera::MaskPlane5|iCamera::MaskPlane6) )
	{
		// we dont need a test, we are already inside
		return mask;
	}

	vec4_t	kP,kN;
	if(miLastViewPlane >= count) miLastViewPlane = 0;
	s32  last = count + miLastViewPlane;
	u32  retmask = mask;

	for(s32 idx = miLastViewPlane; idx < last; idx++)
	{	
		register s32 i = idx%count;
		if( mask & suFrustMaskLUT[i] )
			continue;

		kN[0] = mkBBox.mpoints[(( puSign[i] & iCamera::XNegetive ) >> iCamera::XNegetiveIdx)].x;
		kN[1] = mkBBox.mpoints[(( puSign[i] & iCamera::YNegetive ) >> iCamera::YNegetiveIdx)].y;
		kN[2] = mkBBox.mpoints[(( puSign[i] & iCamera::ZNegetive ) >> iCamera::ZNegetiveIdx)].z;
		kP[0] = mkBBox.mpoints[!(( puSign[i] & iCamera::XNegetive ) >> iCamera::XNegetiveIdx)].x;
		kP[1] = mkBBox.mpoints[!(( puSign[i] & iCamera::YNegetive ) >> iCamera::YNegetiveIdx)].y;
		kP[2] = mkBBox.mpoints[!(( puSign[i] & iCamera::ZNegetive ) >> iCamera::ZNegetiveIdx)].z;

		if( xPCalculatePointR(pkP[i],kN) > 0.f )
		{
			miLastViewPlane = i;
			return iCamera::ObjectCulled;
		}

		if( xPCalculatePointR(pkP[i],kP) > 0.f )
			retmask |= suFrustMaskLUT[i];
	}

	return retmask;
}

u32 iCollisionObject::_InternalCullObject(const plane_t* pkP,const byte* puSign,s32 count)
{
	NR_ASSERT(count <= iCamera::MaxPlaneSet);

	vec4_t	kN;
	if(miLastViewPlane >= count) miLastViewPlane = 0;
	s32  last = count + miLastViewPlane;
	
	for(s32 idx = miLastViewPlane; idx < last; idx++)
	{
		register s32 i = (idx%count);
		kN[0] = mkBBox.mpoints[(( puSign[i] & iCamera::XNegetive ) >> iCamera::XNegetiveIdx)].x;
		kN[1] = mkBBox.mpoints[(( puSign[i] & iCamera::YNegetive ) >> iCamera::YNegetiveIdx)].y;
		kN[2] = mkBBox.mpoints[(( puSign[i] & iCamera::ZNegetive ) >> iCamera::ZNegetiveIdx)].z;
		if( xPCalculatePointR(pkP[i],kN) > 0.f )
		{
			miLastViewPlane = i;
			return iCamera::ObjectCulled;
		}
	}

	return iCamera::ObjectInFrustum;
}

void iCollisionObject::computeTransform()
{
	NR_ASSERT(mpkOffset);
	NR_ASSERT(mpkBody);
	NR_ASSERT(mpkWorld != &mpkBody->mkWorld);
	xTMulEfficient(mpkWorld,mpkOffset,&mpkBody->mkWorld);
}