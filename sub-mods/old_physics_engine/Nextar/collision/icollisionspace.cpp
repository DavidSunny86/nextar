
#include "icollisionspace.h"

using namespace nextar;

iCollisionSpace::iCollisionSpace(iCollisionSpace *pkPar, bool bDynamic) : 
iCollisionObject(pkPar,bDynamic),miLockCount(0)
{
}