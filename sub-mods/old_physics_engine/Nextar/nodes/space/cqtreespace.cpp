//@ Abhishek Dey
//@ Oct 24 2007

#include <cqtreespace.h>
#include <camera.h>
#include <iportal.h>
#include <xcollisionutility.h>

using namespace nextar;

#define X_AABB_INSIDE(ptr,bb)  (bb.min.x >= ptr->mfMinX && \
								bb.max.x <= ptr->mfMaxX && \
								bb.min.z >= ptr->mfMinZ && \
								bb.max.z <= ptr->mfMaxZ)

#define TRAVERSE_UPWARD(dowhat) \
	Block* pkBlock = this; \
	do { \
		pkBlock->dowhat; \
		pkBlock = pkBlock->mpkParent; \
	}while( pkBlock );


//@===================Quad Tree::Block================
cQTreeSpace::Block::Block() : mpkFirst(0),muObjectCount(0),
mpkParent(0),mpkChildren(0),miFrustTC(0)
{
}

cQTreeSpace::Block::~Block()
{
	iCollisionObject* gm = mpkFirst;
	while(gm)
	{
		iCollisionObject* nx = gm->mpkNext;
		//@ this is the valid way of removing 
		//@ any object from a space that
		//@ inherits both a iRegion and 
		//@ iCollisionSpace
		gm->addRef();
		if(gm->querySceneNode())
			gm->querySceneNode()->remParent();
		else
		{
			// this is a generic collision object with
			// no scene node implementor, could be
			// anything, like a trigger
			// std remove
			gm->mpkParent = 0;
			gm->mpkSpaceData = 0;
			gm->mpkNext = 0;
			gm->mpkPrev = 0;
			gm->release(); // we added it with addRef
		}
		gm->release();
		gm = nx;
	}
}

void cQTreeSpace::Block::blockCreate(const vec3_t &kC,f32 fXExt,f32 fZExt,Block* kPar,Block*& pkArr,s32 iDepth)
{
	X_Assert(!mpkChildren);
	X_Assert(!mpkFirst);
	muObjectCount = 0;
	mfMaxX = kC.x + fXExt;
	mfMaxZ = kC.z + fZExt;
	mfMinX = kC.x - fXExt;
	mfMinZ = kC.z - fZExt;
	mpkParent = kPar;
	if(iDepth > 0)
	{
		mpkChildren = pkArr;
		pkArr += 4;
		f32 fxEx = fXExt / 2.0f;
		f32 fxEz = fZExt / 2.0f;
		for(s32 i=0; i<2; i++)
		{
			for(s32 j=0; j<2; j++)
			{
				vec3_t kcC;
				kcC.x = kC.x - fXExt + fxEx + i*(fxEx*2);
				kcC.z = kC.z - fZExt + fxEz + j*(fxEz*2);
				kcC.y = kC.y;
				mpkChildren[i*2+j].blockCreate(kcC,fxEx,fxEz,this,pkArr,iDepth-1);
			}
		}
	}
}

void cQTreeSpace::Block::blockAdd(iCollisionObject *pkO)
{
	X_BreakOutIf( (Block*)(pkO->mpkSpaceData)==this ); // already added
	pkO->mpkSpaceData = (void*)this;
	pkO->mpkNext = mpkFirst;
	if( mpkFirst )
		mpkFirst->mpkPrev = pkO;
	pkO->mpkPrev = 0;
	mpkFirst = pkO;
	TRAVERSE_UPWARD(muObjectCount++);
}

void cQTreeSpace::Block::blockRem(iCollisionObject *pkO)
{
	X_BreakOutIf((Block*)(pkO->mpkSpaceData)!=this);
	pkO->mpkSpaceData= 0;
	if( pkO == mpkFirst ) mpkFirst = pkO->mpkNext;
	if(pkO->mpkPrev) 
		pkO->mpkPrev->mpkNext = pkO->mpkNext;
	if(pkO->mpkNext)
		pkO->mpkNext->mpkPrev = pkO->mpkPrev;
	pkO->mpkPrev = 0;
	pkO->mpkNext = 0;
	TRAVERSE_UPWARD(muObjectCount--);
}

void cQTreeSpace::Block::collide(iCollisionObject* pkObj)
{
	// O(n^2) collide
	iCollisionObject* pkG = mpkFirst;
	while( pkG ) {
		// call the internal collide if this geom is a space
		if( !(pkG->muFlags & iCollisionObject::MovedCollisionChecked) )
			collision::xCollideObjects( pkObj, pkG );
		
		pkG = pkG->mpkNext;
	}
}

