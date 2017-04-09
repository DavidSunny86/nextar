
#pragoma once

namespace nextar {
namespace Math {

template <typename T>
MatBaseOp<T>::row_type MatBaseOp::Row(pref m, uint32 i) const {
	return m.r[i];
}

template <typename T>
MatBaseOp<T>::base_type MatBaseOp::Get(pref m, uint32 i, uint32 j) const {
	return VecOp<row_type>::Get(m.r[i], j);
}

template<typename T>
inline _Mat4Op<T>::type _Mat4Op<T>::FromVectorMapping(TraitsVec3::pref v1, TraitsVec3::pref v2) {
	return type();
}

template<typename T>
inline void _Mat4Op<T>::Rotate(pref m, TraitsVec3::type * iStream, uint32 inStride, uint32 count) {
}

template<typename T>
inline TraitsVec3A::type _Mat4Op<T>::Rotate(pref m, TraitsVec3A::pref v) {
	return TraitsVec3A::type();
}

template<typename T>
inline _Mat4Op<T>::type _Mat4Op<T>::FromQuat(TraitsQuat::pref rot) {
	return type();
}


}
}