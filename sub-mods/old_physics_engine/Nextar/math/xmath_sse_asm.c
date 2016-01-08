//@ Abhishek Dey
//@ 1/17/2007

#include <xmath_c.h>

#ifdef X_SSE_INCLUDE
#ifdef XSSE_ASM_VERSION

void nQuatMul(f32* qa,const f32* qb,const f32* qc)
{
	__asm
	{
		mov		eax, qb
		movaps  xmm0,[eax]
		mov		eax, qc
		movaps  xmm1,[eax]
		mov		eax, qa
		; q0 = qb.qc except that we substract
		movaps	xmm2,xmm0
		mulps	xmm2,xmm1

		movaps	xmm3,xmm2
		shufps	xmm3,xmm3,0F9h
		subss	xmm2,xmm3
		shufps	xmm3,xmm3,0F9h
		subss	xmm2,xmm3
		shufps	xmm3,xmm3,0F9h
		subss	xmm2,xmm3
		movss	[eax],xmm2

		shufps	xmm1,xmm1,0B1h	;1Bh,0B1h
		movaps	xmm2,xmm0
		mulps	xmm2,xmm1
		
		movaps	xmm3,xmm2
		shufps	xmm3,xmm3,0F9h
		addss	xmm2,xmm3
		shufps	xmm3,xmm3,0F9h
		addss	xmm2,xmm3
		shufps	xmm3,xmm3,0F9h
		subss	xmm2,xmm3
		movss	[eax+4],xmm2

		shufps	xmm1,xmm1,1Bh	;0B1h
		movaps	xmm2,xmm0
		mulps	xmm2,xmm1
		
		movaps	xmm3,xmm2
		shufps	xmm3,xmm3,0F9h
		subss	xmm2,xmm3
		shufps	xmm3,xmm3,0F9h
		addss	xmm2,xmm3
		shufps	xmm3,xmm3,0F9h
		addss	xmm2,xmm3
		movss	[eax+8],xmm2

		shufps	xmm1,xmm1,0B1h
		movaps	xmm2,xmm0
		mulps	xmm2,xmm1
		
		movaps	xmm3,xmm2
		shufps	xmm3,xmm3,0F9h
		addss	xmm2,xmm3
		shufps	xmm3,xmm3,0F9h
		subss	xmm2,xmm3
		shufps	xmm3,xmm3,0F9h
		addss	xmm2,xmm3
		movss	[eax+12],xmm2
	}
}

//TODO: write the intrinsic version, i'm feeling lazy
void nWtoDQuat(f32* dq, const f32* w, const f32* q)
{
	const f32 half = 0.5f;
	const f32 one = 1.0f;
	const f32 minusone = -1.0f;
	__asm
	{
		mov eax, w
		movaps xmm0, [eax]
		movss xmm1,half
		shufps xmm1,xmm1,0
		mulps xmm0,xmm1
		mov eax, q
		movaps xmm1, [eax]
		movss xmm2,xmm0
		shufps xmm2,xmm2,0
		movaps xmm3,xmm1
		mulps xmm3,xmm2
		shufps xmm0,xmm0,_MM_SHUFFLE(3,2,2,1)
		movss xmm2,xmm0
		shufps xmm2,xmm2,0
		movaps xmm4,xmm1
		mulps xmm4,xmm2
		shufps xmm0,xmm0,_MM_SHUFFLE(1,1,1,1)
		mulps xmm1,xmm0

		shufps xmm3,xmm3,_MM_SHUFFLE(2,3,0,1)
		shufps xmm4,xmm4,_MM_SHUFFLE(1,0,3,2)
		shufps xmm1,xmm1,_MM_SHUFFLE(0,1,2,3)

		movss  xmm2,one
		movss  xmm0,minusone
		shufps xmm0,xmm2,0
		mulps  xmm1,xmm0
		shufps xmm0,xmm0,_MM_SHUFFLE(0,2,2,0)
		mulps  xmm4,xmm0
		shufps xmm0,xmm0,_MM_SHUFFLE(1,0,1,0)
		mulps  xmm3,xmm0
		addps  xmm4,xmm3
		addps  xmm1,xmm4
		mov eax, dq
		movaps [eax],xmm1
	}
}

#endif // XSSE_ASM_VERSION
#endif