void cQTreeSpace::Block::collideChildren(iCollisionObject* pkF)
{
	iCollisionObject* pkN = mpkFirst;
	while( pkN )
	{
		if( !(pkN->muFlags & iCollisionObject::MovedCollisionChecked)
			&& pkN != pkF )
			collision::xCollideObjects(pkF,pkN);
		pkN = pkN->mpkNext;
	}

	if( mpkChildren ) {
		for (s32 i = 0; i < 4; i++) {
			if( mpkChildren[i].muObjectCount == 0 ||
				pkF->mkBBox.min.x  > mpkChildren[i].mfMaxX ||
				pkF->mkBBox.max.x < mpkChildren[i].mfMinX ||
				pkF->mkBBox.min.z > mpkChildren[i].mfMaxZ ||
				pkF->mkBBox.max.z < mpkChildren[i].mfMinZ
			   )
			   continue;
			mpkChildren[i].collideChildren(pkF);
		}
	}
}

cQTreeSpace::Block* cQTreeSpace::Block::blockGet(const aabb_t &rkBBox)
{
	/* 
	if( mpkChildren ) {
		for( u32 i = 0; i < 4; i++ ) {
			if( X_AABB_INSIDE((&mpkChildren[i]),rkBBox) )
				return mpkChildren[i].blockGet(rkBBox);
		}
	}
	return this;
	*/
	// non recursive procedure for the above code.
	Block* pkCurr = this;
	Block* pkChildren = mpkChildren;
	while( pkChildren ) {
		u32 i;
		for( i = 0; i < 4; i++ ) {
			if( X_AABB_INSIDE(pkChildren,rkBBox) ) {
				pkCurr = pkChildren;
				pkChildren = pkCurr->mpkChildren;
				break;
			}
			pkChildren++;
		}
		if( i == 4 ) 
			return pkCurr;
	}
	return pkCurr;
}

cQTreeSpace::Block* cQTreeSpace::Block::blockGetAbsolute(const aabb_t &rkBBox)
{
	Block* pkCurr = this;
	
	while(pkCurr) {
		if( X_AABB_INSIDE( pkCurr, rkBBox ) )
			break;
		else
		{
#ifdef X_COLLISION_USING_INFINTE_EXTENDS_FOR_PRIMITIVES
			if(pkCurr->mpkParent)
#endif //X_COLLISION_USING_INFINTE_EXTENDS_FOR_PRIMITIVES
				pkCurr = pkCurr->mpkParent;
#ifdef X_COLLISION_USING_INFINTE_EXTENDS_FOR_PRIMITIVES
			else
				break;
#endif //X_COLLISION_USING_INFINTE_EXTENDS_FOR_PRIMITIVES
		}
	}

	X_Assert(pkCurr); 
	//@ cause if the object is outside its parent, 
	//@ we gotta handle it seperately.
	Block* pkChildren = pkCurr->mpkChildren;
	while( pkChildren ) {
		u32 i;
		for( i = 0; i < 4; i++ ) {
			if( X_AABB_INSIDE(pkChildren,rkBBox) ) {
				pkCurr = pkChildren;
				pkChildren = pkCurr->mpkChildren;
				break;
			}
			pkChildren++;
		}
		if( i == 4 ) 
			return pkCurr;
	}
	return pkCurr;
}

