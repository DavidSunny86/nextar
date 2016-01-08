/*VARCHECK*/
/*FILE MARK
 DOC ORIGINAL NAME:	XTypes.h
******************************************************************************************************
 AUTHOR:	Abhishek Dey
 CREATED:	10/28/2006		AT:		7:32:45 AM
 REVISION:	1/22/2007		AT:		4:16:37 PM
END
******************************************************************************************************
*/
#ifndef __XTYPES_H__
#define __XTYPES_H__

#include <xbase.h>
#include <xtypes_c.h>

namespace nextar
{

enum XERRORCODE
{
	X_Err_Index = -1,
	X_SUCCESS = 0,
	X_ERR_FATAL,
	X_ERR_WARNING,
	X_ERR_OUTOFMEMORY,
	// engine 
	X_ERR_COULDNTCRTWINDOW,
	// file 
	X_ERR_CANNOTOPEN,
	X_ERR_WRONGHEADER,
	X_ERR_CORRUPTED,
	X_ERR_SOME_NOT_LOADED,
	X_ERR_COULDNT_LOAD,
	X_ERR_INVALID_FILE,
	// thread specific 
	X_ERR_THREAD_TIMEOUT,
	// renderer
	X_ERR_COULDNT_CREATE_D3D,
	X_ERR_COULDNT_CREATE_D3DDEV,

};

struct cBaseClass
{
	// new
	_n_inline void* operator new( size_t uSize )
	{	return allocMemory(uSize); }
	// delete
	_n_inline void operator delete(void* ptr)
	{ freeMemory(ptr); }
	// new []
	_n_inline void* operator new[]( size_t uSize )
	{	return allocMemory(uSize); }
	// delete []
	_n_inline void operator delete[](void* ptr)
	{ freeMemory(ptr); }
};

struct cAlignedBaseClass
{
	// new
	_n_inline void* operator new( size_t uSize )
	{	return allocMemoryAligned(uSize); }
	// delete
	_n_inline void operator delete(void* ptr)
	{ freeMemoryAligned(ptr); }
	// new []
	_n_inline void* operator new[]( size_t uSize )
	{	return allocMemoryAligned(uSize); }
	// delete []
	_n_inline void operator delete[](void* ptr)
	{ freeMemoryAligned(ptr); }
};

}

#endif //__XTYPES_H__