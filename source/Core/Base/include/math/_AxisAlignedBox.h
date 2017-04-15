#pragma once
namespace nextar {
namespace Math {

inline _AxisAlignedBox::_AxisAlignedBox() {
}

_AxisAlignedBox::_AxisAlignedBox(const Vector3A& minP, const Vector3A& maxP) {
	minPoint = minP;
	maxPoint = maxP;
}

_AxisAlignedBox::_AxisAlignedBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) {
	minPoint = Vec3AOp::Set(minX, minY, minZ);
	maxPoint = Vec3AOp::Set(maxX, maxY, maxZ);
}

_AxisAlignedBox::_AxisAlignedBox(std::initializer_list<float> l) {
	std::copy(l.begin(), l.end(), m);
}

inline bool MatOp<_AxisAlignedBox>::IsValid(pref box) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return _mm_movemask_epi8(_mm_castps_si128(_mm_cmplt_ps(box.maxPoint, box.minPoint))) == 0;
#else
	for (int i = 0; i < 3; ++i)
		if (box.maxPoint.v[i] < box.minPoint.v[i])
			return false;
	return true;
#endif
}

inline TraitsVec3A::type MatOp<_AxisAlignedBox>::GetCenter(pref box) {
	return Vec3A::Mul(Vec3A::Add(box.maxPoint, box.minPoint), 0.5f);
}

inline TraitsVec3A::type MatOp<_AxisAlignedBox>::GetSize(pref box) {
	return Vec3A::Mul(Vec3A::Sub(box.maxPoint, box.minPoint), 0.5f);
}

inline TraitsVec3A::type MatOp<_AxisAlignedBox>::GetPoint(pref box, unsigned int i) {
	return Vec3AOp::Set(
		Vec3AOp::GetX(box.extremes[((i >> 2) & 1)]), 
		Vec3AOp::GetY(box.extremes[((i >> 1) & 1)]), 
		Vec3AOp::GetZ(box.extremes[((i >> 0) & 1)]));
}

inline MatOp<_AxisAlignedBox>::type MatOp<_AxisAlignedBox>::Union(pref b, TraitsVec3A::pref point) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return  _AxisAlignedBox( _mm_min_ps(b.minPoint, point),
		_mm_max_ps(b.maxPoint, point) );
#else
	AxisAlignedBox box = b;
	for (int i = 0; i < 3; ++i) {
		if (box.minPoint.v[i] > point.v[i])
			box.minPoint.v[i] = point.v[i];
		if (box.maxPoint.v[i] < point.v[i])
			box.maxPoint.v[i] = point.v[i];
	}
	return box;
#endif
}

inline MatOp<_AxisAlignedBox>::type MatOp<_AxisAlignedBox>::Union(pref box, pref other) {
#if NEX_VECTOR_MATH_TYPE_IS_SSE
	return  _AxisAlignedBox(
		_mm_min_ps(box.minPoint, other.minPoint),
		_mm_max_ps(box.maxPoint, other.maxPoint)
	);
#else
	AxisAlignedBox ret;
	for (int i = 0; i < 3; ++i) {
		ret.minPoint.v[i] = (box.minPoint.v[i] > other.minPoint.v[i]) ?
			other.minPoint.v[i] : box.minPoint.v[i];
		ret.maxPoint.v[i] = (box.maxPoint.v[i] < other.maxPoint.v[i]) ?
			box.maxPoint.v[i] : other.maxPoint.v[i];
	}
	return box;
#endif
}

}
}