#ifdef X_USE_FRUSTUM_MASKING
extern u32 suFrustMaskLUT[];
void cQTreeSpace::Block::cull(iCamera* pkC)
{
	X_Assert(pkC);

	u32 mask = pkC->getMask();
	
	if( mask & (iCamera::MaskPlane1|iCamera::MaskPlane2|
				iCamera::MaskPlane3|iCamera::MaskPlane4|
				iCamera::MaskPlane5|iCamera::MaskPlane6) )
	{
		// we dont need a test, we are already inside
		preRenderSubtree();
	}

	{
		f32*	pkP		= pkC->getTestPlanes();
		s32		count	= pkC->getPlaneCount();
		byte*	puSign	= pkC->getSignFields();

		vec4_t	kP,kN;
		if(miFrustTC >= count) miFrustTC = 0;
		s32  last = count + miFrustTC;
		u32  newmask = mask;
		
		for(s32 i = miFrustTC; i < last; i++)
		{	
			if( mask & suFrustMaskLUT[i] )
				continue;
			kN[0] = mfMPoints[(( puSign[i] & iCamera::XNegetive ) >> iCamera::XNegetiveIdx)].x;
			kN[2] = mfMPoints[(( puSign[i] & iCamera::ZNegetive ) >> iCamera::ZNegetiveIdx)].y;
			kP[0] = mfMPoints[!(( puSign[i] & iCamera::XNegetive ) >> iCamera::XNegetiveIdx)].x;
			kP[2] = mfMPoints[!(( puSign[i] & iCamera::ZNegetive ) >> iCamera::ZNegetiveIdx)].y;

			if( puSign[i] & iCamera::YNegetive ) {
				kN[1] = N3DInfinity; 
				kP[1] =-N3DInfinity;
			}
			else {
				kN[1] =-N3DInfinity; 
				kP[1] = N3DInfinity;
			}
			
			register s32 idx = i%count;
			if( xPCalculatePointR(&pkP[idx],&kN) > 0.f )
			{
				miFrustTC = idx;
				return;
			}

			if( xPCalculatePointR(&pkP[idx],&kP) > 0.f )
				newmask |= suFrustMaskLUT[idx];
		}
		pkC->setMask(newmask);
	}

	iCollisionObject* pkG = mpkFirst;
	while( pkG ) {
		if(pkG->querySceneNode())
			pkG->querySceneNode()->onUpdate(pkC);
		pkG = pkG->mpkNext;
	}

	if(mpkChildren)
	{
		for(s32 i=0; i<4; i++)
			mpkChildren[i].cull(pkC);
	}
}
#else
void cQTreeSpace::Block::cull(iCamera* pkC)
{
	X_Assert(pkC);

	{
		plane_t*pkP		= pkC->getTestPlanes();
		s32		count	= pkC->getPlaneCount();
		byte*	puSign	= pkC->getSignFields();

		vec4_t	kN;
		if(miFrustTC >= count) miFrustTC = 0;
		s32  last = count + miFrustTC;
		
		for(s32 i = miFrustTC; i < last; i++)
		{	
			kN[0] = mfMPoints[(( puSign[i] & iCamera::XNegetive ) >> iCamera::XNegetiveIdx)].x;
			kN[1] = ( puSign[i] & iCamera::YNegetive ) ? N3DInfinity :-N3DInfinity;
			kN[2] = mfMPoints[(( puSign[i] & iCamera::ZNegetive ) >> iCamera::ZNegetiveIdx)].y;
		
			register s32 idx = i%count;
			if( xPCalculatePointR(pkP[idx],kN) > 0.f )
			{
				miFrustTC = idx;
				return;
			}
		}
	}

	iCollisionObject* pkG = mpkFirst;
	while( pkG ) {
		if(pkG->querySceneNode())
			pkG->querySceneNode()->onUpdate(pkC);
		pkG = pkG->mpkNext;
	}

	if(mpkChildren)
	{
		for(s32 i=0; i<4; i++)
			mpkChildren[i].cull(pkC);
	}
}
#endif

void cQTreeSpace::Block::blockTraverse(iCollisionObject* pkObj)
{
	cQTreeSpace::Block* pkB = blockGetAbsolute(pkObj->mkBBox);
	if( pkB != this )
	{
		blockRem(pkObj);
		pkB->blockAdd(pkObj);
	}
}

//@===================Quad Tree========================
cQTreeSpace::cQTreeSpace(iRegion* pkSpace,const vec3_t &kCenter,f32 fXExt,f32 fZExt,s32 iDepth) :
iRegion(),
iCollisionSpace(0,false),
muGeomCount(0),muBlockCount(0)
{
	mpkImplementor = this;
	RefObjectInit(cQTreeSpace);
	muClass = Col_QTree;
	iCollisionObject::muFlags |= iCollisionObject::IsComplexSpace|iEntity::IsSpace;
	// get block count
	for(s32 i = 0; i <= iDepth; i++ )
		muBlockCount += 1 << i*2;
	mpkRoot = new Block[muBlockCount];

	fXExt *= 0.5f;
	fZExt *= 0.5f;

	mkBBox.min.x = kCenter.x-fXExt;
	mkBBox.min.y =-N3DInfinity;
	mkBBox.min.z = kCenter.z-fZExt;
	mkBBox.max.x = kCenter.x+fXExt;
	mkBBox.max.y = N3DInfinity;
	mkBBox.max.z = kCenter.z+fZExt;

	Block* pkStart = mpkRoot+1;
	mpkRoot[0].blockCreate(kCenter,fXExt,fZExt,0,pkStart,iDepth);
	if(pkSpace)
	{
		pkSpace->addChildNode(this);
		iEntity::mpkParent = pkSpace;
	}
}

