/**
 * @remarks Vec3 functions. Rename those already defined.
 **/
inline Vector4A Vec4AAbs(Vec4AF q) {
	return Quad(Math::Abs(q.x), Math::Abs(q.y), Math::Abs(q.z), Math::Abs(q.w));
}

inline Vector4A Vec4ANegate(Vec4AF q) {
	return Quad(-(q.x), -(q.y), -(q.z), -(q.w));
}

inline Vector4A Vec4AZero() {
	return Quad(0, 0, 0, 0);
}

inline Vector4A Vec4ASet(float x, float y, float z, float w) {
	return Quad(x, y, z, w);
}

inline Vector4A Vec4AAdd(Vec4AF a, Vec4AF b) {
	return Quad(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

inline Vector4A Vec4ASub(Vec4AF a, Vec4AF b) {
	return Quad(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

inline Vector4A Vec4AMulAdd(Vec4AF a, Vec4AF f, Vec4AF b) {
	return Quad(a.x * f.x + b.x, a.y * f.y + b.y, a.z * f.z + b.z,
			a.w * f.w + b.w);
}

inline Vector4A Vec4AReplicate(float v) {
	return Quad(v, v, v, v);
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
	return Quad(vec.x / res, vec.y / res, vec.z / res, vec.w / res);
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
	return Quad(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}
