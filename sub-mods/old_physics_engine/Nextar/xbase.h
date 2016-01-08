/*VARCHECK*/
/*FILE MARK
 DOC ORIGINAL NAME:	XBase.h
******************************************************************************************************
 AUTHOR:	Abhishek Dey
 CREATED:	10/28/2006		AT:		7:32:45 AM
 END
******************************************************************************************************
*/
//@ Nov 24, 07 (updated xshared.h)

#ifndef __XBASE_H__
#define __XBASE_H__

#include <math.h>
#include <stdlib.h>
#include <tchar.h>
#include <xconfig.h>

/* include platform specifics from configs */
#ifdef X_WIN32
/* include windows.h */
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	include <windows.h>
#else
/* do something else */
#endif

// portability issues
// to make the library 
// 64 bit portable, some
// errors need to be
// rectified... check out 
// the warnings 
#ifndef X_64BIT_PORTABLE
#pragma warning(disable: 4311)
#pragma warning(disable: 4312)
#endif

#ifdef  max
#undef  max			/* use __max */
#endif
#ifdef  min
#undef  min			/*	use __min */
#endif

#undef	X_Api
#undef	XTEMPLATEEXPORT
#undef	IMPL_TEMPLATE_EXPORT

/* platform specs */
#if !defined( X_WIN32 ) && !defined( __APPLE__ ) && !defined( POSIX ) && !defined( XBOX )
/* default platform */
#	define POSIX	
#endif

/* ASM ALTERNATIVE */
#if !defined( X_USE_ASM_ALTERNATIVES )
#	define X_USE_ASM_ALTERNATIVES 1
	/* default to level one */
#endif

#define X_Api
#define XTEMPLATEEXPORT
#define IMPL_TEMPLATE_EXPORT(__template__)	

/*	Architecture Specs */
/* Flags, compatible with all compilers
/? X_X86	: X86 model
/? X_PPC	: Power PC
/? X_MMX	: MultiMedia Xtension
/? X_ALPHA	: DEC ALPHA platform
*/

#ifdef _MSC_VER

#	ifdef _M_IX86
#	define X_X86	_M_IX86
#	endif

#	ifdef _M_MPPC
#	define X_PPC	_M_MPPC
#	endif

#	ifdef _M_ALPHA
#	define X_ALPHA	_M_ALPHA
#	endif

#endif

/*	Architecture Specs */
#if defined(X_WIN32)
/*	Only x86 supported */
#	ifndef X_X86
#		define X_X86
#	endif

#	ifdef X_PPC
#		undef X_PPC
#	endif

#	ifdef	_MSC_VER

#	undef	X_Api
#	undef	XTEMPLATEEXPORT
#	undef	IMPL_TEMPLATE_EXPORT

#		if defined(X_EXPORT_LIB_AS_DLLS)
#			ifdef NEXTAR_EXPORTS
#				define X_Api						__declspec(dllexport)
#				define XTEMPLATEEXPORT
#			else
#				define X_Api						__declspec(dllimport)
#				define XTEMPLATEEXPORT			extern
#			endif
#		else
#			define X_Api
#			define XTEMPLATEEXPORT
#		endif
#		ifdef __cplusplus
#			define IMPL_TEMPLATE_EXPORT(__template__)	XTEMPLATEEXPORT template class  __template__;
#		endif

#	elif	defined(__MINGW32__)

#	undef	X_Api
#	undef	XTEMPLATEEXPORT
#	undef	IMPL_TEMPLATE_EXPORT

#		if defined(X_EXPORT_LIB_AS_DLLS) && X_EXPORT_LIB_AS_DLLS == 1
#			ifdef X_EXPORT
#				define X_Api						__declspec(dllexport)
#				define XTEMPLATEEXPORT
#			else
#				define X_Api						__declspec(dllimport)
#				define XTEMPLATEEXPORT			extern
#			endif
#		else
#			define X_Api
#			define XTEMPLATEEXPORT
#		endif
#		ifdef __cplusplus
#			define IMPL_TEMPLATE_EXPORT(__template__)	XTEMPLATEEXPORT template class  __template__;
#		endif
#	else
/*	Post Error */
#	endif	/*_MSC_VER,__MINGW32__*/