cQTreeSpace::~cQTreeSpace()
{
	if(mpkRoot) delete [] mpkRoot;
	for(list_t<iEntity*>::iterator it = mkDecorators.begin(); it.ok(); ++it )
		(*it)->release();
	muBlockCount = 0;
	mpkRoot = 0;
	mkDirtyList.clear();
}

//@ the prerender culling
void cQTreeSpace::onUpdate(nextar::iCamera *pkCam)
{
	for(list_t<iEntity*>::iterator it = mkDecorators.begin(); it.ok(); ++it )
		(*it)->onUpdate(pkCam);
	mpkRoot[0].cull(pkCam);
}

//@ portal addition,
//@ a portal is only added to a space if
//@ it is already pointing to this space
void cQTreeSpace::addPortal(nextar::iPortal *pkPortal)
{
	X_Assert( pkPortal->to() == this || pkPortal->from() == this );
	X_Assert( pkPortal->mpkParent == 0 && pkPortal->mpkNext == 0 );
	X_Assert( xColSpaceIsNotLocked(this) );
	//@ portal is a generic collision object,
	//@ its static in nature and is no different
	//@ from the other collision nodes that we 
	//@ added.
	addObject(pkPortal);
	muGeomCount++;
}

//@ add a child node, could be a decorator,
//@ or a collision node
void cQTreeSpace::addChildNode(iEntity* pkNode)
{
	iCollisionObject* pkO = pkNode->queryCollisionObject();
	if( pkO )
		addObject(pkO);
	else
	{
		mkDecorators.push_back(pkNode);
		pkNode->addRef();
	}
}

//@ rem a child node, could be a decorator,
//@ or a collision node
void cQTreeSpace::remChildNode(iEntity* pkNode)
{
	iCollisionObject* pkO = pkNode->queryCollisionObject();
	if( pkO )
		remObject(pkO);
	else
	{
		X_Assert(pkNode->getParent()==this);
		mkDecorators.remove(pkNode);
		pkNode->release();
	}
}

//@ add an object
void cQTreeSpace::addObject(nextar::iCollisionObject *pkObj)
{
	//@ make sure the object does not have a
	//@ parent.
	if( pkObj->mpkParent == this )
		return;

	pkObj->addRef();
	pkObj->muFlags |= iCollisionObject::BadAABB|iCollisionObject::IsDirty;
	mkDirtyList.push_back((array_t::alloc_type)pkObj);
	pkObj->mpkParent = this;
	//@ add it to this block
	mpkRoot->blockGet(pkObj->mkBBox)->blockAdd(pkObj);
	// xValidateMove();
}

//@ rem an object
void cQTreeSpace::remObject(nextar::iCollisionObject *pkObj)
{
	X_BreakOutIf(!pkObj);
	X_Assert( xColSpaceIsNotLocked(this) );
	X_BreakOutIf(pkObj->mpkParent!=this);

	if( pkObj->muFlags & iCollisionObject::IsDirty )
	{
		for( u32 i = 0; i < mkDirtyList.size(); i++ ) {
			if( mkDirtyList[i] == (u32)pkObj )	{
				mkDirtyList.best_erase(i);
				i--;
			}
		}
	}

	((Block*)(pkObj->mpkSpaceData))->blockRem(pkObj);
	muGeomCount--;

	pkObj->mpkParent = 0;
	pkObj->mpkNext = 0;
	pkObj->mpkPrev = 0;
	pkObj->release();
	// xValidateMove();
}

void cQTreeSpace::moveObject(nextar::iCollisionObject *pkO)
{
	X_BreakOutIf( pkO->mpkParent != this );
	// enable for collision
	if(!(pkO->muFlags & iCollisionObject::IsDirty))
	{
		pkO->muFlags |= iCollisionObject::CollideEnabled|iCollisionObject::IsDirty;
		mkDirtyList.push_back((array_t::alloc_type)pkO);
	}
}

