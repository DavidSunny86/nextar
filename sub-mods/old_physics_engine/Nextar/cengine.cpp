
#include "cengine.h"
#include "ctimer.h"
#include "irenderer.h"
#include "physics/csimulator.h"
#include "cxuimanager.h"
#include "cinputsystem.h"
#include "nodes/space/cqtreespace.h"
#include "nodes/space/clogicalspace.h"
#include "nodes/ccube.h"
#include "nodes/cgroundplane.h"
#include "nodes/cbasiclight.h"
#include "collision/cbbox.h"
#include "camera/cfpscamera.h"
#include "collision/xcollisionutility.h"


using namespace nextar;

// sorting of nodes done here
template<typename T>
void xHeapsort(T *data,u32 count)
{
	/* heap sort */
	register s32 i,s,f;
	register T	elt;
	register T  ivalue;
	
	/* creating heap */
	for( i = 1; i < (s32)count; i++ )
	{
		elt = data[i];
		/* pqinsert */
		s = i;
		f = (s-1)/2;
		while( s > 0 && data[f] < elt )
		{
			data[s] = data[f];
			s = f;
			f = (s-1)/2;
		} /* end while */
		data[s] = elt;
	} /* end for */
	
	/* selection phase : select data[0] and find it its proper position */
	for( i = (s32)count-1; i > 0; i-- )
	{
		/* pqmaxdelete */
		ivalue = data[i];
		data[i] = data[0];
		f = 0;
		/* s = largeson( 0, i-1 ) */
		if( i == 1 )
			s = -1;
		else
			s = 1;
		if( i > 2 && data[1] < data[2]  )
			s = 2;
		while( s >= 0 && ivalue < data[s] ) 
		{
			data[f] = data[s];
			f = s;
			/* s = largeson( f, i-1 ) */
			s = 2*f + 1;
			if( s+1 <= i-1 && data[s] < data[s+1] )
				s = s+1;
			if( s > i-1 )
				s = -1;
		}/* end while */
		data[f] = ivalue;
	}/* end for */
}

struct cSortedTransparentNode
{
	iEntity* rend;
	f32 farval;
};
struct cSortedSolidNode
{
	iEntity* rend;
	void* tex;
};
_n_inline bool operator < (const cSortedTransparentNode& o1,const cSortedTransparentNode& o2) { return o1.farval < o2.farval; }
_n_inline bool operator < (const cSortedSolidNode& o1,const cSortedSolidNode& o2) { return o1.tex < o2.tex; }

SharedSingleInterfaceSetup(cEngine);

cEngine::cEngine(const tchar* sConf) : 
mfFarDistance(X_DEFAULT_WORLD_FAR), mpkActiveCamera(0),
mpkActiveCollisionSpace(0), mpkRoot(0)
{
	RefObjectInit(cEngine);
	SharedSingleInterfaceInit(cEngine);

	collision::xInitializeColliders();
	//@ default initialization
	mpkRoot = crtGenericLogicalSpace(0);
}

cEngine::~cEngine()
{
	if(mpkActiveCamera) mpkActiveCamera->release();
	if(mpkActiveCollisionSpace) mpkActiveCollisionSpace->release();
	if(mpkRoot) mpkRoot->release();
	SharedSingleInterfaceExit(cEngine);
}

void cEngine::wUpdate()
{
	//@ a rough update algo
	//@ * update all controllers
	//@ * update the collision tree
	//@ * update the physics
	//@ * get the active camera space
	//@		* recursively traverse the space
	//@		* end of traversal
	//@	* update the ui manager
	f32 dt = timer_t::skAsynchTimer.getElapsedTime();
	X_SharedPtr(cInputSystem)->updateAllDevice();
	X_SharedPtr(cInputSystem)->updateReceivers();
	for(list_t<iController*>::iterator it = mkControllers.begin(); it != mkControllers.end(); ++it)
		(*it)->update(dt);
	if(mpkActiveCollisionSpace)
	{
		mpkActiveCollisionSpace->onCollide();
	}
	// we should update it after a fixed time 10 times per second is ok
	X_SharedPtr(cSimulator)->setGravity(0,-0.5f,0);
	X_SharedPtr(cSimulator)->wSimulate(0.05f);
	//X_SharedPtr(cSimulator)->wSimulate(dt);
	X_Assert(mpkActiveCamera);
	iSpace* pkS = mpkActiveCamera->getParent();
	pkS->onUpdate(mpkActiveCamera);
}