#elif defined(__APPLE__)
/*	Only PPC supported	*/

#	ifdef X_X86
#		undef X_X86
#	endif

#	ifndef X_PPC
#		define X_PPC
#	endif

#endif

/* default architecture */
#if !defined(X_X86) && !defined(X_PPC)
#	define X_X86
#endif

/* wide char defined */
#ifdef _MSC_VER 
#	ifndef _WCHAR_T_DEFINED
		typedef unsigned short wchar_t;
#		define _WCHAR_T_DEFINED
#	endif
#endif

/* alignment macro */
#if defined(_MSC_VER) && _MSC_VER > 1200
#	define X_ALIGN_MEMBER( _member_, _align_ )	__declspec(align(_align_))	_member_
#	define X_EFFICIENT_ALIGN(_member_)			__declspec(align(16))		_member_
#	define X_EFFICIENT_TYPEDEF( _def_ )			typedef __declspec(align(16)) _def_
#elif  __GNUC__
#	define X_ALIGN_MEMBER( _member_, _align_ )	_member_ __attribute((aligned(_align_)))
#	define X_EFFICIENT_ALIGN(_member_)			_member_ __attribute((aligned(16)))
#	define X_EFFICIENT_TYPEDEF( _def_ )			typedef _def_ __attribute((aligned(16)))
#else
#	define X_ALIGN_MEMBER( _member_, _align_ )	_member_
#	define X_EFFICIENT_ALIGN(_member_)			_member_
#	define X_EFFICIENT_TYPEDEF( _def_ )			typedef _def_
#	pragma message("X_ALIGN_MEMBER: Alignments unsupported. Please see note")
#endif


#ifdef _MSC_VER
/* f32 to s32 cast : for performance */
/* considering that f32 to s32 cast causes */
/* a call to __ftol function consider using this instead */
#define X_FLOAT_TO_INT_INTPTR(intptr, floatval) \
	__asm fld		floatval	\
	__asm mov		edx, intptr	\
	__asm frndint	\
	__asm fistp	dword ptr[edx]	\
/* params are intptr and floatval */
/* intptr is the pointer to the integer ptr */ 
/* to store the f32 value 'floatval' */

#define X_FLOAT_TO_INT_INTVAR(intvar, floatval)  \
	__asm fld		floatval	\
	__asm fistp		intvar	\
/* params are intvar and floatval */
/* intvar is the integer variable  */ 
/* to store the f32 value 'floatval' */

#else

/* f32 to s32 cast : for performance */
/* considering that f32 to s32 cast causes */
/* a call to __ftol function consider using this instead */
#define X_FLOAT_TO_INT_INTPTR(intptr, floatval) *intptr = (s32)floatval; /* ugly casting */
/* params are intptr and floatval */
/* intptr is the pointer to the integer ptr */ 
/* to store the f32 value 'floatval' */
#define X_FLOAT_TO_INT_INTVAR(intvar, floatval)  intvar = (s32)floatval	/* ugly casting */
/* params are intvar and floatval */
/* intvar is the integer variable  */ 
/* to store the f32 value 'floatval' */

#endif

/* f32 to byte case : performance intensive */
#define X_FLOAT_TO_BYTE_CAST(bytevar, floatvar) \
	floatvar += (f32) 0xC00000; \
	bytevar = *(byte*) &floatvar;
/* params are bytevar and floatval */
/* bytevar is the unsigned 8 bit variable  */ 
/* to store the f32 value 'floatval', a variable this time */

/* Note: For _MSC_VER <= 1200 use the pragma (pack) directive to align */
/* elements to specefied boundaries */

#define _n_noinline
/* inline specs	*/
#if	defined( X_FORCE_INLINE ) && ( X_FORCE_INLINE == 1 )
#	ifdef _MSC_VER
#		define _n_inline __forceinline
#	elif __GNUC__
#		define _n_inline inline __attribute__((alwaysinline))
#	else
#		define _n_inline __inline
#	endif
#endif

