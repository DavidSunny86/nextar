//@ Obi De
//@ Nov 25 2007

#ifndef NEXTAR_USER_CONFIG_1_01
#define NEXTAR_USER_CONFIG_1_01

/* Debug flag */
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

#endif //NEXTAR_USER_CONFIG_1_01
