/**
 * @remarks Vec3 functions. Rename those already defined.
 **/
namespace nextar {

inline Vector4A Vec4AAbs(Vec4AF q) {
	return QuadInit(Math::Abs(q.x), Math::Abs(q.y), Math::Abs(q.z), Math::Abs(q.w));
}

inline Vector4A Vec4ANegate(Vec4AF q) {
	return QuadInit(-(q.x), -(q.y), -(q.z), -(q.w));
}

inline Vector4A Vec4AZero() {
	return QuadInit(0, 0, 0, 0);
}

inline Vector4A Vec4ASet(float x, float y, float z, float w) {
	return QuadInit(x, y, z, w);
}

inline Vector4A Vec4AAdd(Vec4AF a, Vec4AF b) {
	return QuadInit(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

inline Vector4A Vec4ASub(Vec4AF a, Vec4AF b) {
	return QuadInit(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

inline Vector4A Vec4AMulAdd(Vec4AF a, Vec4AF f, Vec4AF b) {
	return QuadInit(a.x * f.x + b.x, a.y * f.y + b.y, a.z * f.z + b.z,
			a.w * f.w + b.w);
}

inline Vector4A Vec4AReplicate(float v) {
	return QuadInit(v, v, v, v);
}

inline bool Vec4AGreaterAny(Vec4AF a, Vec4AF b) {
	return a.x > b.x || a.y > b.y || a.z > b.z || a.w > b.w;
}

inline bool Vec4AGreaterAll(Vec4AF a, Vec4AF b) {
	return a.x > b.x && a.y > b.y && a.z > b.z && a.w > b.w;
}

inline bool Vec4ALesserAny(Vec4AF a, Vec4AF b) {
	return a.x < b.x || a.y < b.y || a.z < b.z || a.w < b.w;
}

inline bool Vec4ALesserAll(Vec4AF a, Vec4AF b) {
	return a.x < b.x && a.y < b.y && a.z < b.z && a.w < b.w;
}

/** @reamarks Dot */
inline float Vec4ADot(Vec4AF vec1, Vec4AF vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z + vec1.w * vec2.w;
}

/** @reamarks Normalize */
inline Vector4A Vec4ANormalize(Vec4AF vec) {
	float res = Math::Sqrt(Vec4ADot(vec, vec));
	NEX_ASSERT(res > Math::EPSILON);
	return QuadInit(vec.x / res, vec.y / res, vec.z / res, vec.w / res);
}

/** @reamarks Lerp */
inline Vector4A Vec4ALerp(Vec4AF src, Vec4AF dst, float t) {
	/* todo Make efficient */
	return Vec4AMulAdd(Vec4AReplicate(t), Vec4ASub(dst, src), src);
}

/** todo */
inline float Vec4ASqLength(Vec4AF vec) {
	return (Vec4ADot(vec, vec));
}

/** @reamarks  */
inline float Vec4ALength(Vec4AF vec) {
	return Math::Sqrt(Vec4ADot(vec, vec));
}

/** @brief **/
inline float Vec4ADistance(Vec4AF vec1, Vec4AF vec2) {
	return Vec4ALength(Vec4ASub(vec1, vec2));
}

/** todo*/
inline float Vec4ASqDistance(Vec4AF vec1, Vec4AF vec2) {
	return Vec4ASqLength(Vec4ASub(vec1, vec2));
}

inline Vector4A Vec4AMul(Vec4AF a, Vec4AF b) {
	return QuadInit(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

inline Vector4A Vec4AMulScalar(Vec4AF v, float val) {
	return QuadInit(v.x * val, v.y * val, v.z * val, v.w * val);
}


inline Vector4A Vec4AMulMat4x4(Vec4AF v, Mat4x4F m) {
	Quad r, x, y, z, w;

	z = v.SplatZ();
	y = v.SplatY();
	x = v.SplatX();
	w = v.SplatW();

	r = Vec4AMul(w, m.Row(3));
	r = Vec4AMulAdd(z, m.Row(2), r);
	r = Vec4AMulAdd(y, m.Row(1), r);
	r = Vec4AMulAdd(x, m.Row(0), r);

	return r;
}

}