void cQTreeSpace::_cleanObjects()
{
	miLockCount++;
	for( u32 i = 0; i < mkDirtyList.size(); i++ )
	{
		iCollisionObject* pkObj = (iCollisionObject*)mkDirtyList[i];

//		if( pkObj->muFlags & iCollisionObject::IsSpace )
//			((iCollisionSpace*)pkObj)->cleanObjects();

		if( pkObj->muFlags & iCollisionObject::BadTransform )
			pkObj->computeTransform();

		if( pkObj->muFlags & iCollisionObject::BadAABB )
			pkObj->computeAABB();

		pkObj->muFlags &= (~(iCollisionObject::BadAABB|iCollisionObject::BadTransform|iCollisionObject::IsDirty));
		
	//@
	//@	TODO::
	//@		this is not supposed to take place, but if it
	//@		does, we need to do something about it.
	//@
	//@	if( !X_AABB_INSIDE(mpkRoot,pkObj->mkBBox) )
	//@		mkRemoveList.push_back((array_t::alloc_type)pkObj);

		((Block*)pkObj->mpkSpaceData)->blockTraverse(pkObj);
	}
	miLockCount--;
	mkDirtyList.clear_record();

//@	if(mkRemoveList.size())
//@	{
//@		for( u32 i = 0; i < mkRemoveList.size(); i++ )
//@		{
//@			iCollisionObject* pkObj = (iCollisionObject*)mkRemoveList[i];
//@			pkObj->addRef();
//@			remObject(pkObj);
//@			if(mpkParent)
//@				mpkParent->addObject(pkObj);
//@			pkObj->release();
//@		}
//@		mkRemoveList.clear_record();
//@	}
}

//@ collision
void cQTreeSpace::onCollide()
{	
	// lock during traversal
	miLockCount++;
	for( u32 i = 0; i < mkDirtyList.size(); i++ )
	{
		iCollisionObject* pkObj = (iCollisionObject*)mkDirtyList[i];

	//	if( pkObj->muFlags & iCollisionObject::IsSpace )
	//		((iCollisionSpace*)pkObj)->onCollide();

		if( pkObj->muFlags & iCollisionObject::BadTransform )
			pkObj->computeTransform();

		if( pkObj->muFlags & iCollisionObject::BadAABB )
			pkObj->computeAABB();
		
	//@
	//@	TODO::
	//@		this is not supposed to take place, but if it
	//@		does, we need to do something about it.
	//@
	//@	if( !X_AABB_INSIDE(mpkRoot,pkObj->mkBBox) )
	//@		mkRemoveList.push_back((array_t::alloc_type)pkObj);

		((Block*)pkObj->mpkSpaceData)->blockTraverse(pkObj);
	}

#if 1
	// this will enable us to collide only moving objects with the rest
	for( u32 i = 0; i < mkDirtyList.size(); i++ )
	{
		iCollisionObject* pkObj = (iCollisionObject*)mkDirtyList[i];
		if(pkObj->muFlags & iCollisionObject::CollideEnabled)
		{
			Block* pkMyBlock = ((Block*)pkObj->mpkSpaceData)->mpkParent;
			// part one collide with the parents
			while(pkMyBlock)
			{
				pkMyBlock->collide(pkObj);
				pkMyBlock = pkMyBlock->mpkParent;
			}
			// part two collide with the childrens
			((Block*)pkObj->mpkSpaceData)->collideChildren(pkObj);
			pkObj->muFlags |= iCollisionObject::MovedCollisionChecked;

		}
	}
	for( u32 i = 0; i < mkDirtyList.size(); i++ )
	{
		iCollisionObject* pkObj = (iCollisionObject*)mkDirtyList[i];
		pkObj->muFlags &= (~(iCollisionObject::BadAABB|
			iCollisionObject::BadTransform|
			iCollisionObject::IsDirty|
			iCollisionObject::MovedCollisionChecked));
	}

	mkDirtyList.clear_record();
#else
	mkDirtyList.clear_record();
	mpkRoot[0].collide();
#endif
	// unlock
	miLockCount--;
}

//@ external collision
void cQTreeSpace::onExternalCollide(nextar::iCollisionObject *pkO)
{
	if( mkDirtyList.size() )
		_cleanObjects();
	// lock during traversal
	miLockCount++;
	X_Assert(pkO->mpkParent != this); // this is an error, only external collisions are handled here
	mpkRoot[0].collideChildren(pkO);
	// unlock
	miLockCount--;
}