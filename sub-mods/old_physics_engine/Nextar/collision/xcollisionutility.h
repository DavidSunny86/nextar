//@ Abhishek Dey
//@ Oct 24 2007
//@ Oct 25 2007

#ifndef XCOLLISION_UTILITY_H
#define XCOLLISION_UTILITY_H

#include <icollisionobject.h>
#include <xcontact.h>

namespace nextar
{
namespace collision
{

extern cContact* gpkContactHolder;
void xInitializeColliders();
void xCollideObjects(iCollisionObject*,iCollisionObject*);

// colliders
s32 xColSpace(iCollisionObject* o1,iCollisionObject* o2);
s32 xColBoxBox(iCollisionObject* o1,iCollisionObject* o2);
s32 xColBoxPlane(iCollisionObject* o1,iCollisionObject* o2);
}
}

#endif //XCOLLISION_UTILITY_H