//@ Abhishek Dey
//@ Nov 22 2007

#ifndef XNEXTAR_VERTEX_BUFFER
#define XNEXTAR_VERTEX_BUFFER

#include "xtypes.h"
#include "math/xmath_c.h"

namespace nextar
{

struct cVertexBuffer
{
	enum DType {
		Vec3,
		Vec4,
		Vec2,
		Dword
	};

	void*	mpvData;
	s32		miStride;
	s32		miCount;
	u32		muData;	// driver specific data

	// functions:
	void  initialize(u32 type,DType*,s32 numdt, s32 count);
	_n_inline void*	getVertex(s32 i) { X_Assert(i>=0 && i<miCount); return (((s8*)mpvData)+i*miStride); }

	// for standard buffers these are valid
	_n_inline vec3_t*	getPos() { return mpvData; }
	_n_inline vec2_t*	getTexCoords(s32 level);
};

}
#endif //XNEXTAR_VERTEX_BUFFER