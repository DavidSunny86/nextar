//@ Abhishek Dey
//@ Nov 07 2007
#include "cplane.h"

using namespace nextar;

cPlane::cPlane(nextar::iCollisionSpace *pkCol, f32 fnX, f32 fnY, f32 fnZ, f32 fnD) : iCollisionObject(pkCol,false)
{
	muClass = Col_Plane;
	mkPlane = (f32*)allocMemoryAligned(sizeof(plane_t));
	mkPlane[PNA] = fnX;
	mkPlane[PNB] = fnY;
	mkPlane[PNC] = fnZ;
	mkPlane[PND] = fnD;
	xPNormalize(mkPlane);
}

cPlane::~cPlane()
{
	freeMemoryAligned((void*)mkPlane);
}

void cPlane::computeAABB()
{
	mkBBox.min.x =-N3DInfinity;
	mkBBox.min.y =-N3DInfinity;
	mkBBox.min.z =-N3DInfinity;
	mkBBox.max.x = N3DInfinity;
	mkBBox.max.y = N3DInfinity;
	mkBBox.max.z = N3DInfinity;
}
