
#ifndef NEXCORE_SSE_CONSTANTS_H
#define NEXCORE_SSE_CONSTANTS_H


#ifndef NEXTAR_MATH_SSETYPES_H
#	error Do not include this 'life' (correct me!) directly
#endif
NEX_ALIGNED_STRUCT_BEGIN(16,_ConstF32)
{
	union {
		float f[4];
		Quad v;
	};
}NEX_ALIGNED_STRUCT_END(16);

NEX_ALIGNED_STRUCT_BEGIN(16,_ConstI32)
{
	union {
		nextar::uint32 f[4];
		Quad v;
	};
}NEX_ALIGNED_STRUCT_END(16);

#define NEXCORE_DEFINE_MASK(maskname) NEX_EXTERN_SYM _NexExport const _ConstI32  maskname
#define NEXCORE_DEFINE_F32C(_f32const_) NEX_EXTERN_SYM _NexExport const _ConstF32  _f32const_

/**
 * @remarks Constants used for SIMD operations
 **/
// some bit masks and there description
// This mask with andps will clear sign bit
// for aligned float vectors
NEXCORE_DEFINE_MASK(N3D_ClearSign);
// These masks can be used as defined hereunder:
// Notion: X indicates 1 and O indicates 0
// In actuallity 1 in the MSB indicates negetive.
// The thing to know is in order to flip signs in
// the X areas just XORPS with the appropriate flag.
NEXCORE_DEFINE_MASK(N3D_XXXX);
NEXCORE_DEFINE_MASK(N3D_OXOX);
NEXCORE_DEFINE_MASK(N3D_XOOX);
NEXCORE_DEFINE_MASK(N3D_OOXX);
NEXCORE_DEFINE_MASK(N3D_OXXX);
NEXCORE_DEFINE_MASK(N3D_X000);
// This mask with xorps will flip the sign bit
// for aligned float vectors
#define N3D_FlipSign            N3D_XXXX
// These masks can be used as defined hereunder:
// Notion: F indicates full and O indicates 0
// andps carried out with these masks will clear
// the 0 areas
NEXCORE_DEFINE_MASK(N3D_FFFO);
NEXCORE_DEFINE_MASK(N3D_OOOF);
NEXCORE_DEFINE_MASK(N3D_FFOO);
NEXCORE_DEFINE_MASK(N3D_FFFF);
// These define some floating point constants
NEXCORE_DEFINE_F32C(N3D_1000);
NEXCORE_DEFINE_F32C(N3D_0100);
NEXCORE_DEFINE_F32C(N3D_0010);
NEXCORE_DEFINE_F32C(N3D_0001);
NEXCORE_DEFINE_F32C(N3D_0000);
NEXCORE_DEFINE_F32C(N3D_1111);
NEXCORE_DEFINE_F32C(N3D_1110);
NEXCORE_DEFINE_F32C(N3D_HALF);
NEXCORE_DEFINE_F32C(N3D_THREE);
NEXCORE_DEFINE_F32C(N3D_VMinusOne);
NEXCORE_DEFINE_F32C(N3D_Infinite);

#define N3D_VZero N3D_0000
#define N3D_VOne  N3D_1111

#undef NEXCORE_DEFINE_MASK
#undef NEXCORE_DEFINE_F32C

#endif //NEXCORE_SSE_CONSTANTS_H
