
#include "collision/xcollisionutility.h"
#include "xmemory.h"
#include "collision/icollisionobject.h"
#include "collision/icollisionspace.h"
#include "physics/cnpconstrain.h"
#include "physics/csimulator.h"

namespace nextar
{
namespace collision
{

typedef s32 xColliderFunction (iCollisionObject* pkO1,iCollisionObject* pkO2);
cContact* gpkContactHolder = 0;
struct cCollisionLUT
{
	xColliderFunction*	mOp;
	bool				mbReverse;
};

cCollisionLUT gkColLut[iCollisionObject::Col_NumClasses][iCollisionObject::Col_NumClasses] = {0};

void setCollider(xColliderFunction* Op, s32 class1, s32 class2)
{
	gkColLut[class2][class1].mOp = Op;
	gkColLut[class2][class1].mbReverse = true;
	gkColLut[class1][class2].mOp = Op;
	gkColLut[class1][class2].mbReverse = false;
}

void xInitializeColliders()
{
	// allocate memory for contact manifold
	gpkContactHolder = (cContact*)allocMemoryAligned(sizeof(cContact)*N3DMaxContact);
	zeroMemory(gpkContactHolder,sizeof(cContact)*N3DMaxContact);
	// set up the LUT
	// space colliders
	for( s32 i = iCollisionObject::Col_FirstColSpace; i <= iCollisionObject::Col_LastColSpace; i++ )
		for( s32 j = 0; j < iCollisionObject::Col_NumClasses; j++ )
			setCollider(&xColSpace,i,j);
	// primitive colliders
	setCollider(&xColBoxBox,iCollisionObject::Col_BBox,iCollisionObject::Col_BBox);
	setCollider(&xColBoxPlane,iCollisionObject::Col_BBox,iCollisionObject::Col_Plane);
}

void xCollideObjects(iCollisionObject* pkO1,iCollisionObject* pkO2)
{
	// dont collide if in same body
	if(pkO1->mpkBody == pkO2->mpkBody) return;
	const aabb_t& aabb1 = pkO1->mkBBox;
	const aabb_t& aabb2 = pkO2->mkBBox;
	if( aabb1.min.x > aabb2.max.x ||	aabb1.min.y > aabb2.max.y ||	aabb1.min.z > aabb2.max.z ||
		aabb2.min.x > aabb1.max.x ||	aabb2.min.y > aabb1.max.y ||	aabb2.min.z > aabb1.max.z )
		return;
	// phase 3
	// 2.1 check if any inbuilt function satisifes the condition
	if( pkO1->onAABBColliding(pkO2) == iCollisionObject::WasHandled ||
		pkO2->onAABBColliding(pkO1) == iCollisionObject::WasHandled ) 
		return;
	// phase 4
	// s32 iCnt = xCollide(pkO1,pkO2);
	X_Assert(pkO1->muClass>=0&&pkO1->muClass<iCollisionObject::Col_NumClasses);
	X_Assert(pkO2->muClass>=0&&pkO2->muClass<iCollisionObject::Col_NumClasses);
	// we do not hope pkO1 and pkO2 will be same, in case they
	// are, we need to inform that there was some sort of 
	// error in the space algo.
	X_Assert( pkO1 != pkO2 );
	cCollisionLUT& rkEntry = gkColLut[pkO1->muClass][pkO2->muClass];
	s32  iCnt = 0;
	// there is no harm in altering the geoms
//	if(rkEntry.mbReverse)
//	{
//		cXpeGeom* g = pkO1;
//		pkO1 = pkO2;
//		pkO2 = g;
//	}
	if( rkEntry.mOp )
	{
		if(rkEntry.mbReverse)
		{
			// just swap the bodies
			iCollisionObject* pkB = pkO1;
			pkO1 = pkO2;
			pkO2 = pkB;
		}
		iCnt = (*rkEntry.mOp)(pkO1,pkO2);
		//final phase: handle penetration
		if(iCnt)
		{
			X_Assert(pkO1->mpkSurface && pkO2->mpkSurface);
#if defined(NEXTAR_USE_NP_CONSTRAINS) && defined(NEXTAR_INCLUDE_NON_PENETRATION_CONSTRAINS)
			if(pkO1->mpkBody)
			{
				X_SharedPtr(cSimulator)->addNPConstrain()->initialize(pkO1->mpkBody,pkO2->mpkBody,iCnt);
			}
			else if(pkO2->mpkBody)
			{
				X_SharedPtr(cSimulator)->addNPConstrain()->initialize(pkO2->mpkBody,pkO1->mpkBody,iCnt,true);
			}
#else
			cSurface *s1=pkO1->mpkSurface,*s2 = pkO2->mpkSurface;
			if(pkO1->mpkBody)
			{

				X_SharedPtr(cSimulator)->addFCConstrain()->initialize(pkO1->mpkBody,pkO2->mpkBody,s1,s2,iCnt);
			}
			else if(pkO2->mpkBody)
			{
				X_SharedPtr(cSimulator)->addFCConstrain()->initialize(pkO2->mpkBody,pkO1->mpkBody,s2,s1,iCnt,true);
			}
#endif
		}
	}
}

s32 xColSpace(iCollisionObject* o1,iCollisionObject* o2)
{
	// if o1 is space and o2 is space
	nextar::iCollisionObject* pkGeom=0;
	nextar::iCollisionSpace* pkSpace;
	if(!xColObjectIsSpace(o1)) pkGeom = o1; else pkSpace = (nextar::iCollisionSpace*)o2;
	if(!xColObjectIsSpace(o2)) pkGeom = o2; else pkSpace = (nextar::iCollisionSpace*)o1;
	if( !pkGeom )
	{
		// pick the complex space and collide with it
		nextar::iCollisionSpace* pkSpace1 = (nextar::iCollisionSpace*)o1;
		nextar::iCollisionSpace* pkSpace2 = (nextar::iCollisionSpace*)o2;
		if( xColSpaceIsComplex(o2) )
		{
			pkSpace1 = (nextar::iCollisionSpace*)o2;
			pkSpace2 = (nextar::iCollisionSpace*)o1;
		}
		// collide
		pkSpace1->onExternalCollide(pkSpace2);
	}
	else
		// get the geom and collide
		pkSpace->onExternalCollide(pkGeom);
	// this is internal so return 0
	return 0;
}

}
}