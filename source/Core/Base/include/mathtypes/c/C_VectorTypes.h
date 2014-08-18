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

	Quad();
	Quad(const Quad& q);
	Quad(float vx, float vy, float vz);
	Quad(float vx, float vy, float vz, float vw);

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

}

#endif //NEXTAR_MATH_SSETYPES_H
