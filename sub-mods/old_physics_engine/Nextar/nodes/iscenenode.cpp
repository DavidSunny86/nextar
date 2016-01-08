//@ Abhishek Dey
//@ Oct 21 2007
//@ Nov 02 2007

#include <iscenenode.h>
#include <ispace.h>

using namespace nextar;

// ctor
iEntity::iEntity() : mpkParent(0),muFlags(0)
{
}

// dtor
iEntity::~iEntity()
{
	// dont do anything, we
	// expect the child was removed 
	// from its parent.
}

// a call to set parent
// ensures that the parent adds
// this node as its child.
void iEntity::setParent(iSpace* pkP)
{
	if(mpkParent != pkP)
	{
		remParent();
		if(pkP)
		{
			pkP->addChildNode(this);
			mpkParent = pkP;
		}
	}
}

// a call to rem parent
// ensures that the child is 
// removed from the parent
void iEntity::remParent()
{
	if(mpkParent)
	{
		mpkParent->remChildNode(this);
		mpkParent = 0;
	}
}
