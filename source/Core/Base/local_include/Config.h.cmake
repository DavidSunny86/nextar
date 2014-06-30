//@ Obi De
//@ Nov 25 2007

#ifndef NEXTAR_USER_CONFIG_1_01
#define NEXTAR_USER_CONFIG_1_01

// Jan 11 08 Modification
#if defined(_MSC_VER)
#	define NEX_MSVC
#	define NEX_COMPILER_NAME	"MS VC++"
#elif defined(__MINGW32__) || defined(__CYGWIN32__) || defined(__GNUG__)
#	define NEX_GCC
#	define NEX_COMPILER_NAME	"GNU G++"
#else
#	define NEX_COMPILER_NAME	"<Unknown>"
#endif

/** Predefined architectures */
#define NEX_ARCH_X86 		0
#define NEX_ARCH_X86_64		1
#define NEX_ARCH_IA64		2
#define NEX_ARCH_PPC		3
#define NEX_ARCH_ALPHA		4

#cmakedefine NEX_CROSS_COMPILE 

#ifdef NEX_CROSS_COMPILE
// Prespecified architecture
	#define NEX_ARCH NEX_ARCH_@NEX_ARCHITECTURE@
// Determine architecture
#endif
#ifndef NEX_ARCH
	#if defined(_M_ALPHA) || defined(__alpha__)
		#define NEX_ARCH NEX_ARCH_ALPHA
	#elif defined(__ia64__) || defined(_M_IA64)
		#define NEX_ARCH NEX_ARCH_IA64
	#elif defined(__x86_64__) || defined(_M_X64) || defined(_AMD64_)
		#define NEX_ARCH NEX_ARCH_X86_64
	#elif defined(__i386__) || defined(_M_IX86) || defined(_X86_)
		#define NEX_ARCH NEX_ARCH_X86
	#elif defined(__powerpc__) || defined(_M_PPC)
		#define NEX_ARCH NEX_ARCH_PPC
	#else
		#error Failed to identify architecture.
	#endif
#endif

#if (NEX_ARCH==NEX_ARCH_IA64) || (NEX_ARCH==NEX_ARCH_X86_64)
#	define NEX_PROCESSOR	64
#else
#	define NEX_PROCESSOR	32
#endif


/** 
 * @remarks Basic configuration file, controlled by CMake variables,
 * Alterable via UI.
 */

#ifdef _NexBaseAPI
#undef	_NexBaseAPI
#endif
#ifdef  max
#undef  max			/* use __max */
#endif
#ifdef  min
#undef  min			/*	use __min */
#endif

// define _NexBaseAPI
#if defined(_MSC_VER)
#	define _NexExportSym			__declspec(dllexport)
#	define _NexImportSym			__declspec(dllimport)
#elif defined(__MINGW32__)
#	define _NexExportSym			__attribute__ ((visibility("default")))
#	define _NexImportSym			__attribute__ ((visibility("default")))
#elif defined(NEX_GCC)
#   if (__GNUC__ >= 4)
#	define _NexExportSym			__attribute__ ((visibility("default")))
#	define _NexImportSym			__attribute__ ((visibility("default")))
#   else
#	error Dont know how to define _NexExportSym,_NexImportSym and NEX_EXPORT_TEMPLATE!!
#   endif
#endif


#ifdef NEXTAR_EXPORT_AS_DYNAMIC_LIB
#	ifdef @NEXTAR_LIBRARY_DEFINE@_EXPORTS
#		define _NexBaseAPI	_NexExportSym
#	else
#		define _NexBaseAPI	_NexImportSym
#	endif
#else
#	define _NexBaseAPI
#endif

// try to determine platform if no specified
#if !defined(NEX_WINDOWS) && !defined(NEX_LINUX) && !defined(NEX_MACOSX)
#if defined( _WIN32_ ) || defined ( _WIN32 ) || defined ( _WIN64 )
#	define NEX_WINDOWS 1
#elif defined(__unix__)
#   define NEX_LINUX 1
#else
#	error Platform undefined. Define a basic platform (for example. use #define NEX_WINDOWS)
#endif
#endif

//##===================================================================================================
// Main engine options
//##===================================================================================================

// Debug flag 
#cmakedefine NEX_DEBUG

// store data in files as little by default?
#cmakedefine NEX_STORE_IN_LITTLEENDIN 1