void cEngine::wDraw()
{
	// init draw
	X_Assert(mpkActiveCamera);
	mpkActiveCamera->onCameraUpdate();
	for(u32 i=0; i<mkInitDraw.size(); i++)
		((iEntity*)mkInitDraw[i])->onDraw();
	// draw 3d solids
	X_SharedPtr(iRenderer)->setMode(RSC_3D);
	// add dynamic lights
	for(u32 i=0; i<mkHalos.size(); i++)
		X_SharedPtr(iRenderer)->addDynamicLight((cLight*)mkHalos[i]);
	X_SharedPtr(iRenderer)->finalizeLights();
	mkHalos.clear_record();
	for(u32 i=0; i<mkSolidDraw.size(); i++)
		((iEntity*)mkSolidDraw[i])->onDraw();
	mkSolidDraw.clear_record();
	// for the sake of debugging draw a coordinate system
#if defined( X_DEBUG ) && defined( DRAW_WORLD_TRANSFORM_COORDS )
	X_SharedPtr(iRenderer)->setWorld(N3D4x4IdentityMatrix);
	X_SharedPtr(iRenderer)->draw3DTransform(N3D4x4IdentityMatrix,200);
#endif
	// draw the 2d objects
	X_SharedPtr(iRenderer)->setMode(RSC_2D);
	X_SharedPtr(cXuiManager)->onDraw();
}

void cEngine::addHalo(cLight* l)
{
	mkHalos.push_back(array_t::alloc_type(l));
}

void cEngine::registerDrawNode(iEntity *node, iEntity::RenderPriorityClass c)
{
	switch(c)
	{
	case iEntity::RenderInit:
		mkInitDraw.push_back(array_t::alloc_type(node));
		break;
	case iEntity::RenderSky:
		mkSkyDraw.push_back(array_t::alloc_type(node));
		break;
	case iEntity::RenderRoom:
		mkRoomDraw.push_back(array_t::alloc_type(node));
		break;
	case iEntity::RenderSolid:
		mkSolidDraw.push_back(array_t::alloc_type(node));
		break;
	case iEntity::RenderSolidSort:
		mkSolidSortedDraw.push_back(array_t::alloc_type(node));
		{
			iTexture* t = 0;
			iMaterial* m = ((iSolidSortedNode*)node)->getBaseMaterial();
			if(m) t = m->getType();
			mkSolidSortedDraw.push_back(array_t::alloc_type(t));
		}
		break;
	case iEntity::RenderTransparent:
		mkTransparentDraw.push_back(array_t::alloc_type(node));
		break;
	case iEntity::RenderTransparentSort:
		X_Assert(mpkActiveCamera);
		mkTransparentSortedDraw.push_back(array_t::alloc_type(node));
		{
			f32 fardist = ((iSolidSortedNode*)node)->getDistanceFromView(mpkActiveCamera->getContext()->mkLocation);
			mkSolidSortedDraw.push_back(*(array_t::alloc_type*)(&fardist));
		}
		break;
	case iEntity::RenderFinal:
		mkFinalDraw.push_back(array_t::alloc_type(node));
		break;
	}
}

iSpace* cEngine::crtQuadTreeSpace(nextar::iSpace *pkSpaceParent, const vec3_t &kCenter, f32 fXExt, f32 fZExt, s32 iDepth)
{
	return new cQTreeSpace(pkSpaceParent,kCenter,fXExt,fZExt,iDepth);
}

iSpace* cEngine::crtGenericLogicalSpace(nextar::iSpace *pkParent)
{
	return new cLogicalSpace(pkParent);
}

iEntity* cEngine::crtCube(nextar::iSpace *pkParent, nextar::iMaterial *pkM, f32 sx, f32 sy, f32 sz,f32 density)
{
	cCube* c = new cCube(pkParent,sx,sy,sz,density);
	if(pkM) c->setMaterial(pkM);
	return c;
}

iEntity* cEngine::crtGroundPlane(nextar::iSpace *pkParent, nextar::iMaterial *pkM,s32 tile)
{
	cGroundPlane* c = new cGroundPlane(pkParent);
	if(pkM) c->setMaterial(pkM);
	c->tileTexture(tile);
	return c;
}

iEntity* cEngine::crtBasicLight(iSpace* pkParent,cLight::Type type,const f32* dir,f32 rad,u32 color)
{
	return new cBasicLight(pkParent,type,dir,rad,color);
}

iCollisionObject* cEngine::crtOrientedBox(nextar::iCollisionSpace *pkParent,f32 sx,f32 sy,f32 sz)
{
	return new cBBox(pkParent,sx,sy,sz);
}

iCamera* cEngine::crtFPSCamera(nextar::iSpace *pkAttachTo)
{
	iCamera* cam = new cFPSCamera();
	cam->setParent(pkAttachTo);
	return cam;
}

void cEngine::setActiveCamera(nextar::iCamera *pkCam)
{
	if(mpkActiveCamera) 
	{
		mpkActiveCamera->onDeactivated();
		mpkActiveCamera->release();
	}
		
	mpkActiveCamera = pkCam;
	if(mpkActiveCamera)
	{
		mpkActiveCamera->addRef();
		mpkActiveCamera->onActivated();
	}
}

void cEngine::setActiveCollisionRegion(nextar::iCollisionSpace *pkC)
{
	if(mpkActiveCollisionSpace)
		mpkActiveCollisionSpace->release();
	mpkActiveCollisionSpace = pkC;
	if(mpkActiveCollisionSpace)
		mpkActiveCollisionSpace->addRef();
}
