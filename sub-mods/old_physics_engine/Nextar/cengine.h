//@ Abhishek Dey
//@ May 30 2007
//@ September 23 2007

#ifndef NEXTAR_SCENEMANAGER_H
#define NEXTAR_SCENEMANAGER_H

#include "iobject.h"
#include "list_t.h"
#include "array_t.h"
#include "camera/icamera.h"
#include "nodes/iscenenode.h"
#include "icontroller.h"
#include "nodes/space/ispace.h"
#include "collision/icollisionspace.h"
#include "graphics/clight.h"

namespace nextar
{

class X_Api cEngine : public iObject
{
protected:

	SharedSingleInterface( cEngine );

private:

	// draw classes
	array_t					mkInitDraw;
	array_t					mkSkyDraw;
	array_t					mkRoomDraw;
	array_t					mkSolidDraw;
	array_t					mkSolidSortedDraw;
	array_t					mkTransparentDraw;
	array_t					mkTransparentSortedDraw;
	array_t					mkFinalDraw;
	array_t					mkHalos;

public:

	cEngine(const tchar* pkConfig = 0);
	virtual ~cEngine();

	//:: UTILITY functions
	//:: WORLD traversals;
	void wUpdate();
	//@ the draw call (internally called by the system)
	//@ draws all that is present
	//@ in the scene
	void wDraw();
	//:: register object for rendering,
	// this is called by the rendering nodes
	void registerDrawNode(iEntity*,iEntity::RenderPriorityClass);
	//:: register a light node to use
	//:: with rendering, this is currently
	//:: very basic code
	void addHalo(cLight*);
	//:: create nodes and spaces
	//@ create a quad tree collision space
	//@ Quadtree is a complex space that can 
	//@ be used to represent outdoor geometry
	iSpace*	crtQuadTreeSpace(iSpace* pkSpaceParent,const vec3_t &kCenter,f32 fXExt,f32 fZExt,s32 iDepth);
	//@ create a logical space to group multiple
	//@ complex spaces (collision or logical)
	iSpace*	crtGenericLogicalSpace(iSpace* pkParent);
	//@ create a ccube debug object
	//@ provide a valid material for rendering
	iEntity*	crtCube(iSpace* pkParent,iMaterial* pkM,f32 sx,f32 sy,f32 sz,f32 density=5.0f);
	//@ create a groundplane debug object
	//@ provide a valid material for rendering
	iEntity*	crtGroundPlane(iSpace* pkParent,iMaterial* pkM,s32 tiletexture = 1);
	//@ create a basic light
	//@ this is a very simple light model
	iEntity* crtBasicLight(iSpace* pkParent,cLight::Type t = cLight::Point,const f32* direction = 0,f32 radius=100.f,u32 color = 0xffffffff);
	//@ create a box collision object
	iCollisionObject* crtOrientedBox(iCollisionSpace* pkParent,f32 sx,f32 sy,f32 sz);
	//@ create a fps camera object, with active input
	//@ handling
	iCamera* crtFPSCamera(iSpace* pkAttachTo);

	//:: setup the world
	//@ set up the active camera and call
	//@ the onActivated callback of the camera
	void setActiveCamera(iCamera*);
	//@ set up the active collision space region
	//@ collision detection is performed for only
	//@ this region.
	void setActiveCollisionRegion(iCollisionSpace*);
	//@ set up the far value for the world,
	//@ this will affect the far clipping plane
	_n_inline void setWorldFarDistance(f32 f) { mfFarDistance = f; }

	//:: retrieve value form the world
	//@ return the farthest corner 
	_n_inline f32  getWorldFarDistance() { return mfFarDistance; }
	//@ return the world root. the world root 
	//@ is automatically created and it is a 
	//@ generic logical space unless otherwise 
	//@ specified.
	_n_inline iSpace* getRootSpace() { return mpkRoot; }
	//@ return the active collision region
	//@ the region must be set up for collision
	//@ detection.
	_n_inline iCollisionSpace* getActiveCollisionRegion() { return mpkActiveCollisionSpace; }
	//@ return the current active camera
	//@ the camera must be set up beforehand
	_n_inline iCamera* getActiveCamera() { return mpkActiveCamera; }

protected:
	f32						mfFarDistance;
	iCamera*				mpkActiveCamera;
	iCollisionSpace*		mpkActiveCollisionSpace;
	iSpace*					mpkRoot;
	list_t<iController*>		mkControllers;
};

}

#endif //NEXTAR_SCENEMANAGER_H