//@ Abhishek Dey
//@ Oct 21 2007
//@ Nov 09 2007

#include "cphybody.h"
#include "collision/icollisionobject.h"
#include "xmemory.h"
#include "physics/csimulator.h"

using namespace nextar;

cPhyBody::cPhyBody() : muFlags(Enabled|AutoDisable),mpkNext(0),mpkPrev(0),mpkObjects(0)
{
	storeDwords(mkTmp,8,0);
	nRSetIdentity(mkInvI);
	nRSetIdentity(mkInvIWorld);
	nRSetIdentity(mkWorld.rot);
	storeDwords(mkWorld.pos,4,0);
	mkQ[QW] = 1;mkQ[QA] = 0;mkQ[QB] = 0;mkQ[QC] = 0;
	storeDwords(mkPdot,4,0);
	storeDwords(mkOmega,4,0);
	storeDwords(mkAext,8,0);
	mfMass = 1.0f;
	mfInvMass = 1.0f;
	mtTimeToIdle = 100.0f;
}

cPhyBody::~cPhyBody()
{
}

void cPhyBody::enable()
{
	//@ cant afford redundant calls
	X_Assert(!(muFlags & Enabled));
	muFlags |= Enabled;
	iCollisionObject* start = mpkObjects;
	while(start) {
		start->enableCollision();
		start = start->mpkBodyNext;
	}
}

void cPhyBody::disable()
{
	X_Assert((muFlags & Enabled));
	muFlags &=~Enabled;
	iCollisionObject* start = mpkObjects;
	while(start) {
		start->disableCollision();
		start = start->mpkBodyNext;
	}
}

void cPhyBody::setPosition(f32 x,f32 y,f32 z)
{
	mkWorld.pos[0] = x;
	mkWorld.pos[1] = y;
	mkWorld.pos[2] = z;
	iCollisionObject* start = mpkObjects;
	while(start) {
		start->validateMove();
		start = start->mpkBodyNext;
	}
	if(isDisabled())
		X_SharedPtr(cSimulator)->enableBody(this);
}

void cPhyBody::setRotationEuler(f32 yaw,f32 pitch,f32 roll)
{
	nQuatFromEulerAngles(mkQ,yaw,pitch,roll);
	nQuatToR(mkWorld.rot,mkQ);
	iCollisionObject* start = mpkObjects;
	while(start) {
		start->enableCollision();
		start->validateMove();
		start = start->mpkBodyNext;
	}
	if(isDisabled())
		X_SharedPtr(cSimulator)->enableBody(this);
}

void cPhyBody::setRotationAxisAngle(f32 ex,f32 ey,f32 ez,f32 v)
{
	vec4_t axis = {ex,ey,ez,0};
	xQ4Normalize(axis);
	nQuatFromAxisAngle(mkQ,axis,v);
	nQuatToR(mkWorld.rot,mkQ);
	iCollisionObject* start = mpkObjects;
	while(start) {
		start->enableCollision();
		start->validateMove();
		start = start->mpkBodyNext;
	}
	if(isDisabled())
		X_SharedPtr(cSimulator)->enableBody(this);
}

void cPhyBody::setCubeMass(f32 density, f32 lx, f32 ly, f32 lz)
{
	mfDensity = density;
	mfMass = lx*ly*lz*density;
	mfInvMass = 1/mfMass;
	mat34_t I = {
		mfMass/12.0f * (ly*ly + lz*lz),0,0,0,
		0,mfMass/12.0f * (lx*lx + lz*lz),0,0,
		0,0,mfMass/12.0f * (lx*lx + ly*ly),0
	};
	// take its inverse
	nM12Inverse(mkInvI,I);
}