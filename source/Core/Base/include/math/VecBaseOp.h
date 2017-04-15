#include "VectorMethods.h"
#ifndef VECBASEOP_H_
#define VECBASEOP_H_

namespace nextar {
namespace Math {

template<typename T, typename impl>
inline bool VecBaseOp<T, impl>::IsNan(pref v) {
	for (uint32 i = 0; i < _count; ++i)
		if(traits::IsNan(v.v[i]))
			return true;
	return false;
}

template<typename T, typename impl>
inline bool VecBaseOp<T, impl>::IsInf(pref v) {
	for (uint32 i = 0; i < _count; ++i)
		if (traits::IsInf(v.v[i]))
			return true;
	return false;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Set(base_type val) {
	type v;
	for (uint32 i = 0; i < _count; ++i)
		v.v[i] = val;
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Set(base_type x, base_type y) {
	type v;
	v.v[0] = x;
	v.v[1] = y;
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Set(base_type x, base_type y, base_type z) {
	type v;
	v.v[0] = x;
	v.v[1] = y;
	v.v[2] = z;
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Set(base_type x, base_type y, base_type z, base_type w) {
	type v;
	v.v[0] = x;
	v.v[1] = y;
	v.v[2] = z;
	v.v[3] = w;
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Set(const base_type * val) {
	type v;
	for (uint32 i = 0; i < _count; ++i)
		v.v[i] = val[i];
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::SetX(pref v, base_type val) {
	type r = v;
	r.v[0] = val;
	return r;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::SetY(pref v, base_type val) {
	type r = v;
	r.v[1] = val;
	return r;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::SetZ(pref v, base_type val) {
	type r = v;
	r.v[2] = val;
	return r;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::SetW(pref v, base_type val) {
	type r = v;
	r.v[3] = val;
	return r;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::SplatX(pref val) {
	type v;
	for (uint32 i = 0; i < _count; ++i)
		v.v[i] = val.v[0];
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::SplatY(pref val) {
	type v;
	for (uint32 i = 0; i < _count; ++i)
		v.v[i] = val.v[1];
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::SplatZ(pref val) {
	type v;
	for (uint32 i = 0; i < _count; ++i)
		v.v[i] = val.v[2];
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::SplatW(pref val) {
	type v;
	for (uint32 i = 0; i < _count; ++i)
		v.v[i] = val.v[3];
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Zero() {
	type v;
	for (uint32 i = 0; i < _count; ++i)
		v.v[i] = 0;
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::base_type VecBaseOp<T, impl>::Get(pref v, uint32 i) {
	return v.v[i];
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Abs(pref q) {
	type v;
	for (uint32 i = 0; i < _count; ++i)
		v.v[i] = std::abs(q.v[i]);
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Negate(pref q) {
	type v;
	for (uint32 i = 0; i < _count; ++i)
		v.v[i] = -q.v[i];
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Add(pref a, pref b) {
	type v;
	for (uint32 i = 0; i < _count; ++i)
		v.v[i] = a.v[i] + b.v[i];
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Sub(pref a, pref b) {
	type v;
	for (uint32 i = 0; i < _count; ++i)
		v.v[i] = a.v[i] - b.v[i];
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Mul(pref a, pref b) {
	type v;
	for (uint32 i = 0; i < _count; ++i)
		v.v[i] = a.v[i] * b.v[i];
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Mul(pref a, base_type b) {
	type v;
	for (uint32 i = 0; i < _count; ++i)
		v.v[i] = a.v[i] + b;
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Mul(base_type b, pref a) {
	return Mul(a, b);
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Div(pref a, pref b) {
	type v;
	for (uint32 i = 0; i < _count; ++i)
		v.v[i] = a.v[i] / b.v[i];
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Madd(pref q, pref m, pref a) {
	type v;
	for (uint32 i = 0; i < _count; ++i)
		v.v[i] = q.v[i] * m.v[i] + a.v[i];
	return v;
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::base_type VecBaseOp<T, impl>::Hadd(pref q1) {
	base_type r = 0;
	for (uint32 i = 0; i < _count; ++i)
		r += v.v[i];
	return r;
}

template<typename T, typename impl>
inline bool VecBaseOp<T, impl>::GreaterAll(pref q1, pref q2) {
	for (uint32 i = 0; i < _count; ++i) {
		if (q1.v[i] <= q2.v[i])
			return false;
	}

	return true;
}

template<typename T, typename impl>
inline bool VecBaseOp<T, impl>::GreaterAny(pref q1, pref q2) {
	for (uint32 i = 0; i < _count; ++i) {
		if (q1.v[i] > q2.v[i])
			return true;
	}

	return false;
}

template<typename T, typename impl>
inline bool VecBaseOp<T, impl>::LesserAll(pref q1, pref q2) {
	for (uint32 i = 0; i < _count; ++i) {
		if (q1.v[i] >= q2.v[i])
			return false;
	}

	return true;
}

template<typename T, typename impl>
inline bool VecBaseOp<T, impl>::LesserAny(pref q1, pref q2) {
	for (uint32 i = 0; i < _count; ++i) {
		if (q1.v[i] < q2.v[i])
			return true;
	}

	return false;

}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::base_type VecBaseOp<T, impl>::Dot(pref q1, pref q2) {
	return impl::Hadd(impl::Mul(q1, q2));
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::float_type VecBaseOp<T, impl>::SqLength(pref c1) {
	return impl::Dot(c1, c1);
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::float_type VecBaseOp<T, impl>::Length(pref c1) {
	return float_type(std::sqrt(impl::SqLength(c1)));
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::float_type VecBaseOp<T, impl>::Distance(pref vec1, pref vec2) {
	return impl::Length(impl::Sub(vec2, vec1));
}

template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::float_type VecBaseOp<T, impl>::SqDistance(pref vec1, pref vec2) {
	return impl::SqLength(impl::Sub(vec2, vec1));
}


template<typename T, typename impl>
inline typename VecBaseOp<T, impl>::type VecBaseOp<T, impl>::Normalize(pref v) {
	return impl::Mul(v, 1 / impl::Length(v) );
}


}
}
#endif //VECBASEOP_H_