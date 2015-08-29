#ifndef _NEXTAR_MATH_CONFIG_H
#define _NEXTAR_MATH_CONFIG_H

#define NEX_VECTOR_MATH_TYPE_SSE		0
#define NEX_VECTOR_MATH_TYPE_3DNOW		1
#define NEX_VECTOR_MATH_TYPE_VMX128		2
#define NEX_VECTOR_MATH_TYPE_FALLBACK   3

#if defined(NEX_USE_VECTOR_INSTRUCTIONS)
#define NEX_VECTOR_MATH_TYPE   NEX_VECTOR_MATH_TYPE_SSE
#else
#define NEX_VECTOR_MATH_TYPE   NEX_VECTOR_MATH_TYPE_FALLBACK
#endif

#if NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE
//#define NEX_VECTOR_MATH_SSE3
#define NEX_VECTOR_MATH_SSE4
#endif

// @remarks Set this to 1 to do debug checks
#ifdef NEX_DEBUG
#	define NEX_CORE_DEBUG_CHECKS		1
#endif

// Num matrix per block in matrix pool
#define NEX_MATRIX_POOL_NUM_PER_BLOCK ((size_t)BaseConstants::NUM_MATRIX_PER_BLOCK)
#define NEX_BV_POOL_NUM_PER_BLOCK ((size_t)BaseConstants::NUM_BV_PER_BLOCK)
#ifdef NEX_USE_ASM_WHEN_REQUIRED 
#	define NEX_CORE_USE_ASM
#endif

#endif //_NEXTAR_MATH_CONFIG_H