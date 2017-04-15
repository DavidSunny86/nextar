//-------------------------
// Created on july 24 2009
//-------------------------
#ifndef _MATHTYPES_H_
#define _MATHTYPES_H_

#include <MathBase.h>
#include <NexAlloc.h>
#include <MemUtils.h>
#include <Randomizer.h>

// Specialize allocator
namespace nextar {
namespace Math {

template<typename T, const size_t NumPerBlock>
class AllocMathPool : public AllocPooledObjectBase<
	PooledAllocator<T, NumPerBlock, MEMCAT_MATH_CORE> > {
public:
	typedef PooledAllocator<T, NumPerBlock, MEMCAT_MATH_CORE> Allocator;
};

template <typename T>
struct TraitsBase {
	typedef T type;
	typedef const type& pref;
	typedef float base_type;
	typedef base_type row_type;
	typedef float float_type;

	enum : uint32 {
		_count = sizeof(type) / sizeof(base_type),
		_rows = 1,
		_columns = _count,
	};

	inline static bool Equals(pref v1, pref v2) {
		return v1 - v2 == 0;
	}

	inline static bool IsNan(pref v) {
		return false;
	}

	inline static bool IsInf(pref v) {
		return false;
	}
};

template <typename T>
class Traits : public TraitsBase<T> {
public:

	typedef typename TraitsBase<T>::type type;
	typedef typename TraitsBase<T>::pref pref;
	typedef typename TraitsBase<T>::base_type base_type;
	typedef typename TraitsBase<T>::row_type row_type;
	
	typedef type& ref;
	typedef const ref cref;
	
	enum : uint32 {
		_size = sizeof(type)
	}; 
};


template <>
class TraitsBase<float> {
public:
	typedef float type;
	typedef const float pref;
	typedef float base_type;
	typedef float row_type;

	enum : uint32 {
		_count = 1,
	};

	inline static bool Equals(pref v1, pref v2) {
		if (Math::AlmostEqualUlps(v1, v2, 2))
			return true;
		return Math::AlmostEqualRelativeOrAbsolute(v1, v2, MAX_RELATIVE_ERROR,
			std::numeric_limits<float>::min());
	}

	inline static bool IsNan(pref v) {
		return std::isnan(v);
	}

	inline static bool IsInf(pref v) {
		return std::isinf(v);
	}
};


}
}

#include <math/VectorTypes.h>

#endif //_MATHTYPES_H_
