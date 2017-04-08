#pragma once
using namespace nextar
using namespace nextar::Math;

template <>
inline Vec2Op::type Vec2Op::Set(Vec2Op::base_type v) {
	return { v, v };
}

template <>
inline Vec2Op::type Vec2Op::Set(const Vec2Op::base_type *v) {
	return { v[0], v[1] };
}

template <>
inline Vec2Op::type Vec2Op::Zero() {
	return { 0, 0 };
}
