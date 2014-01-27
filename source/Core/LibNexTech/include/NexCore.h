#ifndef NEXTAR_NEXCORE_H
#define NEXTAR_NEXCORE_H

#include <cmath>
#include "NexSys.h"
#include "MathConfig.h"

#define NEXCORE_VERSION NEX_MAKE_VERSION(1,0,1)

// check if vectorizing
#if NEX_VECTOR_MATH_TYPE != NEX_VECTOR_MATH_TYPE_FALLBACK
#define NEX_VECTOR_HARDWARE_USED
#endif

//! feature bits
#define NEX_CPU_FEATURE_MMX    0x0001
#define NEX_CPU_FEATURE_SSE    0x0002
#define NEX_CPU_FEATURE_SSE2   0x0004
#define NEX_CPU_FEATURE_SSE3   0x0008
#define NEX_CPU_FEATURE_SSE4   0x0010
#define NEX_CPU_FEATURE_3DNOW  0x0020

// Typecast immediate real num to float
#define _R(x)	(x##f)
//! Integer representation of a floating-point value.
#define NEX_MK_INT(x)						(*(uint32*)(&(x)))
//! Signed integer representation of a floating-point value.
#define NEX_MK_SIGNED_INT(x)						(*(int32*)(&(x)))
//! Absolute integer representation of a floating-point value
#define NEX_MK_ABS_INT(x)					(NEX_MK_INT(x)&0x7fffffff)
//! Floating-point representation of an integer value.
#define NEX_MK_FLOAT(x)					(*(float*)(&(x)))
//! Check the sign of a floating point number.
#define NEX_IS_FLOAT_NEGETIVE(x)					(NEX_MK_INT(x)&0x80000000)
//! Is a undefined quantity
#define NEX_IS_FLOAT_NAN( value )				((NEX_MK_INT(value)&0x7f800000) == 0x7f800000)
//! Is indeterminate
#define NEX_IS_FLOAT_IND( value )				(NEX_MK_INT(value) == 0xffc00000)
//! Is +infinity
#define NEX_IS_FLOAT_PINF( value )				(NEX_MK_INT(value) == 0x7f800000)
//! Is -infinity
#define NEX_IS_FLOAT_NINF( value )				(NEX_MK_INT(value) == 0xff800000)
//! Is float valid
#define NEX_IS_VALID_FLOAT( value )		(!(NEX_IS_FLOAT_NAN(value) || NEX_IS_FLOAT_IND(value) || NEX_IS_FLOAT_PINF(value) || NEX_IS_FLOAT_NINF(value)))

//! More than
#define NEX_FLOAT_MORETHAN(a,b)					(NEX_MK_ABS_INT(a) > NEX_MK_INT(b))
#define NEX_ABSFLOAT_MORETHAN(a,b)				(NEX_MK_INT(a) > NEX_MK_INT(b))
// some comparison macros for float that can prove efficient
#define NEX_FLOAT_MORETHANEQZERO(a)					!NEX_IS_FLOAT_NEGETIVE(a)
#define NEX_FLOAT_MORETHANZERO(a)					((a)>0)
#define NEX_FLOAT_LESSTHANEQZERO(a)					((a)<=0)
#define NEX_FLOAT_LESSTHANZERO(a)					NEX_IS_FLOAT_NEGETIVE(a)

#define NEX_PIXEL_ALIGNED(v)				(float)(int32)(v + (v > 0 ? 0.5f : -0.5f))
//! roll degree [0,360]
#define NEX_ROLL_360(a)				if(NEX_IS_FLOAT_NEGETIVE(a)) a=360; else if (a>360) a = 0;
//! roll radians [0,2pi]
#define NEX_ROLL_2PI(a)				if(NEX_IS_FLOAT_NEGETIVE(a)) a=N3D2Pi; else if (a>N3D2Pi) a = 0;
//! roll degree [-180,180]
#define NEX_ROLL_180(a)				if(a<-180.0f) a=180.0f; else if (a > 180.0f) a = -180.0f;
//! roll pi
#define NEX_ROLL_PI(a)				if(a < -(N3DPi) ) a = (N3DPi); else if (a > (N3DPi)) a = -(N3DPi);
//! blocks the range of angle to straight up/down
#define NEX_ROLL_PIBY2(a)			if(a<-N3DPiBy2-N3DEpsilon)a=-N3DPiBy2-N3DEpsilon;else if(a>N3DPiBy2-N3DEpsilon) a = N3DPiBy2-N3DEpsilon;  
//! grades to radians
#define NEX_DEG2RAD(a)			(a*0.0174532925f)               
//! radians to grades
#define NEX_RAD2DEG(a)			(a*57.295779513f)           
//! if floats are equal with round off
#define NEX_FLOAT_TOLERANCE_EQUAL( v1, v2, roundoff ) ((v2-roundoff) <= v1 && v1 <= (v2+roundoff))
//! random number generator
#define NEX_FLOAT_RAND(iLow,iHigh) (float)((float)(rand()%iLow)/iHigh) /* further apart iHigh-iLow more near 0 the ans goes */
#define NEX_FLOAT_RAND_PREC(nLow, nHigh) ( ( (1.0f / ( (rand() % 100) + 1) ) * ((nHigh) - (nLow ) ) ) + (nLow) )		
#define NEX_INT_RAND(low,high) (rand() % ( (high) - (low) + 1) + (low) )
/* swap float */
#define NEX_FLOAT_SWAP( fV1, fV2 ) NEX_MK_INT(fV1) ^= NEX_MK_INT(fV2) ^= NEX_MK_INT(fV1) ^= NEX_MK_INT(fV2);