/* Use ned-allocator */
#cmakedefine NEX_USE_PTMALLOC
/* In order to support multithreading statically this flag is set. */
#cmakedefine NEX_SUPPORT_MULTITHREADING 
/* Use thread building blocks. */
#cmakedefine NEX_USE_TBB

/* Endianness */
#define NEX_ENDIAN_LITTLE   0
#define NEX_ENDIAN_BIG      1
#define NEX_ENDIANNESS NEX_ENDIAN_@NEX_ENDIAN@

/** Turn on bug reporting, exception handling and memory tracker **/
#ifdef NEX_DEBUG
#	define NEX_MEMTRACKERLEVEL	3	// set any level form { 1,2,3 }
#	define NEX_USE_ASSERTION    1
#else
#	define NEX_MEMTRACKERLEVEL	0	// set any level form { 1,2,3 }
#	define NEX_USE_ASSERTION    0
#endif

/** Version modification **/
#define NEX_VER_MAJOR		@NEX_VER_MAJOR@
#define NEX_VER_MINOR		@NEX_VER_MINOR@
#define NEX_VER_REVISION	@NEX_VER_REVISION@
#define NEX_VERSION_STRING()	"@NEX_VER_MAJOR@.@NEX_VER_MINOR@.@NEX_VER_REVISION@"

//##===================================================================================================
//##===================================================================================================

/**
 * Predefined constants
 * =======================================================
 */
/** any name (event or class) 120 characters. **/
#define NEX_MAX_NAME_LENGTH                         160
/** config var name length **/
#define NEX_MAX_VAR_NAME_LENGTH                     256
/** max path length **/
#define NEX_MAX_PATH                                256

#ifdef NEX_MEMTRACKERLEVEL
#	if	NEX_MEMTRACKERLEVEL == 1
#		define NEX_TRACK_OBJECTS
#	elif NEX_MEMTRACKERLEVEL == 2
#		define NEX_SIMPLE_FAST_MEM_TRACKER
#		define NEX_TRACK_OBJECTS
#	elif	NEX_MEMTRACKERLEVEL >= 3
// @urgent Memory tracking can only be switched on when the memory tracker static object and the class
// has been implemented
// #		define NEX_EXTENSIVE_MEM_TRACKER
#		define NEX_TRACK_OBJECTS
#	endif
#endif


#if defined(NEX_USE_ASM)
#   define NEX_ASM_LEVEL NEX_USE_ASM
#else
#   define NEX_ASM_LEVEL 0
#endif
#define NEX_EXTERN_SYM	extern

#ifdef __cplusplus
#	define NEX_EXTERNC_SYM	extern "C"
#else
#	define NEX_EXTERNC_SYM	extern
#endif

// wide char defined 
#ifdef _MSC_VER 
#	ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#		define _WCHAR_T_DEFINED
#	endif
#endif

#ifdef NEX_MSVC
#if	_MSC_VER >= 1400
#	include <intrin.h>
#endif
#	define inline __inline
#	if !defined( NEX_DEBUG ) && defined( NEX_USE_INTRINSICS )
#		if defined( NEX_ARCH_X86 )
#			pragma intrinsic (memset, memcpy, memcmp)
#			pragma intrinsic (strcpy, strcmp, strlen, strcat)
#			pragma intrinsic (abs)
#			pragma intrinsic (_byteswap_ushort, _byteswap_ulong, _byteswap_uint64)
#		else
#			undef	NEX_USE_INTRINSICS
#		endif
#	endif
#endif

#if (defined(_MSC_VER) && (_MSC_VER <= 1700)) || defined(__MINGW32__)
#	define noexcept
#	define constexpr const
	// no need to explicitly align as first member is aligned anyway
#	define alignas(x)	
#endif

#if defined( NEX_MSVC ) && !defined(NEX_DONOT_USE_NOVTABLE)
#	define _NexNoVtable __declspec(novtable)
#else
#	define _NexNoVtable 
#endif 

// macro to test if its debug mode
#ifdef NEX_DEBUG
#	define NEX_IS_DEBUG_MODE() true
#else
#	define NEX_IS_DEBUG_MODE() false
#endif

#ifdef NEX_MSVC
//#	pragma	warning(disable : 4250 4275 4355 4251)
#	pragma	warning(disable : 4275 4355 4251 4661 4800)
#endif

