//@ Abhishek Dey
//@ Oct 24 2007

#ifndef NEXTAR_IPORTAL_H
#define NEXTAR_IPORTAL_H

#include <iobject.h>

namespace nextar
{

class iSpace;
class iPortal : public iCollisionObject
{
public:
public:

	_n_inline iSpace*		from() { return mpkFrom; }
	_n_inline iSpace*		to() { return mpkTo; }

	virtual void		onCollisionCallback(iCollisionObject*);
	virtual vec4_t*		getPolyList(u32 &nCnt) = 0;
protected:
	iSpace*	mpkFrom;
	iSpace*	mpkTo;
};

}

#endif //NEXTAR_IPORTAL_H