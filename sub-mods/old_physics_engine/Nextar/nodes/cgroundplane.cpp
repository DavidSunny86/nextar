//@ Abhishek Dey
//@ Nov 10 2007

#include "nodes/cgroundplane.h"
#include "camera/icamera.h"
#include "irenderer.h"
#include "graphics/imaterial.h"
#include "cengine.h"

using namespace nextar;

cGroundPlane::cGroundPlane(iSpace* pkParent) : cPlane(0,0,1,0,0),mpkBaseMaterial(0)
{
	mpkImplementor = this;
	if(pkParent)
	{
		pkParent->addChildNode(this);
		iEntity::mpkParent = pkParent;
	}
	//! now the object is both in a collision space
	//! and a logical space (or may be a collision space)
	// 1----3 
	// |	|
	// 0----2
	// set up default texture mapping
	X_SetV2(mkVertices[0].mkTexCoord,0,1);
	X_SetV2(mkVertices[1].mkTexCoord,0,0);
	X_SetV2(mkVertices[2].mkTexCoord,1,1);
	X_SetV2(mkVertices[3].mkTexCoord,1,0);
	//@ initialize vertices
#define Extends 80
	X_SetV3(mkVertices[0].mkPosition,-Extends,0,-Extends);
	X_SetV3(mkVertices[1].mkPosition,-Extends,0, Extends);
	X_SetV3(mkVertices[2].mkPosition, Extends,0,-Extends);
	X_SetV3(mkVertices[3].mkPosition, Extends,0, Extends);
#undef Extends
	// set up normals
	for( s32 i=0; i<4; i++)
		mkVertices[i].miColor = -1;
	muIndices[0] = 0;
	muIndices[1] = 1;
	muIndices[2] = 2;
	muIndices[3] = 2;
	muIndices[4] = 1;
	muIndices[5] = 3;
}

cGroundPlane::~cGroundPlane()
{
	remParent();
	if(mpkBaseMaterial) 
		mpkBaseMaterial->release();
}

f32 cGroundPlane::getDistanceFromView(f32 *viewpoint)
{
	X_Assert( !(iCollisionObject::muFlags & iCollisionObject::BadTransform) );
	return xVDotR(mpkWorld->pos,viewpoint);
}

void cGroundPlane::setMaterial(iMaterial* m)
{
	if(mpkBaseMaterial)
		mpkBaseMaterial->release();
	mpkBaseMaterial = m;
	if(mpkBaseMaterial)
		mpkBaseMaterial->addRef();
}

void cGroundPlane::tileTexture(s32 numtimes)
{
	f32 tile = (f32)numtimes;
	X_SetV2(mkVertices[0].mkTexCoord,0,tile);
	X_SetV2(mkVertices[1].mkTexCoord,0,0);
	X_SetV2(mkVertices[2].mkTexCoord,tile,tile);
	X_SetV2(mkVertices[3].mkTexCoord,tile,0);
}

void cGroundPlane::onUpdate(nextar::iCamera *pkCam)
{
	X_SharedPtr(cEngine)->registerDrawNode(this,iEntity::RenderSolid);
}

void cGroundPlane::onDraw()
{
	// first draw the thing
	if(mpkBaseMaterial)
		X_SharedPtr(iRenderer)->setMaterial(mpkBaseMaterial);
	X_SharedPtr(iRenderer)->setWorld(N3D4x4IdentityMatrix);
	X_SharedPtr(iRenderer)->setVertexShader(VERTEX_STD);
	X_SharedPtr(iRenderer)->drawIndexedTriangleList(mkVertices,muIndices,4,2,sizeof(cVertexStd));
}