/** Helper macros **/
#define NEX_MAKE_VERSION(maj,min,rev) (VersionID)((maj << 24) | (min << 16) | rev)
#define NEX_GET_VER_MAJOR(ver)		(ver >> 24)
#define NEX_GET_VER_MINOR(ver)		((ver >> 16) & 0x000000ff)
#define NEX_GET_VER_REVISION(ver)	((ver) & 0x0000ffff)
#define NEX_GET_VER_MINREV(ver)		((ver) & 0x00ffffff)
#define NEX_VERSION()				NEX_MAKE_VERSION(NEX_VER_MAJOR,NEX_VER_MINOR,NEX_VER_REVISION)
#define NEX_MAKE_TEXT( __t__ ) #__t__

#ifdef NEX_DEBUG
#	include <typeinfo>
#	define NEX_TYPE_ID(ptr)		typeid(ptr)
#	define NEX_TYPE_NAME(ptr)	typeid(ptr).name()
#else
#	define NEX_TYPE_ID(ptr)		0
#	define NEX_TYPE_NAME(ptr)	"<unknown>"
#endif

#if defined(NEX_GCC) && defined(NEX_DEBUG)
#	include <cxxabi.h>
#	define NEX_CLEAN_TYPE_NAME(ptr, status)	abi::__cxa_demangle(typeid(ptr).name(), 0, 0, &status)
#else
#	define NEX_CLEAN_TYPE_NAME(ptr, status)	typeid(ptr).name(); status
#endif


#if defined(_MSC_VER) && _MSC_VER > 1200
#	define NEX_ALIGN_MEMBER( _member_, _align_ )	__declspec(align(_align_))	_member_
#	define NEX_EFFICIENT_ALIGN(_member_)			__declspec(align(16))		_member_
#	define NEX_EFFICIENT_TYPEDEF( _align_num_, _def_ )	typedef __declspec(align(_align_num_)) _def_
#elif  __GNUC__
#	define NEX_ALIGN_MEMBER( _member_, _align_ )	_member_ __attribute((aligned(_align_)))
#	define NEX_EFFICIENT_ALIGN(_member_)			_member_ __attribute((aligned(16)))
#	define NEX_EFFICIENT_TYPEDEF( _align_num_, _def_ )	typedef _def_ __attribute((aligned(_align_num_)))
#else
#	define NEX_ALIGN_MEMBER( _member_, _align_ )	_member_
#	define NEX_EFFICIENT_ALIGN(_member_)			_member_
#	define NEX_EFFICIENT_TYPEDEF( _align_num_, _def_ )	typedef _def_
#	pragma message("Alignment: Alignments unsupported. Please see note.")
#endif

#if !defined( NEX_ENDIAN_LITTLE ) || !defined( NEX_ENDIAN_BIG )
# error Required constants missing.
#endif

#ifndef NEX_ENDIANNESS
#	error Define endian-ness. Use NEX_ENDIAN_LITTLE or NEX_ENDIAN_BIG
#endif

#if !defined(NEX_STORE_IN_LITTLEENDIN) 
#	define NEX_STORE_IN_LITTLEENDIN 1
#endif

//#include <ErrorDefs.h>
//#include <nexshared.h>

// helper macros
//#define NEX_MULTIPLE_OF(_number_,n) (_number_ + (n-_number_%n))
#define NEX_TOKEN_PASTE(X,Y)	NEX_TOKEN_PASTE1(X,Y)
#define NEX_TOKEN_PASTE1(X,Y)	X##Y
#define _NexInline inline

//*****************************************************************************
// Compiler specific options
//*****************************************************************************
#if defined( NEX_MSVC )
#	define NEX_FUNCTION_NAME		__FUNCTION__
#	define NEX_SOURCEFILE_NAME		__FILE__
#	define NEX_SOURCEFILE_LINE		__LINE__
#elif defined( NEX_GCC )
#	define NEX_FUNCTION_NAME		__FUNCTION__
#	define NEX_SOURCEFILE_NAME		__FILE__
#	define NEX_SOURCEFILE_LINE		__LINE__
#else
#	define NEX_FUNCTION_NAME		"<Unresolved>"
#	define NEX_SOURCEFILE_NAME		"<Unknown>"
#	define NEX_SOURCEFILE_LINE		0
#endif

//*****************************************************************************
// more macros
#define NEX_4LETTER_KEY(a,b,c,d) \
	((long)a + ((long)b << 8) + ((long)c << 16) + ((long)d << 24))

#if defined(NEX_DEBUG) && defined(NEX_USE_TBB)
#	define TBB_USE_DEBUG	1
#endif

#endif //NEXTAR_USER_CONFIG_1_01
