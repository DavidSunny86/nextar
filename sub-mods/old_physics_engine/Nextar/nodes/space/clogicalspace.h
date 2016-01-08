
//@ Abhishek Dey
//@ Oct 26 2007

#ifndef X_LOGICALSPACE_H
#define X_LOGICALSPACE_H

#include <ispace.h>
#include <list_t.h>

namespace nextar
{

//@ Logical spaces are meant to group other
//@ spaces, collision or logical. This simplifies
//@ space traversals.
class cLogicalSpace : public iSpace
{
public:
	cLogicalSpace(iSpace* pkParent = 0);
	~cLogicalSpace();

	virtual void addChildNode(iEntity*);
	virtual void remChildNode(iEntity*);
	virtual void onUpdate(iCamera*);
protected:
	list_t<iEntity*>	mkChildren;
};

}

#endif //X_LOGICALSPACE_H