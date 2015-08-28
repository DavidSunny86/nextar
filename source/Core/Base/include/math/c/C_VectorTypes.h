#ifndef NEXTAR_MATH_CTYPES_H
#define NEXTAR_MATH_CTYPES_H

namespace nextar {
struct Quad {
	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};
		float v[4];
	};

	void Load(float val);
	void Load3(const float* vals);
	void Store3(float* vals) const;
	void Load4(const float* vals);
	void Store4(float* vals) const;
	Quad SplatX() const;
	Quad SplatY() const;
	Quad SplatZ() const;
	Quad SplatW() const;

};

inline Quad QuadInit(const Quad& q);
inline Quad QuadInit(float vx, float vy, float vz);
inline Quad QuadInit(float vx, float vy, float vz, float vw);

typedef Quad Vector3A;
typedef Quad Vector4A;
typedef Quad Quaternion;
typedef Quad Plane;

typedef const Quad& QuadPF; // fast param
typedef const Quad& QuadPR; // ref param

typedef QuadPF Vec3AF;
typedef QuadPR Vec3AR;

typedef QuadPF Vec4AF;
typedef QuadPR Vec4AR;

typedef QuadPF QuatF;
typedef QuadPR QuatR;

typedef QuadPF PlaneF;
typedef QuadPR PlaneR;

inline float QuadGetX(QuadPF q);
inline float QuadGetY(QuadPF q);
inline float QuadGetZ(QuadPF q);
inline float QuadGetW(QuadPF q);
inline Quad QuadSetX(QuadPF q, float val);
inline Quad QuadSetY(QuadPF q, float val);
inline Quad QuadSetZ(QuadPF q, float val);
inline Quad QuadSetW(QuadPF q, float val);
inline Quad QuadRet(QuadPF q);
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

#endif //NEXTAR_MATH_SSETYPES_H
