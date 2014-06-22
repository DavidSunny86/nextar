//@ Obi De
//@ Nov 25 2007

#ifndef NEXTAR_ERR_DFS_1_01
#define NEXTAR_ERR_DFS_1_01

#include <NexAssert.h>

#ifdef NEX_DEBUG
#	if (NEX_ARCH == NEX_ARCH_X86)
#		if defined(NEX_MSVC)
#			define NEX_DEBUG_BREAK()	__asm int 3
#		elif defined(NEX_GCC)
#			define NEX_DEBUG_BREAK()	__asm__ volatile ("int3")
#		else
#			define NEX_DEBUG_BREAK()
#		endif
#	else
#		if defined(NEX_MSVC)
#			define NEX_DEBUG_BREAK()	__debugbreak()
#		elif defined(NEX_GCC)
#			define NEX_DEBUG_BREAK()	__asm__ volatile ("int3")
#		else
#			define NEX_DEBUG_BREAK()
#		endif
#	endif
#else
#	define NEX_DEBUG_BREAK()
#endif

#ifdef NEX_DEBUG
#define NEX_DEBUG_DO( __what__ )	__what__
#define NEX_DEBUG_DO_IF( __cond__, __what__ ) if(__cond__) __what__
#else
#define NEX_DEBUG_DO( __what__ )
#define NEX_DEBUG_DO_IF( __cond__, __what__ )
#endif

namespace nextar {

enum {
	NEX_SUCCESS = 0
};

/*! \enum KnownErrors
 *
 *   @brief The error definitions.
 *
 *   The most common error definitions. However other error codes are acceptable.
 *   These codes may be handled by deafault handler. These codes are used inside
 *   nexter.
 */
enum KnownError {
	EXCEPT_UNKNOWN = 0xf01,
	EXCEPT_BAD_CAST,
	EXCEPT_INVALID_ARGS,
	EXCEPT_INVALID_CALL,
	EXCEPT_OUT_OF_MEMORY,
	EXCEPT_IO,
	EXCEPT_RECURSIVE_LOCK_TO_NONRECURSIVE_MUTEX,
	EXCEPT_LOCKING_PROTOCOL_REQUIRED_FOR_SAFETY,
	EXCEPT_DELETE_CALLED_ON_BAD_MEM,
	EXCEPT_MEMORY_LEAK,
	EXCEPT_NOT_IMPLEMENTED,
	EXCEPT_COMPILATION_FAILED,
	EXCEPT_RESOLUTION_FAILED,
	EXCEPT_CAN_NOT_OPEN_FILE,
	EXCEPT_LOCK_ERROR,
	EXCEPT_OUT_OF_RESOURCES,
	EXCEPT_DEREFERENCED_BELLOW_0,
	EXCEPT_FAILED_TO_LOADLIB,
	EXCEPT_EVENT_ID_CLASH,
	EXCEPT_READ_ERROR,
	EXCEPT_WRITE_ERROR,
	EXCEPT_INCOMPATIBLE_LIBRARY,
	EXCEPT_APPSTARTUP_FAILED,
	EXCEPT_APPINIT_FAILED,
	EXCEPT_APPBEGIN_FAILED,
	EXCEPT_APPRUN_FAILED,
	EXCEPT_FAILED_TO_OPEN_WINDOW,
	EXCEPT_REQUIRED_LIB_MISSING,
	EXCEPT_GENERIC_FUNCTION_FALIURE,
	EXCEPT_DEVICE_CREATION_FAILED,
	EXCEPT_FAILED_TO_CREATE_OBJECT,
	EXCEPT_COULD_NOT_LOAD_ASSET,
	EXCEPT_COULD_NOT_LOCATE_ASSET,
	EXCEPT_ASSERTION_FAILED,
	EXCEPT_FAILED_TO_START_THREAD,
	EXCEPT_THREAD_MUTEX_CREATION,
	EXCEPT_MALFORMED_ENCODING,
	EXCEPT_MISSING_PLUGIN,
};

}

#include <NexException.h>

#endif //NEXTAR_ERR_DFS_1_01