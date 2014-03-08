//@ Obi De
//@ Nov 25 2007

#ifndef NEXTAR_USER_CONFIG_1_01
#define NEXTAR_USER_CONFIG_1_01

/* Debug flag */
#define NEX_DEBUG

// store data in files as little by default?
#define NEX_STORE_IN_LITTLEENDIN 1

/* Use ned-allocator */
/* #undef NEX_USE_PTMALLOC */
/* In order to support multithreading statically this flag is set. */
#define NEX_SUPPORT_MULTITHREADING 
/* Use thread building blocks. */
/* #undef NEX_USE_TBB */

/* Endianness */
#define NEX_ENDIAN_LITTLE   0
#define NEX_ENDIAN_BIG      1
#define NEX_ENDIANNESS NEX_ENDIAN_LITTLE

/** Turn on bug reporting, exception handling and memory tracker **/
#ifdef NEX_DEBUG
#	define NEX_MEMTRACKERLEVEL	3	// set any level form { 1,2,3 }
#	define NEX_USE_ASSERTION    1
#else
#	define NEX_MEMTRACKERLEVEL	0	// set any level form { 1,2,3 }
#	define NEX_USE_ASSERTION    0
#endif

/** Version modification **/
#define NEX_VER_MAJOR		1
#define NEX_VER_MINOR		0
#define NEX_VER_REVISION	3
#define NEX_VERSION_STRING()	"1.0.3"

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
