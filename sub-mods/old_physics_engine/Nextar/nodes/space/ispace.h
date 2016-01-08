//@ Abhishek Dey
//@ Oct 20 2007

#ifndef X_SPACE_H
#define X_SPACE_H

#include <iobject.h>
#include <iscenenode.h>
#include <list_t.h>
#include <array_t.h>

namespace nextar
{

//@ iSpace is a conceptual space that can have objects group together.
//@ Use of it will vary, it can be used as a switch to start rendering from.
//@ Importantly it can contain portals

class iPortal;
class iSpace : public iEntity
{
protected:
	enum { 
		SubTreeDisabled = iEntity::LastFlag << 0,
		Conceptual = iEntity::LastFlag << 1,
	};
public:
	
	virtual void addChildNode(iEntity*) = 0;
	virtual void remChildNode(iEntity*) = 0;

protected:
};

}

#endif //X_SPACE_H