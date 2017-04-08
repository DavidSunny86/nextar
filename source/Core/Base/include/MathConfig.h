#ifndef _NEXTAR_MATH_CONFIG_H
#define _NEXTAR_MATH_CONFIG_H

// @remarks Set this to 1 to do debug checks
#ifdef NEX_DEBUG
#	define NEX_CORE_DEBUG_CHECKS		1
#endif

#if (NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE2) \
	|| (NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE3) \
	|| (NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE4) \
	|| (NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE4_1) \
	|| (NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE4_2)
#define NEX_VECTOR_MATH_TYPE_IS_SSE 1
#else
#define NEX_VECTOR_MATH_TYPE_IS_SSE 0
#endif

// Num matrix per block in matrix pool
#define NEX_MATRIX_POOL_NUM_PER_BLOCK ((size_t)BaseConstants::NUM_MATRIX_PER_BLOCK)
#define NEX_BV_POOL_NUM_PER_BLOCK ((size_t)BaseConstants::NUM_BV_PER_BLOCK)
#ifdef NEX_USE_ASM_WHEN_REQUIRED 
#	define NEX_CORE_USE_ASM
#endif


#endif //_NEXTAR_MATH_CONFIG_H