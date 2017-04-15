#pragma once

#include <NexBase.h>
#include <PooledAllocator.h>

namespace nextar {
namespace Math {

template <>
class TraitsBase<_Vec2> {
public:
	typedef _Vec2::type type;
	typedef const type& pref;
	typedef float base_type;
	typedef float row_type;
	
	enum : uint32 {
		_count = 2,
	};

	inline static bool Equals(pref v1, pref v2) {
		return (bool)(Math::Traits<float>::Equals(v1.x, v2.x) &&
			Math::Traits<float>::Equals(v1.y, v2.y));
	}

	inline static bool IsNan(pref v) {
		return Traits<float>::IsNan(v.x) ||
			Traits<float>::IsNan(v.y);
	}

	inline static bool IsInf(pref v) {
		return Traits<float>::IsInf(v.x) ||
			Traits<float>::IsInf(v.y);
	}
};

template <>
class TraitsBase<_Vec3> {
public:
	typedef _Vec3::type type;
	typedef const type& pref;
	typedef float base_type;
	typedef float row_type;
	
	enum : uint32 {
		_count = 3,
	};

	inline static bool Equals(pref v1, pref v2) {
		return (bool)(Math::Traits<float>::Equals(v1.x, v2.x) &&
			Math::Traits<float>::Equals(v1.y, v2.y) &&
			Math::Traits<float>::Equals(v1.z, v2.z));
	}


	inline static bool IsNan(pref v) {
		return Traits<float>::IsNan(v.x) ||
			Traits<float>::IsNan(v.y) ||
			Traits<float>::IsNan(v.z);
	}

	inline static bool IsInf(pref v) {
		return Traits<float>::IsInf(v.x) ||
			Traits<float>::IsInf(v.y) ||
			Traits<float>::IsInf(v.z);
	}
};

template <>
class TraitsBase<_Quad> {
public:
	typedef _Quad type;
	typedef const type pref;
	typedef float base_type;
	typedef float row_type;

	enum : uint32 {
		_count = 4,
	};

	inline static bool Equals(pref v1, pref v2);
	inline static bool IsNan(pref v);
	inline static bool IsInf(pref v);
};

template <>
class TraitsBase<_Vec3A> : public TraitsBase<_Quad> {
public:
};

template <>
class TraitsBase<_Vec4> : public TraitsBase<_Quad> {
public:
};

template <>
class TraitsBase<_Plane> : public TraitsBase<_Quad> {
public:
};

template <>
class TraitsBase<_Quat> : public TraitsBase<_Quad> {
public:
};

template <>
class TraitsBase<_EulerAngles> : public TraitsBase<_Vec3> {
public:
};

template <>
class TraitsBase<_AxisAngle> : public TraitsBase<_Quad> {
public:
};

template <>
class TraitsBase<_PolarCoord> : public TraitsBase<_Vec2> {
public:
};

template <const uint32 n>
class TraitsIntVec {
public:
	typedef _IVecN<n> type;
	typedef const type& pref;
	typedef int32 base_type;
	typedef int32 row_type;
	
	enum : uint32 {
		_count = n,
	};

	inline static bool Equals(pref v1, pref v2) {
		return v1 == v2;
	}

	inline static bool IsNan(pref v) {
		return false;
	}

	inline static bool IsInf(pref v) {
		return false;
	}
};

template <>
class TraitsBase<IVector2> : public TraitsIntVec<2> {
public:
};

template <>
class TraitsBase<IVector3> : public TraitsIntVec<3> {
public:
};

template <>
class TraitsBase<IVector4> : public TraitsIntVec<4> {
public:
};

template <typename M, const unsigned R, const unsigned C, 
	typename RowType>
class _TraitsBaseMat {
public:
	typedef M type;
	typedef const type& pref;
	typedef RowType row_type_id;
	typedef Traits<RowType> row_type_traits;
	typedef typename row_type_traits::type row_type;
	typedef typename row_type_traits::base_type base_type;
	typedef float float_type;

	enum : uint32 {
		_count = R*C,
		_rows = R,
		_columns = C,
	};

	inline static bool Equals(pref v1, pref v2) {
		for (unsigned int i = 0; i < _rows; ++i) {
			if (!row_type_traits::Equals(v1.r[i], v2.r[i]))
				return false;
		}
		return true;
	}

	inline static bool IsNan(pref v) {
		return false;
	}

	inline static bool IsInf(pref v) {
		return false;
	}
};

template <>
class TraitsBase<_Matrix4x4> : public _TraitsBaseMat<_Matrix4x4, 4, 4, _Vec4> {
public:
};

template <>
class TraitsBase<_Matrix3x4> : public _TraitsBaseMat<_Matrix3x4, 3, 4, _Vec4> {
public:
};

template <>
class TraitsBase<_AxisAlignedBox> : public _TraitsBaseMat<_AxisAlignedBox, 2, 4, _Vec3A> {
public:
};

template <>
class TraitsBase<_Rect> : public _TraitsBaseMat<_Rect, 2, 2, _Vec2> {
public:
};

}
}

