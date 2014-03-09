#ifndef NEXTAR_VECTOR4_H
#define NEXTAR_VECTOR4_H

//# generic vector4 class
//# vector4 is not aligned and operations in vector4
//# mostly avoid SSE or other optimization.

class _NexBaseExport Vector4 : public nextar::AllocGeneral {
public:
	float x, y, z, w;
	//# construction and implicit conversions

	inline Vector4() {
	}

	inline Vector4(const float * const v) : x(v[0]), y(v[1]),
	z(v[2]), w(v[3]) {
	}

	inline Vector4(const float fx, const float fy,
			const float fz, float fw) : x(fx), y(fy), z(fz), w(fw) {
	}

	inline Vector4(float v) : x(v), y(v), z(v), w(v) {
	}

	inline Vector4(const Vector4 & cp) :
	x(cp.x), y(cp.y), z(cp.z), w(cp.w) {

	}
};

#endif //NEXTAR_VECTOR4_H
