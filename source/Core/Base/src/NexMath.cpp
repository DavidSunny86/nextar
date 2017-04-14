#include <BaseHeaders.h>
#include <MathBase.h>
#include <MathTypes.h>

#ifdef NEX_MATH_RANDOMIZER_DEFINED
NEX_MATH_RANDOM_SEED
;
#endif

namespace nextar {
using namespace Math;
uint32 RandomizerSeed::seed = 2147483647;
template class PooledAllocator< _Matrix3x4, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE >;
template class PooledAllocator< _Matrix4x4, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE >;
template class PooledAllocator<Vec3A::type, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE>;
//template class PooledAllocator<Vec4::type, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE>;
//template class PooledAllocator<Quat::type, NEX_MATRIX_POOL_NUM_PER_BLOCK, MEMCAT_MATH_CORE>;

const Vec3::type Vec3::type::XAxis = { 1, 0, 0 };
const Vec3::type Vec3::type::YAxis = { 0, 1, 0 };
const Vec3::type Vec3::type::ZAxis = { 0, 0, 1 };
const Vec3::type Vec3::type::Origin = { 0, 0, 0 };
const Vec2::type Vec2::type::Origin = { 0, 0 };
_NexBaseAPI const Mat4::type Mat4::type::IdentityMatrix = { 	
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1	
};

_NexBaseAPI const AxisAlignedBox AxisAlignedBox::LargestBox = {
	-Math::SCALAR_MAX, -Math::SCALAR_MAX, -Math::SCALAR_MAX, 1,
	 Math::SCALAR_MAX,  Math::SCALAR_MAX,  Math::SCALAR_MAX, 1
};

_NexBaseAPI const AxisAlignedBox AxisAlignedBox::InvalidBox( Math::SCALAR_MAX,  Math::SCALAR_MAX,  Math::SCALAR_MAX,-Math::SCALAR_MAX, -Math::SCALAR_MAX, -Math::SCALAR_MAX);

namespace Math {

_NexBaseAPI int32 GetNearestPow2(int32 from) {
	int32 r = 1;
	while (r < from)
		r <<= 1;
	if (r > from && r > 64)
		r >>= 1;
	return r;
}

_NexBaseAPI bool IsPow2(uint32 from) {
	uint32 r = 1;
	if (!from)
		return 0;

	while (r < from)
		r <<= 1;
	if (r != from)
		return 0;
	return 1;
}
}

#if NEX_VECTOR_MATH_TYPE_IS_SSE

#define NEXCORE_DEFINE_MASK(maskname) const _ConstI32  maskname
#define NEXCORE_DEFINE_F32C(_f32const_) const _ConstF32  _f32const_

NEXCORE_DEFINE_MASK(N3D_ClearSign) = { 0x7fffffff, 0x7fffffff, 0x7fffffff,
		0x7fffffff };
NEXCORE_DEFINE_MASK(N3D_XXXX) =
		{ 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
NEXCORE_DEFINE_MASK(N3D_OXOX) =
		{ 0x00000000, 0x80000000, 0x00000000, 0x80000000 };
NEXCORE_DEFINE_MASK(N3D_XOOX) =
		{ 0x80000000, 0x00000000, 0x00000000, 0x80000000 };
NEXCORE_DEFINE_MASK(N3D_OOXX) =
		{ 0x00000000, 0x00000000, 0x80000000, 0x80000000 };
NEXCORE_DEFINE_MASK(N3D_OXXX) =
		{ 0x00000000, 0x80000000, 0x80000000, 0x80000000 };
NEXCORE_DEFINE_MASK(N3D_X000) =
		{ 0x80000000, 0x00000000, 0x00000000, 0x00000000 };
NEXCORE_DEFINE_MASK(N3D_FFFO) =
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 };
NEXCORE_DEFINE_MASK(N3D_F00O) =
		{ 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000 };
NEXCORE_DEFINE_MASK(N3D_0F0O) =
		{ 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000 };
NEXCORE_DEFINE_MASK(N3D_00FO) =
		{ 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000 };
NEXCORE_DEFINE_MASK(N3D_OOOF) =
		{ 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF };
NEXCORE_DEFINE_MASK(N3D_FFOO) =
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000 };
NEXCORE_DEFINE_MASK(N3D_FFFF) =
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
// These define some floating point constants
NEXCORE_DEFINE_F32C(N3D_1000) = { 1.f, 0.f, 0.f, 0.f };
NEXCORE_DEFINE_F32C(N3D_0100) = { 0.f, 1.f, 0.f, 0.f };
NEXCORE_DEFINE_F32C(N3D_0010) = { 0.f, 0.f, 1.f, 0.f };
NEXCORE_DEFINE_F32C(N3D_0001) = { 0.f, 0.f, 0.f, 1.f };
NEXCORE_DEFINE_F32C(N3D_0000) = { 0.f, 0.f, 0.f, 0.f };
NEXCORE_DEFINE_F32C(N3D_1111) = { 1.f, 1.f, 1.f, 1.f };
NEXCORE_DEFINE_F32C(N3D_1110) = { 1.f, 1.f, 1.f, 0.f };
NEXCORE_DEFINE_F32C(N3D_HALF) = { 0.5f, 0.5f, 0.5f, 0.5f };
NEXCORE_DEFINE_F32C(N3D_THREE) = { 3.f, 3.f, 3.f, 3.f };
NEXCORE_DEFINE_F32C(N3D_VMinusOne) = { -1.f, -1.f, -1.f, -1.f };
NEXCORE_DEFINE_F32C(N3D_Infinite) = { 0x7F800000, 0x7F800000, 0x7F800000,
		0x7F800000 };

#undef NEXCORE_DEFINE_MASK
#undef NEXCORE_DEFINE_F32C

#endif

}

