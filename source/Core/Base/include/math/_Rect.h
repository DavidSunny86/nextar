#pragma once

namespace nextar {
namespace Math {

inline nextar::Math::_Rect::_Rect() {
}

inline nextar::Math::_Rect::_Rect(float left, float top, float right, float bottom) {
	leftTop = Vec2Op::Set(left, top);
	rightBottom = Vec2Op::Set(right, bottom);
}

inline TraitsVec2::type MatOp<_Rect>::GetHalfSize(pref box) {
	return Vec2::Mul(Vec2::Sub(box.max, box.min), 0.5f);
}

inline TraitsVec2::type MatOp<_Rect>::GetSize(pref box) {
	return Vec2::Sub(box.max, box.min);
}

inline TraitsVec2::type MatOp<_Rect>::GetCenter(pref box) {
	return Vec2::Mul(Vec2::Add(box.max, box.min), 0.5f);
}

}
}