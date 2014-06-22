#ifndef NEXTAR_MATH_CONFIG_H
#define NEXTAR_MATH_CONFIG_H

#define NEX_VECTOR_MATH_TYPE_SSE		0	// intel#define NEX_VECTOR_MATH_TYPE_3DNOW		1	// amd#define NEX_VECTOR_MATH_TYPE_VMX128		2	// xbox#define NEX_VECTOR_MATH_TYPE_FALLBACK           3	// c emulation// @remarks Set the vector math type architecture to use here
#define NEX_VECTOR_MATH_TYPE   NEX_VECTOR_MATH_TYPE_SSE

#if NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE
// @remarks Comment out this line to remove sse3/sse4 instructions
#	define NEX_VECTOR_MATH_SSE3
//#	define NEX_VECTOR_MATH_SSE4
#endif

// @remarks Set this to 1 to do debug checks
#define NEX_CORE_DEBUG_CHECKS		1

// Num matrix per block in matrix pool
#define NEX_MATRIX_POOL_NUM_PER_BLOCK 32
#define NEX_BV_POOL_NUM_PER_BLOCK 32
#ifdef NEX_USE_ASM_WHEN_REQUIRED 
#	define NEX_CORE_USE_ASM
#endif

#endif //NEXTAR_MATH_CONFIG_H