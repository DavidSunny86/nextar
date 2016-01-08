/*VARCHECK*/
/*FILE MARK
 DOC ORIGINAL NAME:	XConfig.h
******************************************************************************************************
 AUTHOR:	Abhishek Dey
 CREATED:	10/28/2006		AT:		6:18:05 AM
END
******************************************************************************************************
*/

#ifndef X_CONFIG_H
#define X_CONFIG_H
/******************************************************************************
./ debug flag substitute
******************************************************************************/
#ifdef _DEBUG
#	define X_DEBUG
#endif

/******************************************************************************
./ user breaks
******************************************************************************/
#ifdef X_DEBUG
#		define X_BreakOutIf( __condition__ ) if( __condition__ ) { X_BREAK }
#	ifdef _MSC_VER
#		define X_BREAK		{__asm int 3}
#	elif defined(__GNUC__)
#		define X_BREAK		asm volatile {"int 3"}
#	else
#		define X_BREAK
#	endif
#else
#	define X_BREAK
#	define X_BreakOutIf( __condition__ )
#endif

#define X_EXPORT_LIB_AS_DLLS
#define X_FORCE_INLINE			1
/******************************************************************************
./ X_KEEP_TRACK_OF_OBJECTS objects reference counted can be kept 
./ track of using this flag
******************************************************************************/
#define X_KEEP_TRACK_OF_OBJECTS

/******************************************************************************
./ Assembly codes will become active.
******************************************************************************/
#define X_USE_ASM_ALTERNATIVES	2
/* Assemly alternative levels : */
/* 0 - Assembly codes are disabled */
/* 1 - Assembly codes which are presumably safe are only enabled */
/* 2 - Assembly codes that might face compatibility issue are active */
/* ( however this is recommended for LITTLEENDIAN architecture ) */
/* 3 - Its preferrable not to use this level */

//@......................................................
//@ collision detection:: use this to specify that certain
//@ collision primitives can extend to infinity
#define X_COLLISION_USING_INFINTE_EXTENDS_FOR_PRIMITIVES
//@......................................................
//@ include support for unrealistic but fast response
//@ contact constrains (the NPConstrains), engine however
//@ prefers the frictional constrains.
#define NEXTAR_INCLUDE_NON_PENETRATION_CONSTRAINS
//#define NEXTAR_USE_NP_CONSTRAINS	// this enables or makes the engine prefer NP constrains
/******************************************************************************
./ X_USE_BUG_REPORTING
******************************************************************************/
#ifdef X_DEBUG
#	define X_USE_BUG_REPORTING
#endif

/******************************************************************************
./ Enable SSE 
******************************************************************************/
#define X_SSE_INCLUDE

/******************************************************************************
./ For some architecture floating point intrinsics are available, 
./ enable this flag to use them
******************************************************************************/
#define X_USE_FLOATINGPT_INTRIN
/******************************************************************************
./ version definition
******************************************************************************/
#define X_MODULE_NAME			"nextar"
#define X_VERSION_STRING		"0.0.1"
#define X_NEXTAR_FULLNAME		"nextar v0.0.1"
#define X_VERSION_MAJOR		0
#define X_VERSION_MINOR		0
#define X_VERSION_REVISION	1
#define X_VERSION_REAL		1.0f

#ifdef _MSC_VER
/******************************************************************************
./ Disable warnings
******************************************************************************/
#	pragma warning( disable : 4251 4800 4275 4231 )
#endif

/******************************************************************************
./ the following definition can be used to implement coriolis force calculation
./ during simulation. Disabling it will speed up things a bit.
******************************************************************************/
//#define X_XPEGYROSCOPICFORCES

/******************************************************************************
./ the following definition can be used during debug mode to generate msgs in console.
******************************************************************************/
//#define XALLOW_TRACE_MSG

/******************************************/
/* IMPORTANT:: Specify the iXPlatform here.*/
/*	X_WIN32 = Win32 */

#define X_WIN32		1
#define X_PLATFORM	X_WIN32
// internal
/******************************************/
/* Default values */
#define X_FS_EL_NAME	X_Text("FileSystem")
#define X_WIN_EL_NAME	X_Text("Window")
#define X_WIN_EL_ATT1	X_Text("Width")
#define X_WIN_EL_ATT2	X_Text("Height")
#define X_WIN_EL_ATT3	X_Text("FullScreen")

// Window related
#define X_DEFAULT_WIN_WIDTH		640
#define X_DEFAULT_WIN_HEIGHT	480
#define X_DEFAULT_BITDEPTH		32
#define X_DEFAULT_RENDER_FLAGS	(iRenderer::StencilBuffer|iRenderer::AntiAlias|iRenderer::HardwareVP)
#define X_FULLSCRENN			false

//@ 
#define X_DEFAULT_WORLD_FAR		1000.0f
#endif //X_CONFIG_H