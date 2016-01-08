//@ Abhishek Dey
//@ Nov 24, 07

#ifndef X_GENMESH_HEADER_H
#define X_GENMESH_HEADER_H
#include "xfile_s.h"

#undef ALIGN_STRUCT
#ifdef _MSC_VER
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define ALIGN_STRUCT
#elif defined( __GNUC__ )
#	define ALIGN_STRUCT	__attribute__(aligned(1))
#else
#	error "Unsupported compiler"
#endif

struct cMeshFile
{
	// -- v 1.01 mesh file
	char	identifier
};

#undef ALIGN_STRUCT
#ifdef _MSC_VER
#	pragma pack( pop, packing )
#	endif
#endif

#endif //X_GENMESH_HEADER_H