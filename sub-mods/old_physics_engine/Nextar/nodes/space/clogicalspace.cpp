//@ Abhishek Dey
//@ Oct 26 2007

#include "clogicalspace.h"
#include <icamera.h>

using namespace nextar;

cLogicalSpace::cLogicalSpace(nextar::iSpace *pkParent)
{
	muFlags = Conceptual|IsSpace;
	if(pkParent) setParent(pkParent);
}

cLogicalSpace::~cLogicalSpace()
{
	for(list_t<iEntity*>::iterator it=mkChildren.begin(); it.ok(); ++it)
		(*it)->release();
}

void cLogicalSpace::addChildNode(nextar::iEntity *pkNode)
{
	if(pkNode->getParent() == this) 
		return;
	mkChildren.push_back(pkNode);
	pkNode->addRef();
}

void cLogicalSpace::remChildNode(nextar::iEntity *pkNode)
{
	X_Assert(pkNode->getParent() == this);
	mkChildren.remove(pkNode);
	pkNode->release();
}

void cLogicalSpace::onUpdate(nextar::iCamera *pkCam)
{
	for(list_t<iEntity*>::iterator it=mkChildren.begin(); it.ok(); ++it)
		(*it)->onUpdate(pkCam);
}