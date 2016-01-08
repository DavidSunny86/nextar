//@ Abhishek Dey
//@ 12/30/2006
//@ 1/2/2007
//@ Sept 1 2007


#include "ifilesystem.h"

namespace nextar
{

SharedSingleInterfaceSetup(iFileSystem);

iFileSystem::iFileSystem()
{
	SharedSingleInterfaceInit(iFileSystem);
}

iFileSystem::~iFileSystem()
{
	SharedSingleInterfaceExit(iFileSystem);
}

}