#ifdef NEX_MSVC

/* float to int32 cast : for performance */
/* considering that float to int32 cast causes */
/* a call to __ftol function consider using this instead */
#define NEX_FLOAT_TO_INT_INTPTR(intptr, floatval) \
	__asm fld		floatval	\
	__asm mov		edx, intptr	\
	__asm frndint	\
	__asm fistp	dword ptr[edx]	\
/* params are intptr and floatval */
/* intptr is the pointer to the integer ptr */
/* to store the float value 'floatval' */

#define NEX_FLOAT_TO_INT_INTVAR(intvar, floatval)  \
	__asm fld		floatval	\
	__asm fistp		intvar	\
/* params are intvar and floatval */
/* intvar is the integer variable  */
/* to store the float value 'floatval' */

#else

/* float to int32 cast : for performance */
/* considering that float to int32 cast causes */
/* a call to __ftol function consider using this instead */
#define NEX_FLOAT_TO_INT_INTPTR(intptr, floatval) *intptr = (int32)floatval; /* ugly casting */
/* params are intptr and floatval */
/* intptr is the pointer to the integer ptr */
/* to store the float value 'floatval' */
#define NEX_FLOAT_TO_INT_INTVAR(intvar, floatval)  intvar = (int32)floatval	/* ugly casting */
/* params are intvar and floatval */
/* intvar is the integer variable  */
/* to store the float value 'floatval' */

#endif

/* float to byte case : performance intensive */
#define NEX_FLOAT_TO_BYTE_CAST(bytevar, floatvar) \
	floatvar += (float) 0xC00000; \
	bytevar = *(byte*) &floatvar;
/* params are bytevar and floatval */
/* bytevar is the unsigned 8 bit variable  */
/* to store the float value 'floatval', a variable this time */

#if defined(NEX_GCC) && (NEX_ARCH==NEX_ARCH_X86 || NEX_ARCH==NEX_ARCH_X86_64) && defined(NEX_USE_ASM)
#elif defined(NEX_MSVC) && (NEX_ARCH==NEX_ARCH_X86 || NEX_ARCH==NEX_ARCH_X86_64)
#	pragma intrinsic(acos,cosh,pow,tanh,asin,fmod,sinh)
#	pragma intrinsic(atan,exp,log10,sqrt,atan2,log,sin,tan,cos)
#	pragma intrinsic(fabs)
#endif

/* constants */
#define N3DRoundOff					_R(0.000001)
#define N3DPi						_R(3.14159265358900)
#define N3D2Pi						_R(6.28318530717900)
#define N3DPiBy2					_R(1.57079632679400)
#define N3DPiBy4					_R(0.78539816339700)
#define N3DPiBy6					_R(0.52359877559800)
#define N3DGrad						_R(0.0174532925199)
#define N3D1ByPi					_R(0.31830988618379067153776752674503)
#define N3D1By2Pi					_R(0.15915494309189533576888376337254)
#define N3DInfinity					_R(3.402823466e+38)
#define N3DInfinityMed					_R(999999999.0)
#define N3DEpsilonBig					_R(0.0625)
#define N3DEpsilonMed					_R(0.0009765625)
#define N3DEpsilon					_R(1.192092896e-06)
#define N3DInv3						_R(0.33333333333333333333)
#define N3DInv6						_R(0.16666666666666666666)
#define N3DInv7						_R(0.14285714285714285714)
#define N3DInv9						_R(0.11111111111111111111)
#define N3DInv255					_R(0.00392156862745098039)
#define N3DSqrt2					_R(1.41421356237)
#define N3DInvSqrt2					_R(0.707106781188)
#define N3DSqrt3 					_R(1.73205080757)
#define N3DInvSqrt3					_R(0.577350269189)

namespace nextar {
/*	enum ClipSpecification {
 CLIP_INSIDE = 0,
 CLIP_OUTSIDE = 1,
 CLIP_INTERSECTS = 2
 };
 */ // internal
// NEX_EFFICIENT_TYPEDEF(16,uint32 _albitmask[4]);
}

#endif //NEXTAR_NEXCORE_H
