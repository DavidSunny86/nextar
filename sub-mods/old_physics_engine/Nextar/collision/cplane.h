//@ Abhishek Dey
//@ Nov 07 2007
#ifndef NEXTAR_COLLISION_PLANE
#define NEXTAR_COLLISION_PLANE

#include <icollisionobject.h>

namespace nextar
{
class iCollisionSpace;
struct cPlane : public iCollisionObject
{
	f32*	mkPlane;
	cPlane(iCollisionSpace*,f32 fnX, f32 fnY, f32 fnZ,f32 fnD);
	virtual ~cPlane();
	virtual void computeAABB();
};

}


#endif //NEXTAR_COLLISION_PLANE