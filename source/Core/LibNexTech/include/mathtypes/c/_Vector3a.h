/**
 * @remarks Vec3 functions. Rename those already defined.
 **/
_NexInline Vector3A Vec3ASet(float x, float y, float z) {
	return Quad(x, y, z);
}

_NexInline Vector3A Vec3AAbs(Vec3AF q) {
	return Quad(fabsf(q.x), fabsf(q.y), fabsf(q.z));
}

_NexInline Vector3A Vec3ANegate(Vec3AF q) {
	return Quad(-(q.x), -(q.y), -(q.z));
}

_NexInline Vector3A Vec3AZero() {
	return Quad(0, 0, 0);
}

_NexInline Vector3A Vec3AFromVec3(const Vector3& vec3) {
	return Quad(vec3.x, vec3.y, vec3.z);
}

_NexInline Vector3A Vec3AMulScalar(Vec3AF v, float val) {
	return Quad(v.x * val, v.y * val, v.z * val);
}

_NexInline Vector3A Vec3AAdd(Vec3AF a, Vec3AF b) {
	return Vector3A(a.x + b.x, a.y + b.y, a.z + b.z);
}

_NexInline Vector3A Vec3ASub(Vec3AF a, Vec3AF b) {
	return Vector3A(a.x - b.x, a.y - b.y, a.z - b.z);
}

_NexInline Vector3A Vec3AMulAdd(Vec3AF a, Vec3AF f, Vec3AF b) {
	return Quad(a.x * f.x + b.x, a.y * f.y + b.y, a.z * f.z + b.z);
}

_NexInline Vector3A Vec3AMul(Vec3AF a, Vec3AF b) {
	return Vector3A(a.x * b.x, a.y * b.y, a.z * b.z);
}

_NexInline float Vec3ADot(Vec3AF vec1, Vec3AF vec2) {
	return (vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z);
}

/** @reamarks Normalize */
_NexInline Vector3A Vec3ANormalize(Vec3AF vec) {
	float val = Math::Sqrt(Vec3ADot(vec, vec));
	NEX_ASSERT(val > N3DEpsilon);
	return Vec3AMulScalar(vec, 1 / val);
}

_NexInline bool Vec3AGreaterAny(Vec4AF q1, Vec4AF q2) {
	return (q1.x > q2.x || q1.y > q2.y || q1.z > q2.z);
}

_NexInline bool Vec3AGreaterAll(Vec4AF q1, Vec4AF q2) {
	return (q1.x > q2.x && q1.y > q2.y && q1.z > q2.z);
}

_NexInline bool Vec3ALesserAny(Vec4AF q1, Vec4AF q2) {
	return (q1.x < q2.x || q1.y < q2.y || q1.z < q2.z);
}

_NexInline bool Vec3ALesserAll(Vec4AF q1, Vec4AF q2) {
	return (q1.x < q2.x && q1.y < q2.y && q1.z < q2.z);
}

_NexInline Vector3A Vec3ACross(Vec3AF v1, Vec3AF v2) {
	return Vector3A(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x);

}

/** @reamarks Lerp */
_NexInline Vector3A Vec3ALerp(Vec3AF src, Vec3AF dst, float t) {
	/* @todo Make efficient */
	return Vec3AMulAdd(Vec4AReplicate(t), Vec3ASub(dst, src), src);
}

/** @todo */
_NexInline float Vec3ASqLength(Vec3AF vec) {
	return (Vec3ADot(vec, vec));
}

/** @reamarks Normalize */
_NexInline float Vec3ALength(Vec3AF vec) {
	return Math::Sqrt(Vec3ADot(vec, vec));
}

/** @brief **/
_NexInline float Vec3ADistance(Vec3AF vec1, Vec3AF vec2) {
	return Vec3ALength(Vec3ASub(vec1, vec2));
}

/** @brief **/
_NexInline float Vec3ASqDistance(Vec3AF vec1, Vec3AF vec2) {
	return Vec3ASqLength(Vec3ASub(vec1, vec2));
}
