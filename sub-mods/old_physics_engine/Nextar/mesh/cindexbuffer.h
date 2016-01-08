//@ Abhishek Dey
//@ Nov 22 2007

#ifndef XNEXTAR_INDEX_BUFFER
#define XNEXTAR_INDEX_BUFFER

#include "xtypes.h"

namespace nextar
{

struct cIndexBuffer
{
	union Tri {
		struct {
			u16 _1,_2,_3;
		};
		u16		i[3];
	};
	u16*	mpuData;
	s32		miCount;

	// functions:
	// for standard buffers these are valid
	_n_inline Tri*	getTri(s32 i) { X_Assert(i>=0 && i<miCount); return (((Tri*)mpuData)+i); }
};

}
#endif //XNEXTAR_INDEX_BUFFER