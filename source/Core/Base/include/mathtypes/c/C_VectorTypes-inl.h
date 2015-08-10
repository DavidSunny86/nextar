#pragma once

namespace nextar {

inline Quad::Quad(float vx, float vy, float vz, float vw) :
		x(vx), y(vy), z(vz), w(vw) {
}

inline Quad::Quad(float vx, float vy, float vz) :
		x(vx), y(vy), z(vz), w(0) {
}

inline Quad::Quad(const Quad& q) :
		x(q.x), y(q.y), z(q.z), w(q.w) {
}

inline Quad::Quad() {
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
	return Quad(x, x, x, x);
}

inline Quad Quad::SplatY() const {
	return Quad(y, y, y, y);
}

inline Quad Quad::SplatZ() const {
	return Quad(z, z, z, z);
}

inline Quad Quad::SplatW() const {
	return Quad(w, w, w, w);
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
	return Quad(val, q.y, q.z, q.w);
}

inline Quad QuadSetY(QuadPF q, float val) {
	return Quad(q.x, val, q.z, q.w);
}

inline Quad QuadSetZ(QuadPF q, float val) {
	return Quad(q.x, q.y, val, q.w);
}

inline Quad QuadSetW(QuadPF q, float val) {
	return Quad(q.x, q.y, q.z, val);
}

inline Quad QuadRet(QuadPF q) {
	return q;
}

/** definitions come here */
#define Vec4AGetX		QuadGetX
#define Vec4AGetY		QuadGetY
#define Vec4AGetZ		QuadGetZ
#define Vec4AGetW		QuadGetW
#define Vec4ASetX		QuadSetX
#define Vec4ASetY		QuadSetY
#define Vec4ASetZ		QuadSetZ
#define Vec4ASetW		QuadSetW
/**
 * @remarks Vec3 functions
 **/
#define Vec3AGetX		QuadGetX
#define Vec3AGetY		QuadGetY
#define Vec3AGetZ		QuadGetZ
#define Vec3ASetX		QuadSetX
#define Vec3ASetY		QuadSetY
#define Vec3ASetZ		QuadSetZ
#define Vec3AMulMat4x4  Mat4x4TransVec3A
#define Vec3AFromVec4A  QuadRet

#define PlaneNormalize		Vec3ANormalize
#define PlaneNormalDotVec3A     Vec3ADot
#define PlaneAbsNormal          QuadAbs

#define QuatGetX QuadGetX
#define QuatGetY QuadGetY
#define QuatGetZ QuadGetZ
#define QuatGetW QuadGetW
#define QuatNormalize Vec4ANormalize
#define QuatDot Vec4ADot
#define QuatGreaterAny Vec4AGreaterAny
#define QuatGreaterAll Vec4AGreaterAll
#define QuatLesserAny Vec4ALesserAny
#define QuatLesserAll Vec4ALesserAll
#define QuatMulScalar Vec4AMulScalar

}

#include <mathtypes/c/Vector4a-inl.h>
#include <mathtypes/c/Vector3a-inl.h>
#include <mathtypes/c/Plane-inl.h>
#include <mathtypes/c/Quaternion-inl.h>
#include <mathtypes/c/Matrix3x4-inl.h>
#include <mathtypes/c/Matrix4x4-inl.h>