/* report bug */
#if defined( X_USE_BUG_REPORTING ) && defined( __cplusplus )
#	include "xbugreport.h"
/* bug reporting */
#	define ReportBug( __string__, __funcname__ ) nextar::ReportBugDlg( __string__, __funcname__, X_MODULE_NAME, __FILE__, __LINE__ );
/* to avoid empty control statemets */
#	define ReportBugIf( __condition__, __string__, __funcname__ ) if( __condition__ ) ReportBug(__string__,__funcname__);
#	define X_Assert( __condition__ ) if(!(__condition__)) { X_BREAK ReportBug(X_Text("Assertion Failed!"),X_Text("Unknown")); }
#else
#	define ReportBug( __string__, __funcname__ )
#	define ReportBugIf( __condition__, __string__, __funcname__ ) 
#	define X_Assert( __condition__ )
#endif

//? some more debug macros
#ifdef X_DEBUG
#define X_ONDEBUG_DO( __cond__, __what__ ) if(__cond__) __what__
#else
#define X_ONDEBUG_DO( __cond__, __what__ )
#endif

#ifdef _MSC_VER
#	if _MSC_VER > 1200
#		define XALIGN_STRUCT __declspec(align(16))
#	else
#		define XALIGN_STRUCT
#	endif
#elif defined( __GNUC__ )
#	define XALIGN_STRUCT	__attribute__(aligned(16))
#else
#	error "Unsupported compiler"
#endif

/******************************************************************************
./ SafeDelete
******************************************************************************/
#define SafeDelete( memptr ) if( memptr ) { delete memptr; memptr = 0; }
#define SafeDeleteBlock( memptr ) if( memptr ) { delete [] memptr; memptr = 0; }
#define SafeRelease( memptr ) if( memptr ) { memptr->Release(); memptr = 0; }

/*Time releated */

/*------------------------------------------------------------------------------------------------
/.useful macros
/.----------------------------------------------------------------------------------------------*/
#define FORM16( a, b ) ( ((u16) a << 8) | (u16) b )
#define LOWOF16( a )  ( (u16)( a & 0x00ff) )
#define HIGHOF16( a ) ( (u16)( a >> 8) )
#define LOWOF32( a ) ( a & 0x0000ffff )
#define HIGHOF32( a ) ( a >> 16 )
#define CLAMP( val, min, max ) val = (val<min ? min : val>max ? max : val)

// ? the macro is very reliable for member definition in
// ? classes requiring faster access.
#ifdef X_DEBUG
// ? for debug purposes this type of addition 
// ? is done even where faster access is required
#define GET_MEMBER( __prefix__, __name__ ) get##__name__##()
#define SET_MEMBER( __prefix__, __name__, __value__ ) set##__name__##(__value__)
#define ADD_MEMBER( __type__, __prefix__, __name__ ) \
	private: __type__ __prefix__##__name__; \
	public: _n_inline const __type__& get##__name__##() { return __prefix__##__name__; } \
	public: _n_inline void set##__name__##(const __type__& t) { __prefix__##__name__ = t; }
#else
// ? however the definition changes for retail version.
#define GET_MEMBER( __prefix__, __name__ ) __prefix__##__name__
#define SET_MEMBER( __prefix__, __name__, __value__ ) __prefix__##__name__ = __value__
#define ADD_MEMBER( __type__, __prefix__, __name__ ) \
	public: __type__ __prefix__##__name__; \

#endif //X_DEBUG

#ifdef X_DEBUG
// STDOUT Tracer: enclose the parameters in bracket else error will be generated
#	ifdef XALLOW_TRACE_MSG
#		include <stdio.h>
#		define XTRACE(__TextMsg__)	_tprintf##__TextMsg__
#	else
#		define XTRACE(__TextMsg__) 
#	endif
#else
#		define XTRACE(__TextMsg__) 
#endif

#define X_DEFINE_C_STRUCT(name) extern "C" { struct tag_##name {
#define X_END_DEFINE_C_STRUCT(name)  } typedef struct tag_##name name; } // "C"
// define the x time data type
// for physics and animation the suitable unit is f32
#define X_TIME_TYPE		f32
#include <xshared.h>
#endif //__XBASE_H__