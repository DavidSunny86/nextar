//@ Abhishek Dey
//@ Oct 24 2007

#ifndef NEXTAR_COLLISION_SPACE_H
#define NEXTAR_COLLISION_SPACE_H

#include "collision/icollisionobject.h"

namespace nextar
{

class iPortal;
// generic collision space
class iCollisionSpace : public iCollisionObject
{
protected:
	
	s32		miLockCount;

public:

#define xColSpaceIsComplex(ptr) (ptr->muFlags & iCollisionObject::IsComplexSpace)
#define xColObjectIsSpace(ptr) (ptr->muFlags & iCollisionObject::IsSpace)
#define xColSpaceIsNotLocked(ptr) (ptr->miLockCount==0)

	_n_inline void lock() { ++miLockCount; }
	_n_inline void unlock() { --miLockCount; }

	iCollisionSpace(iCollisionSpace* mpkPar,bool bDynamic = false);

	enum { StaticBody = 0x1, BodyInMotion = 0x2 };

	virtual void addObject(iCollisionObject*) = 0;
	virtual void remObject(iCollisionObject*) = 0;
	virtual void addPortal(iPortal*) = 0;
	virtual void moveObject(iCollisionObject*) = 0;
	virtual void onCollide() = 0;
	virtual void onExternalCollide(iCollisionObject*) = 0;
};

}

#endif //NEXTAR_COLLISION_SPACE_H