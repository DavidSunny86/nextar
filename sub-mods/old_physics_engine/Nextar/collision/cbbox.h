//@ Abhishek Dey
//@ Oct 24 2007
#ifndef IORIENTED_BOX_H
#define IORIENTED_BOX_H

#include <icollisionobject.h>

namespace nextar
{
class iCollisionSpace;
struct cBBox : public iCollisionObject
{
	f32*	mkHalfSide;
	cBBox(iCollisionSpace*,f32 fX, f32 fY, f32 fZ);
	virtual ~cBBox();
	f32 pointDepth(const f32* p);
	virtual void computeAABB();
};

}

#endif