//@ Abhishek Dey
//@ Oct 25 2007

#include "nodes/ccube.h"
#include "camera/icamera.h"
#include "irenderer.h"
#include "graphics/imaterial.h"
#include "cengine.h"
#include "physics/csimulator.h"

using namespace nextar;

cCube::cCube(iSpace* pkParent,f32 x,f32 y,f32 z,f32 density) : cBBox(0,x,y,z),mpkBaseMaterial(0)
{
	X_Assert( x > 0.f  && y > 0.f && z > 0.f);
	mpkImplementor = this;
	// before adding to parent, just add the body
	attach(X_SharedPtr(cSimulator)->addBody());
	if(mpkBody) mpkBody->setCubeMass(density,x,y,z);
	if(pkParent)
	{
		pkParent->addChildNode(this);
		iEntity::mpkParent = pkParent;
	}
	//! now the object is both in a collision space
	//! and a logical space (or may be a collision space)
	
	//	 0----1
	//	/|	 /|
	// 2-7--3 6
	// |/	|/
	// 4----5
	x *= .5f;	
	y *= .5f;
	z *= .5f;
	vec3_t pos[8];
	vec3_t norm[8];
	u16 idx24[] = {
		2,2,2,3,3,3,5,5,5,4,4,4,0,0,0,1,1,1,6,6,6,7,7,7
	};
	u16 idx36[] = {
		9,0,6,6,0,3,
		7,4,18,18,4,15,
		19,16,21,21,16,12,
		22,13,10,10,13,1,
		2,14,5,5,14,17,
		23,11,20,20,11,8
	};
	// set up default texture mapping
	for(s32 i=0; i<6; i++)
	{
		X_SetV2(mkVertices[idx36[i*6+0]].mkTexCoord,0,1);
		X_SetV2(mkVertices[idx36[i*6+1]].mkTexCoord,0,0);
		X_SetV2(mkVertices[idx36[i*6+2]].mkTexCoord,1,1);
		X_SetV2(mkVertices[idx36[i*6+5]].mkTexCoord,1,0);
	}
	//@ initialize vertices
	X_SetV3(pos[0],-x, y, z);
	X_SetV3(pos[1], x, y, z);
	X_SetV3(pos[2],-x, y,-z);
	X_SetV3(pos[3], x, y,-z);
	X_SetV3(pos[4],-x,-y,-z);
	X_SetV3(pos[5], x,-y,-z);
	X_SetV3(pos[6], x,-y, z);
	X_SetV3(pos[7],-x,-y, z);
	for(s32 i=0; i<24; i++)
		mkVertices[i].mkPosition = pos[idx24[i]];
	// set up normals
	f32 val = sqrtr(x*x + y*y + z*z);
	x /= val; y /= val; z /= val;
	X_SetV3(norm[0],-x, y, z);
	X_SetV3(norm[1], x, y, z);
	X_SetV3(norm[2],-x, y,-z);
	X_SetV3(norm[3], x, y,-z);
	X_SetV3(norm[4],-x,-y,-z);
	X_SetV3(norm[5], x,-y,-z);
	X_SetV3(norm[6], x,-y, z);
	X_SetV3(norm[7],-x,-y, z);
	for(s32 i=0; i<24; i++)
		mkVertices[i].mkNormal = norm[idx24[i]];
	for( s32 i=0; i<24; i++)
		mkVertices[i].miColor = -1;
	for( s32 i=0; i<12; i++)
	{
		muIndices[i*3 + 0]=idx36[i*3 + 0];
		muIndices[i*3 + 1]=idx36[i*3 + 1];
		muIndices[i*3 + 2]=idx36[i*3 + 2];
	}
}

cCube::~cCube()
{
	detach();
	remParent();
	if(mpkBaseMaterial) 
		mpkBaseMaterial->release();
}

f32 cCube::getDistanceFromView(f32 *viewpoint)
{
	X_Assert( !(iCollisionObject::muFlags & iCollisionObject::BadTransform) );
	return xVDotR(mpkWorld->pos,viewpoint);
}

void cCube::setMaterial(iMaterial* m)
{
	if(mpkBaseMaterial)
		mpkBaseMaterial->release();
	mpkBaseMaterial = m;
	if(mpkBaseMaterial)
		mpkBaseMaterial->addRef();
}

void cCube::onUpdate(nextar::iCamera *pkCam)
{
#ifdef X_USE_FRUSTUM_MASKING
	if( 
		_InternalCullObject(pkCam->getTestPlanes(),
							pkCam->getSignFields(),
							pkCam->getPlaneCount(),
							pkCam->getMask()) != iCamera::ObjectCulled)
#else
	if( _InternalCullObject(pkCam->getTestPlanes(),
							pkCam->getSignFields(),
							pkCam->getPlaneCount()) != iCamera::ObjectCulled )

#endif
	{
		X_SharedPtr(cEngine)->registerDrawNode(this,iEntity::RenderSolid);
	}
}

void cCube::setColor(s32 c)
{
	for(s32 i=0; i<24; i++)
		mkVertices[i].miColor = c;
}

void cCube::onDraw()
{
	// first draw the thing
	if(mpkBaseMaterial)
	{
		if(isEnabled())
			setColor(0xffffffff);
		else
			setColor(0xff111111);

		X_SharedPtr(iRenderer)->setMaterial(mpkBaseMaterial);
	}
	f32* mat = (f32*)mpkWorld;
	mat[15] = 1;
//	mat44_t m = {
//	mat[0],mat[2],mat[1],0,
///	mat[4],mat[6],mat[5],0,
//	mat[8],mat[10],mat[9],0,
//	mat[12],mat[14],mat[13],1
//	};
	X_SharedPtr(iRenderer)->setWorld(mat);
	mat[15] = 0;
	X_SharedPtr(iRenderer)->setVertexShader(VERTEX_STD);
	X_SharedPtr(iRenderer)->drawIndexedTriangleList(mkVertices,muIndices,24,12,sizeof(cVertexStd));
}