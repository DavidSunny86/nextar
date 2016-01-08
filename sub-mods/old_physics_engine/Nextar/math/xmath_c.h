
//@ Abhishek Dey
//@ 1/16/2007	11:58:45 PM
//@ September 03 2007

#ifndef XMATH_C_H
#define XMATH_C_H

#include <xbasictypes.h>
#ifdef __cplusplus
extern "C" {
#endif

// some macros are very specific
// just to ensure that simd does not creep into
// the actual code, macro seems good though...;)
// macro
#define X_Real(x)	(x##f)
//! Integer representation of a floating-point value.
#define X_MkInt(x)						(*(u32*)(&(x)))
//! Signed integer representation of a floating-point value.
#define X_MkSInt(x)						(*(s32*)(&(x)))
//! Absolute integer representation of a floating-point value
#define X_MkAbsInt(x)					(X_MkInt(x)&0x7fffffff)
//! Floating-point representation of an integer value.
#define X_MkFloat(x)					(*(f32*)(&(x)))
//! Check the sign of a floating point number.
#define X_IsFloatNeg(x)				(X_MkInt(x)&0x80000000)
//! Is a undefined quantity
#define X_IsNan( value )					((X_MkInt(value)&0x7f800000) == 0x7f800000)
//! Is indeterminate
#define X_IsInd( value )		(X_MkInt(value) == 0xffc00000)
//! Is +infinity
#define X_IsPlusInf( value )				(X_MkInt(value) == 0x7f800000)
//! Is -infinity
#define X_IsMinusInf( value )				(X_MkInt(value) == 0xff800000)
//! Is float valid
#define X_IsValidFloat( value )		(!(X_IsNan(value) || X_IsInd(value) || X_IsPlusInf(value) || X_IsMinusInf(value)))
//! More than
#define X_MoreThan(a,b)					(X_MkAbsInt(a) > X_MkInt(b))
//! set FPU precision.
// some comparison macros for f32 that can prove efficient
#define X_MoreThanEqZero(a)					!X_IsFloatNeg(a)
#define X_MoreThanZero(a)					((a)>0)
#define X_LessThanEqZero(a)					((a)<=0)
#define X_LessThanZero(a)					X_IsFloatNeg(a)

#ifdef X_WIN32
#	define SET_FPU_PRECISION_SINGLE		_controlfp(_PC_24, MCW_PC);
#	define SET_FPU_PRECISION_DOUBLE		_controlfp(_CW_DEFAULT, 0xfffff);
#else
#	define SET_FPU_PRECISION_SINGLE		
#	define SET_FPU_PRECISION_DOUBLE		
#endif
//! roll degree [0,360]
#define X_Roll360(a)				if(X_IsFloatNeg(a)) a=360; else if (a>360) a = 0;
//! roll radians [0,2pi]
#define X_Roll2Pi(a)				if(X_IsFloatNeg(a)) a=N3D2Pi; else if (a>N3D2Pi) a = 0;
//! roll degree [-180,180]
#define X_Roll180(a)				if(a<-180.0f) a=180.0f; else if (a > 180.0f) a = -180.0f;
//! roll pi
#define X_RollPi(a)				if(a < -(N3DPi) ) a = (N3DPi); else if (a > (N3DPi)) a = -(N3DPi);
//! blocks the range of angle to straight up/down
#define X_RollPiBy2(a)			if(a<-N3DPiBy2-N3DEpsilon)a=-N3DPiBy2-N3DEpsilon;else if(a>N3DPiBy2-N3DEpsilon) a = N3DPiBy2-N3DEpsilon;  
//! grades to radians
#define X_Deg2Rad(a)			(a*0.0174532925f)               
//! radians to grades
#define X_Rad2Deg(a)			(a*57.295779513f)           
//! if floats are equal with round off
#define X_FloatEquals( v1, v2 ) ((v2-N3DRoundOff) <= v1 && v1 <= (v2+N3DRoundOff))
//! random number generator
#define X_Frand(iLow,iHigh) (f32)((f32)(rand()%iLow)/iHigh) /* further apart iHigh-iLow more near 0 the ans goes */
#define X_FrandPrec(nLow, nHigh) ( ( (1.0f / ( (rand() % 100) + 1) ) * ((nHigh) - (nLow ) ) ) + (nLow) )		
#define X_IRand(low,high) (rand() % ( (high) - (low) + 1) + (low) )
/* swap f32 */
#define X_FloatSwap( fV1, fV2 ) X_MkInt(fV1) ^= X_MkInt(fV2) ^= X_MkInt(fV1) ^= X_MkInt(fV2);
// cross
#define X_SetV3(a,vx,vy,vz)	(a).x=vx; (a).y=vy; (a).z=vz;
#define X_SetV2(a,vx,vy)		(a).x=vx; (a).y=vy;
#define X_V3Cpy(a,b)			(a).x=(b).x; (a).y=(b).y; (a).z=(b).z;
#define X_V3Add(dest,a,b)		dest.x=a.x+b.x;dest.y=a.y+b.y;dest.z=a.z+b.z;
#define X_V3Sub(dest,a,b)		dest.x=a.x-b.x;dest.y=a.y-b.y;dest.z=a.z-b.z;
#define X_V3Dot(a,b)			(a.x*b.x + a.y*b.y + a.z*b.z)
#define X_V3Length(v)			(sqrtr(X_V3Dot(v,v)))
#define X_V3FMul(dest,v1,f)		dest.x = v1.x*f;dest.y = v1.y*f;dest.z = v1.x*f;
#define X_V3Cross(dest,op,v1,v2)	\
	(dest).x op (v1).y*(v2).z-(v1).z*(v2).y; \
	(dest).y op (v1).z*(v2).x-(v1).x*(v2).z; \
	(dest).z op (v1).x*(v2).y-(v1).y*(v2).x;

/* End macros */


#ifndef __cplusplus
	typedef unsigned char bool;
#	define true		1
#	define false	0
#endif

enum {
	X_AXIS = 0,
	Y_AXIS = 1,
	Z_AXIS = 2
};
typedef enum _tag_INTERSECTION
{
	IREL_FRONT,
	IREL_BACK,
	IREL_COPLANER,
	IREL_SPANNING
}INTERSECTION;

/* constants */
#define N3DRoundOff						X_Real(0.0001)
#define N3DPi								X_Real(3.14159265358900)
#define N3D2Pi							X_Real(6.28318530717900)
#define N3DPiBy2							X_Real(1.57079632679400)
#define N3DPiBy4						   X_Real(0.78539816339700)
#define N3DPiBy6						   X_Real(0.52359877559800)
#define N3DGrad						   X_Real(0.0174532925199)
#define N3D1ByPi							X_Real(0.31830988618379067153776752674503)
#define N3D1By2Pi							X_Real(0.15915494309189533576888376337254)
#define N3DInfinity						X_Real(3.402823466e+38)
#define N3DInfinityMed					X_Real(999999999.0)
#define N3DEpsilonBig						X_Real(0.0625)
#define N3DEpsilonMed						X_Real(0.0009765625)
#define N3DEpsilon						X_Real(1.192092896e-07)
#define N3DInv3							X_Real(0.33333333333333333333)
#define N3DInv6							X_Real(0.16666666666666666666)
#define N3DInv7						   X_Real(0.14285714285714285714)
#define N3DInv9						   X_Real(0.11111111111111111111)
#define N3DInv255							X_Real(0.00392156862745098039)
#define N3DSqrt2							X_Real(1.41421356237)
#define N3DInvSqrt2						X_Real(0.707106781188)
#define N3DSqrt3							X_Real(1.73205080757)
#define N3DInvSqrt3						X_Real(0.577350269189)
extern const mat44_t					N3D4x4IdentityMatrix;
/* End constants */
#if X_USE_ASM_ALTERNATIVES >= 1
#ifndef X_USE_FLOATINGPT_INTRIN

#ifndef ifloor/* try using the unsafe versions */
#define ifloor(x)	asmfloorr(x)
#endif

#ifndef cosr
#define	cosr(x)		asmcosr(x)
#endif

#ifndef sinr
#define	sinr(x)		asmsinr(x)
#endif

#ifndef tanr
#define tanr(x)		asmtanr(x)
#endif

#ifndef rsqrtr
#define rsqrtr(x)	asmrsqrtr(x)
#endif

#ifndef fabsr
#define	fabsr(x)	asmabsr(x)
#endif

#endif //X_USE_FLOATINGPT_INTRIN
#endif //X_USE_ASM_ALTERNATIVES

#if	defined( _M_ALPHA ) || defined( _M_MRX000 ) || defined(__INTEL_COMPILER) || defined(X_X86)

#ifndef cosr
#define	cosr(x)			cosf(x)
#endif

#ifndef acosr
#define	acosr(x)		acosf(x)
#endif

#ifndef sinr
#define	sinr(x)			sinf(x)
#endif

#ifndef asinr
#define	asinr(x)		asinf(x)
#endif

#ifndef tanr
#define	tanr(x)			tanf(x)
#endif

#ifndef atanr
#define	atanr(x)		atanf(x)
#endif

#ifndef atan2r
#define	atan2r(x,y)		atan2f(x,y)
#endif

#ifndef powr
#define	powr(x,y)		powf(x,y)
#endif

#ifndef fmodr
#define	fmodr(x,y)		fmodf(x,y)
#endif

#ifndef fabsr
#define fabsr(x)		fabsf(x)
#endif

#ifndef sqrtr
#define sqrtr(x)		sqrtf(x)
#endif 

#else

#ifndef cosr
#define	cosr(x)		(f32)cos((double)x)
#endif

#ifndef acosr
#define	acosr(x)	acosf(x)
#endif

#ifndef sinr
#define	sinr(x)		(f32)sin((double)x)
#endif

#ifndef asinr
#define	asinr(x)	asinf(x)
#endif

#ifndef tanr
#define	tanr(x)		(f32)tan((double)x)
#endif

#ifndef atanr
#define	atanr(x)	atanf(x)
#endif

#ifndef atan2r
#define	atan2r(x,y)	atan2f(x,y)
#endif

#ifndef powr
#define	powr(x,y)	powf(x,y)
#endif

#ifndef fmodr
#define	fmodr(x,y)	fmod(x,y)
#endif

#endif 

#ifndef ifloor
#define ifloor(x) floorr(x)
#endif

#ifndef rsqrtr
#define rsqrtr(x) 1.f/sqrtr(x)
#endif

#ifndef fabsr
#define fabsr	_absr(x)
#endif

#ifndef sqrtr
#define sqrtr(x) fsqrtr(x)
#endif

void nQuatMul(f32* store,const f32* q1,const f32* q2);
void nQuatFromAxisAngle(f32* store,const f32* axis,const f32 ang);
void nQuatFromEulerAngles(f32* store,const f32 yaw,const f32 pitch,const f32 roll);
void nMatrixFromAxisAngle(f32* m,const f32* axis,const f32 ang);
void nRSetIdentity(f32* r);
void nMatrixCameraLH(f32* m,const f32* eye,const f32* lookat,const f32* up);
void nMatrixProjectionFovLH(f32* m,const f32 fov,const f32 aspratio,const f32 nearplane_t,const f32 farplane_t);
void nWtoDQuat(f32* dq, const f32* w, const f32* q);
void nQuatToR(f32* r,const f32* dq);
void nM12Inverse(f32* store,const f32* m); // we need an SIMD version of this
//void cM12InverseDP(f32* store,const f32* m); // we need an SIMD version of this double precision
void nOrthoVectors(f32* v1,f32* v2,const f32* n);
// these functions were ripped off
void cRtoQ(f32* dq,const f32* r);
//void cPlaneSpace(const f32* n, f32* p, f32* q);
void cMatrixProjectionFovRH(f32* m,const f32 fov,const f32 aspratio,const f32 nearplane_t,const f32 farplane_t);
void cMatrixCameraRH(f32* m,const f32* eye,const f32* lookat,const f32* up);
// the inline functions
//========================================================================================
// IMPORTANT NOTE::
// Some functions marked as 
// SAFE_PTR_USAGE
// are safe when the store and any other ptr are the same, i.e. no extra
// space for store is needed and any other ptr can be reused to store.

//// macro to find the inverse of a TR object

// copy one to another
_n_inline void xM12Cpy(f32* m1,const f32* m2); //SAFE_PTR_USAGE
_n_inline void xM16Cpy(f32* m1,const f32* m2); //SAFE_PTR_USAGE
_n_inline void xQ4Cpy(f32* q1,const f32* q2); //SAFE_PTR_USAGE
// Use the macros instead and use ptrs as used for the function
//macro xM12CpyMac(f32* m1,const f32* m2); //SAFE_PTR_USAGE
//macro xM16CpyMac(f32* m1,const f32* m2); //SAFE_PTR_USAGE
//macro xQ4CpyMac(f32* q1,const f32* q2); //SAFE_PTR_USAGE
// quad lengths
_n_inline void xQ3LenSq(f32* store,const f32* q1); //SAFE_PTR_USAGE
_n_inline f32  xQ3LenSqR(const f32* q1); //SAFE_PTR_USAGE
_n_inline void xQ3Len(f32* store,const f32* q1); //SAFE_PTR_USAGE
_n_inline f32  xQ3LenR(const f32* q1); //SAFE_PTR_USAGE
_n_inline void xQ4LenSq(f32* store,const f32* q1); //SAFE_PTR_USAGE
_n_inline f32  xQ4LenSqR(const f32* q1); //SAFE_PTR_USAGE
_n_inline void xQ4Len(f32* store,const f32* q1); //SAFE_PTR_USAGE
_n_inline f32  xQ4LenR(const f32* q1); //SAFE_PTR_USAGE
// normalizations
_n_inline void xQ3Normalize(f32* q); //SAFE_PTR_USAGE
_n_inline void xQ4Normalize(f32* q); //SAFE_PTR_USAGE
_n_inline void xPNormalize(f32* q); //SAFE_PTR_USAGE
_n_inline void xQ3NormalizeS(f32* q1,const f32* q2);
_n_inline void xQ4NormalizeS(f32* q1,const f32* q2);
_n_inline void xPNormalizeS(f32* q1,const f32* q2);
// quad spec
_n_inline void xQ4Scale(f32* store,f32 val); //SAFE_PTR_USAGE
_n_inline void xQ3Scale(f32* store,f32 val); //SAFE_PTR_USAGE
_n_inline void xQ4ScaleS(f32* store,const f32* v,f32 val);
_n_inline void xQ3ScaleS(f32* store,const f32* v,f32 val);
_n_inline void xQ3ScaleNAdd(f32* store,const f32* v,f32 val); //SAFE_PTR_USAGE
_n_inline void xQ4ScaleNAdd(f32* store,const f32* v,f32 val); //SAFE_PTR_USAGE
_n_inline void xQ3ScaleNSub(f32* store,const f32* v,f32 val); //SAFE_PTR_USAGE
_n_inline void xQ4ScaleNSub(f32* store,const f32* v,f32 val); //SAFE_PTR_USAGE
_n_inline void xQ3ScaleNMul(f32* store,const f32* v,f32 val); //SAFE_PTR_USAGE
_n_inline void xQ4ScaleNMul(f32* store,const f32* v,f32 val); //SAFE_PTR_USAGE
_n_inline void xQ3ScaleNDiv(f32* store,const f32* v,f32 val); //SAFE_PTR_USAGE
_n_inline void xQ4ScaleNDiv(f32* store,const f32* v,f32 val); //SAFE_PTR_USAGE
_n_inline void xQ4Add(f32* store,const f32* v1,const f32* v2); //SAFE_PTR_USAGE
_n_inline void xQ4Sub(f32* store,const f32* v1,const f32* v2); //SAFE_PTR_USAGE
_n_inline void xQ4Mul(f32* store,const f32* v1,const f32* v2); //SAFE_PTR_USAGE
_n_inline void xQ4Div(f32* store,const f32* v1,const f32* v2); //SAFE_PTR_USAGE
_n_inline void xQ4Interpolate(f32* store,const f32* v2,f32 factor); //SAFE_PTR_USAGE
// vector
_n_inline void xVCross(f32* store,const f32* v1,const f32* v2);
_n_inline void xVDot(f32* store,const f32* v1,const f32* v2);
_n_inline f32  xVDotR(const f32* v1,const f32* v2);
_n_inline void xVDistance(f32* store,const f32* v1,const f32* v2);
_n_inline f32  xVDistanceR(const f32* v1,const f32* v2);
_n_inline void xVDistanceSq(f32* store,const f32* v1,const f32* v2);
_n_inline f32  xVDistanceSqR(const f32* v1,const f32* v2);
// multiplications
// dot each row of r with a and store
_n_inline void xMul0_344(f32* store,const f32* a,const f32* r);	//SAFE_PTR_USAGE
// dot each coloumn of r with a and store (multiply a vector with rotation matrix)
_n_inline void xMul1_344(f32* store,const f32* a,const f32* r);	//SAFE_PTR_USAGE
// multiply each row of r with a, add each COLOUMN of r and store in store
_n_inline void xMul2_344(f32* store,const f32* a,const f32* r);	//SAFE_PTR_USAGE
// dot each row of a with b and store each(i=row(a),j=row(b)) in store(i,j) for(i=0 to 2)( for(j=0 to 2) )
_n_inline void xMul0_34(f32* store,const f32* a,const f32* b);	//SAFE_PTR_USAGE
// dot each row of a with col b and store each(i=row(a),j=col(b)) in store(i,j) for(i=0 to 2)( for(j=0 to 2) )
_n_inline void xMul1_34(f32* store,const f32* a,const f32* b);
// s(i,j) += col(a,i). col(b,j)
_n_inline void xMul2_34(f32* store,const f32* a,const f32* b);
// transpose
_n_inline void xMTrans_34(f32* store,const f32* a);
_n_inline void xMTrans_44(f32* store,const f32* a);
// std vector matrix mul ( no optimization )
_n_inline void xMul1_444(f32* store,const f32* a,const f32* r);	//SAFE_PTR_USAGE
// the result is the vector transform
_n_inline void xMul1_443(f32* store,const f32* a,const f32* r);	//SAFE_PTR_USAGE
// 44 matrix mul ( no optimization )
_n_inline void xMul1_44(f32* store,const f32* a,const f32* b);
// n count quads
_n_inline void xMul1_444o_n(vec4_t* store,const vec4_t* a,const f32* r,s32 cnt);	//SAFE_PTR_USAGE
// n count vector
_n_inline void xMul1_443o_n(vec3_t* store,const vec3_t* a,const f32* r,s32 cnt);	//SAFE_PTR_USAGE
// n count quads
_n_inline void xMul1_344o_n(f32* store,const f32* a,const f32* r,s32 cnt);	//SAFE_PTR_USAGE
// the result is the vector transform [ assumes homogeneous vector i.e. [x,y,z,1] ]
_n_inline void xMul1_444o(f32* store,const f32* a,const f32* r);	//SAFE_PTR_USAGE
// transform local to world space using parent world space matrix ( optimized )
_n_inline void xMul1_44o(f32* store,const f32* a,const f32* b);
// plane_t construction
_n_inline void xPFromPoints(f32* store,const f32* a,const f32* b,const f32* c);
// evaluate a point and a plane_t i.e. plane_t.normal(.)point + plane_t.d
_n_inline f32 xPCalculatePointR(const f32* pl,const f32* point);
_n_inline void xPCalculatePoint(f32*,const f32* pl,const f32* point);
// quaternion
#define xQuatNormalize(q) xQ4Normalize((f32*)q) //SAFE_PTR_USAGE
#define xQuatNormalizeS(s,q) xQ4NormalizeS((f32*)s,(const f32*)q) //SAFE_PTR_USAGE
//
_n_inline void xTMulEfficient(transform* store,const transform* offset,const transform* parent);
//========================================================================================



#ifdef X_SSE_INCLUDE
#include <xmmintrin.h>
#define _x_fclamp(clampwhat,lowvalue,hivalue)	_mm_store_ss(&clampwhat,_mm_max_ss(_mm_set_ss(lowvalue),_mm_min_ss(_mm_set_ss(clampwhat),_mm_set_ss(hivalue))))
#	if defined( XSSE_ASM_VERSION ) && defined( XSSE_INTRIN_VERSION )
#		undef XSSE_INTRIN_VERSION
#	endif
#	if !defined( XSSE_ASM_VERSION ) && !defined( XSSE_INTRIN_VERSION )
#		define XSSE_ASM_VERSION		// this is real good as compared to the intrinsic version
#	endif
#else
#define _x_clamp(clampwhat,lowvalue,hivalue)	clampwhat = __max(lowvalue,__min(clampwhat,hivalue))
#endif

#ifdef XSSE_ASM_VERSION
#	include "xmath_sse_asm.inl"
#elif defined(XSSE_INTRIN_VERSION)
#	include "xmath_sse_int.inl"
#else
#	include "xmath_c.inl"
#endif

#include "xmath_asm.inl"

#ifdef __cplusplus
}
#endif

#endif //XMATH_C_H