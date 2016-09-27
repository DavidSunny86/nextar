// Obi De
// Nov 25 2007
// Dec 27 07
//---------------------------
#ifndef NEXTAR_MATH_H_
#define NEXTAR_MATH_H_

#include <NexCore.h>
#include <MathConfig.h>

#undef max
#undef min

namespace nextar {

/**
 * @note Matrix Layout
 * @par OpenGl uses Coloumn Major while DirectX uses Row major,
 * but, while considering memory, OpenGL and DirectX both use the
 * same memory locations to store their matrices. 
 * @par Nextar uses the same thing. More specifically, it uses:
 * Row Major, Row Ordered matrices. So the 4 vectors are in 4 rows.
 **/
namespace Math {

static constexpr float ROUND_OFF = 0.000001f;
static constexpr float PI = 3.14159265358900f;
static constexpr float TWO_PI = 6.28318530717900f;
static constexpr float PI_BY_2 = 1.57079632679400f;
static constexpr float PI_BY_4 = 0.78539816339700f;
static constexpr float PI_BY_6 = 0.52359877559800f;
static constexpr float INV_PI = 0.31830988618379067153776752674503f;
static constexpr float INV_2_PI = 0.15915494309189533576888376337254f;
static constexpr float DEG_TO_RAD_MULTIPLIER = 0.0174532925199f;

static constexpr float SCALAR_MAX = 3.402823466e+38f;
static constexpr float SCALAR_MED = 999999999.0f;

static constexpr float EPSILON = 1.192092896e-06f;
static constexpr float EPSILON_MED = 0.0009765625f;
static constexpr float EPSILON_BIG = 0.0625f;

static constexpr float INV_3 = 0.33333333333333333333f;
static constexpr float INV_6 = 0.16666666666666666666f;
static constexpr float INV_7 = 0.14285714285714285714f;
static constexpr float INV_9 = 0.11111111111111111111f;
static constexpr float INV_255 = 0.00392156862745098039f;
static constexpr float SQRT_2 = 1.41421356237f;
static constexpr float INV_SQRT_2 = 0.707106781188f;
static constexpr float SQRT_3 = 1.73205080757f;
static constexpr float INV_SQRT_3 = 0.577350269189f;

static constexpr float MAX_RELATIVE_ERROR = 0.005f;

using namespace ::nextar;

/** Math function overrides */

inline float Floor(float value) {
	return std::floor(value);
}

inline float ArcTan(float value) {
	return std::atan(value);
}

inline float ArcTan2(float y, float x) {
	return std::atan2(y, x);
}

inline float ArcSin(float value) {
	return std::asin(value);
}

inline float ArcCos(float value) {
	return std::acos(value);
}

inline float Abs(float value) {
	return std::fabs(value);
}

inline float Sqrt(float val) {
	return std::sqrt(val);
}

inline float InvSqrt(float val) {
	return 1.f / std::sqrt(val);
}

inline float Sin(float value) {
	return std::sin(value);
}

template <typename T>
inline T Sign(T x) {
	return (x > 0) - (x < 0);
}

inline float Cos(float value) {
	return std::cos(value);
}

inline float Tan(float value) {
	return std::tan(value);
}

inline void SinCos(float __arg, float& _sinvalue, float& _cosvalue) {

#if (NEX_ARCH==NEX_ARCH_X86) && defined(NEX_USE_ASM) && defined(NEX_GCC)
	register float sinvalue, cosvalue;
	asm ("fld %[angle]\t\n"
			"fsincos\t\n"
			"fstp %[cosval]\t\n"
			"fstp %[sinval]\t\n"
			: [sinval] "=g" (sinvalue),
			[cosval] "=g" (cosvalue)
			: [angle] "g" (__arg));
	_sinvalue = sinvalue;
	_cosvalue = cosvalue;
#elif (NEX_ARCH==NEX_ARCH_X86) && defined(NEX_USE_ASM) && defined(NEX_MSVC)
	register float sinvalue, cosvalue;
	__asm
	{
		fld __arg
		fsincos
		fstp cosvalue
		fstp sinvalue
		;mov eax, cosvalue
		;fstp dword ptr[eax]
		;mov eax, sinvalue
		;fstp dword ptr[eax]
	}
	_sinvalue = sinvalue;
	_cosvalue = cosvalue;
#else
	_sinvalue = Math::Sin(__arg);
	_cosvalue = Math::Cos(__arg);
#endif
}
//-------------------------------------------------------
// Other utilities
_NexBaseAPI int32 GetNearestPow2(int32);
_NexBaseAPI bool IsPow2(uint32);
//-------------------------------------------------------
// inlined functions
// wrap angle between -pi & +pi
template <typename IntType>
inline IntType RoundUp(IntType number, IntType multiple) {
	IntType remainder = number % multiple;
	if (remainder == 0)
		return number;
	return number + multiple - remainder;
}

inline float WrapPi(float theta) {
	theta += Math::PI;
	theta -= std::floor(theta * Math::INV_2_PI) * Math::TWO_PI;
	theta -= Math::PI;
	return theta;
}
// float to fixed point conversion, float must be
// between 0-1
inline uint32 FloatToFixed(float f, uint32 n) {
	// value * maxvalue
	return uint32(f * ((1 << (n)) - 1));
}
// fixed to float, float returned is between
// zero and one

inline float FixedToFloat(uint32 f, uint32 n) {
	// value / maxvalue
	return (float) (f) / (float) ((1 << n) - 1);
}
// fixed to fixed, fixed returned is between
// zero and one

inline uint32 FixedToFixed(uint32 f, uint32 fb, uint32 reqb) {

	// ((max(reqb)/max(fb)) * f)
	// the trick is if reqb < fb we can straightforwardly
	// divide by pow(2,reqb-fb), so
	if (reqb < fb)
		return (f >> (reqb - fb));
	return f * ((1 << reqb) - 1 / ((1 << fb) - 1));
}

//-- half-float conversions.
//-- to speed up we can use tables
// (really small tables with mantissa, exponent, offset)
//-- this might be required elsewhere, for now just do it
// algorithmetically

inline uint16 FloatToHalfI(uint32 i) {
	// can use SSE here, but lets
	// do it naive way.
	register int s = (i >> 16) & 0x00008000;
	register int e = ((i >> 23) & 0x000000ff) - (127 - 15);
	register int m = i & 0x007fffff;
	if (e <= 0) {
		if (e < -10)
			return 0;
		m = (m | 0x00800000) >> (1 - e);

		return s | (m >> 13);
	} else if (e == 0xff - (127 - 15)) {
		if (m == 0) // Inf
			return s | 0x7c00;
		else // NAN
		{
			m >>= 13;
			return s | 0x7c00 | m | (m == 0);
		}
	} else {
		if (e > 30) // Overflow
			return s | 0x7c00;
		return s | (e << 10) | (m >> 13);
	}
}

inline uint16 FloatToHalf(float f) {
	return FloatToHalfI(*(uint32*) &f);
}

inline uint32 HalfToFloatI(uint16 y) {
	// can use SSE here, but lets
	// do it naive way.
	register int s = (y >> 15) & 0x00000001;
	register int e = (y >> 10) & 0x0000001f;
	register int m = y & 0x000003ff;

	if (e == 0) {
		if (m == 0) // Plus or minus zero
				{
			return s << 31;
		} else // Denormalized number -- renormalize it
		{
			while (!(m & 0x00000400)) {
				m <<= 1;
				e -= 1;
			}

			e += 1;
			m &= ~0x00000400;
		}
	} else if (e == 31) {
		if (m == 0) // Inf
				{
			return (s << 31) | 0x7f800000;
		} else // NaN
		{
			return (s << 31) | 0x7f800000 | (m << 13);
		}
	}

	e = e + (127 - 15);
	m = m << 13;
	return (s << 31) | (e << 23) | m;
}

inline float HalfToFloat(uint16 y) {

	union {
		float f;
		uint32 i;
	} o;
	o.i = HalfToFloatI(y);
	return o.f;
}

inline float ToRadians(float value) {
	return NEX_DEG2RAD(value);
}

inline float ToDegrees(float value) {
	return NEX_RAD2DEG(value);
}

/* Famous fast reciprocal sqrt */
inline float RecipSqrt(float x) {
	int32 i;
	float y, r;
	y = x * 0.5f;
	i = *(int32 *) (&x);
	i = 0x5f3759df - (i >> 1);
	r = *(float *) (&i);
	r = r * (1.5f - r * r * y);
	return r;
}

/* Sin of angle in the range of [0, pi/2]*/
inline float SinZeroHalfPi(float a) {
	float s, t;
	s = a * a;
	t = -2.39e-08f;
	t *= s;
	t += 2.7526e-06f;
	t *= s;
	t += -1.98409e-04f;
	t *= s;
	t += 8.3333315e-03f;
	t *= s;
	t += -1.666666664e-01f;
	t *= s;
	t += 1.0f;
	t *= a;
	return t;
}

/* Arc tan when x and y are positives */
inline float ArcTanPositive(float y, float x) {
	float a, d, s, t;
	if (y > x) {
		a = -x / y;
		d = Math::PI / 2;
	} else {
		a = y / x;
		d = 0.0f;
	}
	s = a * a;
	t = 0.0028662257f;
	t *= s;
	t += -0.0161657367f;
	t *= s;
	t += 0.0429096138f;
	t *= s;
	t += -0.0752896400f;
	t *= s;
	t += 0.1065626393f;
	t *= s;
	t += -0.1420889944f;
	t *= s;
	t += 0.1999355085f;
	t *= s;
	t += -0.3333314528f;
	t *= s;
	t += 1.0f;
	t *= a;
	t += d;
	return t;
}


inline bool AlmostEqualUlps(float A, float B, int maxUlps)
{
	assert(sizeof(float) == sizeof(int));
	if (A == B)
		return true;
	int aInt = *(int*)&A;
	// Make aInt lexicographically ordered as a twos-complement int
	if (aInt < 0)
		aInt = 0x80000000 - aInt;
	// Make bInt lexicographically ordered as a twos-complement int
	int bInt = *(int*)&B;
	if (bInt < 0)
		bInt = 0x80000000 - bInt;

	int intDiff = std::abs(aInt - bInt);
	if (intDiff <= maxUlps)
		return true;
	return false;
}

inline bool AlmostEqualRelativeOrAbsolute(float A, float B,
	float maxRelativeError, float maxAbsoluteError)
{
	if (fabs(A - B) < maxAbsoluteError)
		return true;
	float relativeError;
	if (fabs(B) > fabs(A))
		relativeError = fabs((A - B) / B);
	else
		relativeError = fabs((A - B) / A);
	if (relativeError <= maxRelativeError)
		return true;
	return false;
}

inline void Clamp(float& clampwhat, float lowvalue, float hivalue) {
#if NEX_VECTOR_MATH_TYPE == NEX_VECTOR_MATH_TYPE_SSE
	_mm_store_ss(&clampwhat,
			_mm_max_ss(_mm_load_ss(&lowvalue),
					_mm_min_ss(_mm_load_ss(&clampwhat),
							_mm_load_ss(&hivalue))));
#else
	clampwhat = std::max(lowvalue,std::min(clampwhat,hivalue));
#endif
}
}

}

#endif //NEXTAR_MATH_H_
