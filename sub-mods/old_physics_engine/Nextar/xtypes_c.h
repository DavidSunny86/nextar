/*VARCHECK*/
/*FILE MARK
 DOC ORIGINAL NAME:	xtypes_c.h
******************************************************************************************************
 AUTHOR:	Abhishek Dey
 CREATED:	Aug 03 07
END
******************************************************************************************************
*/

// this file shud only contain typedefs
// they r not contained within nextar namespace
#ifndef __XTYPES_C_H__
#define __XTYPES_C_H__
#include <tchar.h>
#include <xbase.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef char				s8;
typedef	short				s16;
typedef	long				s32;
typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned long		u32;
typedef float				f32;
typedef double				f64;
typedef __int64				s64;
typedef unsigned __int64	u64;
typedef u8					byte;
typedef u32					pi_id;
typedef u32					pi_version;
/* REVISION: 28 jan 07: check def of TCHAR in tchar.h for info */
typedef TCHAR				tchar;
/* REVISION: 17 feb 07 */
typedef void*				h32; /* handles */
#define XISVALIDHANDLE(h)	(h!=NULL)
//REVISION:	1/1/2007		
//typedef struct { s32 high; s32 low; } interfaceId;
/* similar to f32 equals, in case time is changed to u32 it */
/* can be modified. Time value will be in seconds if f32 is maintained */
typedef X_TIME_TYPE			x_time;
// allocate memory
X_Api void*	allocMemory( size_t size );			  // alloc 
X_Api void*	allocMemoryAligned(size_t size);		  // aligned alloc
// free only if allocated by allocMemory
X_Api void	freeMemory( void* ptr );	// free
X_Api void	freeMemoryAligned(void* ptr); // aligned free
// realloc: returns a valid ptr
X_Api void* reallocMemory(void* cur,size_t newsize);
X_Api void* reallocMemoryAligned(void* cur,size_t newsize);
#ifdef __cplusplus
}
#endif

#endif //__XTYPES_C_H__