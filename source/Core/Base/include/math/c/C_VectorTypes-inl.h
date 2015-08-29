#pragma once

namespace nextar {

inline Quad QuadInit(float vx, float vy, float vz, float vw) {
	Quad q = {vx, vy, vz, vw};
	return q;
}

inline Quad QuadInit(float vx, float vy, float vz) {
	Quad q = {vx, vy, vz, 0};
	return q;
}

inline Quad QuadInit(const Quad& q) {
	return q;
}

inline void Quad::Load(float val) {
	x = y = z = w = val;
}

inline void Quad::Load3(const float* vals) {
	x = vals[0];
	y = vals[1];
	z = vals[2];
}

inline void Quad::Store3(float* vals) const {
	vals[0] = x;
	vals[1] = y;
	vals[2] = z;
}

inline void Quad::Load4(const float* vals) {
	x = vals[0];
	y = vals[1];
	z = vals[2];
	w = vals[3];
}

inline void Quad::Store4(float* vals) const {
	vals[0] = x;
	vals[1] = y;
	vals[2] = z;
	vals[3] = w;
}

inline Quad Quad::SplatX() const {
	return QuadInit(x, x, x, x);
}

inline Quad Quad::SplatY() const {
	return QuadInit(y, y, y, y);
}

inline Quad Quad::SplatZ() const {
	return QuadInit(z, z, z, z);
}

inline Quad Quad::SplatW() const {
	return QuadInit(w, w, w, w);
}

/** Inlined functions **/
inline float QuadGetX(QuadPF q) {
	return q.x;
}

inline float QuadGetY(QuadPF q) {
	return q.y;
}

inline float QuadGetZ(QuadPF q) {
	return q.z;
}

inline float QuadGetW(QuadPF q) {
	return q.w;
}

inline Quad QuadSetX(QuadPF q, float val) {
	return QuadInit(val, q.y, q.z, q.w);
}

inline Quad QuadSetY(QuadPF q, float val) {
	return QuadInit(q.x, val, q.z, q.w);
}

inline Quad QuadSetZ(QuadPF q, float val) {
	return QuadInit(q.x, q.y, val, q.w);
}

inline Quad QuadSetW(QuadPF q, float val) {
	return QuadInit(q.x, q.y, q.z, val);
}

inline Quad QuadRet(QuadPF q) {
	return q;
}

}

#include <math/c/Vector4A-inl.h>
#include <math/c/Vector3A-inl.h>
#include <math/c/Plane-inl.h>
#include <math/c/Quaternion-inl.h>
#include <math/c/Matrix3x4-inl.h>
#include <math/c/Matrix4x4-inl.h>
#include <math/c/AxisAlignedBox-inl.h>

