
//@ Abhishek Dey
//@ September 01 2007

#include <isystem.h>
#include <xio.h>

#ifdef X_WIN32
#	include <csystemwin32.h>
#	define NEW_SYSCLASS(__conf__)	new cSystemWin32(__conf__)
#endif

#ifndef NEW_SYSCLASS
#	define NEW_SYSCLASS(__conf__) 0
#endif

using namespace nextar;

SharedSingleInterfaceSetup(iSystem);

//@ constructor
iSystem::iSystem() : mkLogger(X_Text("xrun.log"),true)
{
	SharedSingleInterfaceInit(iSystem);
	mkVersion = X_VERSION_STRING;
}

//@ dtor
iSystem::~iSystem()
{
	// we r almost done....
	// if we have object lists check for memory leaks
#ifdef X_KEEP_TRACK_OF_OBJECTS
	u32 uCnt = iObject::getExistingCount();
	if(uCnt>1) 
	{
		// we have a memory leak and we need to handle it
		printf("\nSerious Error in Deallocation: Number of Objects Existing = %u",uCnt);
		printf("\nPrinting Locations");
		iObject* obj = iObject::getHead();
		while(obj)
		{
			if(obj!=this)
				printf("\nObject[0x%x] , Name[%s], RefCount[%d].", 
				(u32)obj, obj->getDebugName(), obj->getRefCount());
			obj = obj->nextObject();
		}
		printf("\nForcing Delete Operation...");
		obj = iObject::getHead();
		while(obj)
		{
			iObject* n = obj->nextObject();
			obj->deleteThis();
			obj = n;
		}
		printf("\nDone...");
	}
#endif
	SharedSingleInterfaceExit(iSystem);
}

void iSystem::getRealFilePath(tchar* sName) const
{
	while(*sName)
	{	
		if( (s32)(*sName) == N3DPathSeparator )
			*sName = (tchar)miPathSeparator;
		else if( (s32)(*sName) == N3DFileSeparator )
			*sName = (tchar)miFileSeparator;
		sName++;
	}
}

void iSystem::getNextarFilePath(tchar* sName) const
{
	X_Assert(sName);
	if( miPathSeparator != miFileSeparator )
	{
		while(*sName)
		{	
			if( (s32)(*sName) == miPathSeparator )
				*sName = (tchar)N3DPathSeparator;
			if( (s32)(*sName) == miFileSeparator )
				*sName = (tchar)N3DFileSeparator;
			sName++;
		}
	}
	else
	{
		tchar* sLastIdx = 0;
		while(*sName)
		{	
			if( (s32)(*sName) == miPathSeparator )
			{
				*sName = (tchar)N3DPathSeparator;
				sLastIdx = sName;
			}
			sName++;
		}
		if(sLastIdx) *sLastIdx = (tchar)N3DFileSeparator;
	}
}

// create 
iSystem* iSystem::newSystem(const tchar* sConfig)
{
	return NEW_SYSCLASS(sConfig);
}
