#ifndef NEXTAR__CORE_FUNCTION_DECL_H
#define NEXTAR__CORE_FUNCTION_DECL_H

/**
 * @ Vector4A
 **/

/** @brief	Vector 4 set 0. **/
_NexInline Vector4A Vec4AZero();
/** @brief	Vector 4 set x,y,z,w. **/
_NexInline Vector4A Vec4ASet(float x, float y, float z, float w);
/** @brief	Vector 4 get x coordinate. **/
_NexInline float Vec4AGetX(Vec4AF q);
/** @brief	Vector 4 get y coordinate. **/
_NexInline float Vec4AGetY(Vec4AF q);
/** @brief	Vector 4 get z coordinate. **/
_NexInline float Vec4AGetZ(Vec4AF q);
/** @brief	Vector 4 get w coordinate. **/
_NexInline float Vec4AGetW(Vec4AF q);
/** @brief	Vector 4 dot x1*x2 + y1*y2 + z1*z2 + w1*w2. **/
_NexInline float Vec4ADot(Vec4AF vec1, Vec4AF vec2);
/** @brief	Abs Vector4A **/
_NexInline Vector4A Vec4AAbs(Vec4AF vec3);
/** @brief	Negate Vector4A **/
_NexInline Vector4A Vec4ANegate(Vec4AF vec3);
/** @brief	Vector 4 Add. **/
_NexInline Vector4A Vec4AAdd(Vec4AF q1, Vec4AF q2);
/** @brief	Vector 4 Sub. **/
_NexInline Vector3A Vec4ASub(Vec4AF q1, Vec4AF q2);
/** @brief	Vector 4 Multiply and Add. **/
_NexInline Vector4A Vec4AMulAdd(Vec4AF q1, Vec4AF q2, Vec4AF q3);
/** @brief	Vector 4 Replicate a value. **/
_NexInline Vector4A Vec4AReplicate(float value);
/** @brief	Vector 4 Normalize (vec/sqrt(x*x + y*y + z*z + w*w)) **/
_NexInline Vector4A Vec4ANormalize(Vec4AF vec);
/** @brief	Return true if any element in vec1 is greater than vec2 **/
_NexInline bool Vec4AGreaterAny(Vec4AF vec1, Vec4AF vec2);
/** @brief	Return true if all elements in vec1 is greater than vec2 **/
_NexInline bool Vec4AGreaterAll(Vec4AF vec1, Vec4AF vec2);
/** @brief	Return true if any element in vec1 is lesser than vec2 **/
_NexInline bool Vec4ALesserAny(Vec4AF vec1, Vec4AF vec2);
/** @brief	Return true if all elements in vec1 is lesser than vec2 **/
_NexInline bool Vec4ALesserAll(Vec4AF vec1, Vec4AF vec2);
/** @brief Interpolate */
_NexInline Vector4A Vec4ALerp(Vec4AF src, Vec4AF dst, float t);
/** @brief	Vector 4 sqrt(x*x + y*y + z*z + w*w) **/
_NexInline float Vec4ALength(Vec4AF vec);
/** @brief	Vector 4 (x*x + y*y + z*z + w*w) **/
_NexInline float Vec4ASqLength(Vec4AF vec);
/** @brief	Vector 4  length(vec1-vec2)**/
_NexInline float Vec4ADistance(Vec4AF vec1, Vec4AF vec2);
/** @brief	Vector 4  sqlength(vec1-vec2)**/
_NexInline float Vec4ASqDistance(Vec4AF vec1, Vec4AF vec2);
/**
 * @ Vector3A
 **/
/** @brief	Vector 3 set 0. **/
_NexInline Vector3A Vec3AZero();
/** @brief	Vector 3 set x,y,z,w. **/
_NexInline Vector3A Vec3ASet(float x, float y, float z);
/** @brief	Vector 3 get x coordinate. **/
_NexInline float Vec3AGetX(Vec3AF q);
/** @brief	Vector 3 get y coordinate. **/
_NexInline float Vec3AGetY(Vec3AF q);
/** @brief	Vector 3 get z coordinate. **/
_NexInline float Vec3AGetZ(Vec3AF q);
/** @brief	Vector 3 dot product. **/
_NexInline float Vec3ADot(Vec3AF vec1, Vec3AF vec2);
/** @brief	Vector3A from Vector3. **/
_NexInline Vector3A Vec3AFromVec3(const Vector3& vec3);
/** @brief	Vector3A Add. **/
_NexInline Vector3A Vec3AAdd(Vec3AF q1, Vec3AF q2);
/** @brief	Vector3A Sub. **/
_NexInline Vector3A Vec3ASub(Vec3AF q1, Vec3AF q2);
/** @brief	Vector3A Mul. **/
_NexInline Vector3A Vec3AMul(Vec3AF q1, Vec3AF q2);
/** @brief	Abs Vector4A **/
_NexInline Vector3A Vec3AAbs(Vec3AF vec3);
/** @brief	Negate Vector3A **/
_NexInline Vector3A Vec3ANegate(Vec3AF vec3);
/** @brief	Multiply scalar to Vector3A **/
_NexInline Vector3A Vec3AMulScalar(Vec3AF v, float fac);
/** @brief	Result = Vector3A*Mat4x4. Same as  Mat4x4TransVec3 **/
_NexInline Vector3A Vec3AMulMat4x4(Vec3AF v, Mat4x4F m2);
/** @brief	Vector 3 Multiply and Add q1*q2 + q3. **/
_NexInline Vector3A Vec3AMulAdd(Vec3AF q1, Vec3AF q2, Vec3AF q3);
/** @brief	Vector 3 Normalize (vec/sqrt(x*x + y*y + z*z)) **/
_NexInline Vector3A Vec3ANormalize(Vec3AF vec);
/** @brief	Return true if any element(x,y,z) in vec1 is greater than vec2 **/
_NexInline bool Vec3AGreaterAny(Vec3AF vec1, Vec3AF vec2);
/** @brief	Return true if all elements(x,y,z) in vec1 is greater than vec2 **/
_NexInline bool Vec3AGreaterAll(Vec3AF vec1, Vec3AF vec2);
/** @brief	Return true if any element(x,y,z) in vec1 is lesser than vec2 **/
_NexInline bool Vec3ALesserAny(Vec3AF vec1, Vec3AF vec2);
/** @brief	Return true if all elements(x,y,z) in vec1 is lesser than vec2 **/
_NexInline bool Vec3ALesserAll(Vec3AF vec1, Vec3AF vec2);
/** @brief	Return true if all elements(x,y,z) in vec1 is lesser than vec2 **/
_NexInline Vector3A Vec3ACross(Vec3AF vec1, Vec3AF vec2);
/** @brief Interpolate */
_NexInline Vector3A Vec3ALerp(Vec3AF src, Vec3AF dest, float t);
/** @brief	Vector 3 sqrt(x*x + y*y + z*z) **/
_NexInline float Vec3ALength(Vec3AF vec);
/** @brief	Vector 3 (x*x + y*y + z*z) **/
_NexInline float Vec3ASqLength(Vec3AF vec);
/** @brief	Vector 3  length(vec1-vec2)**/
_NexInline float Vec3ADistance(Vec3AF vec1, Vec3AF vec2);
/** @brief	Vector 3  sqlength(vec1-vec2)**/
_NexInline float Vec3ASqDistance(Vec3AF vec1, Vec3AF vec2);

/**
 * @ Quaternion
 **/
/** @brief Quaternion set **/
_NexInline Quaternion QuatSet(float x, float y, float z, float w);
/** @brief Quaternion get identity quat. **/
_NexInline Quaternion QuatIdentity();
/** @brief Quaternion get x coordinate. **/
_NexInline float QuatGetX(QuatF q);
/** @brief Quaternion get y coordinate. **/
_NexInline float QuatGetY(QuatF q);
/** @brief Quaternion get z coordinate. **/
_NexInline float QuatGetZ(QuatF q);
/** @brief Quaternion get w coordinate. **/
_NexInline float QuatGetW(QuatF q);
/** @brief Quaternion from axis angle. **/
_NexInline Quaternion QuatFromAxisAng(const Vector3& axis, float ang);
/** @brief Quaternion from axis angle. **/
_NexInline Quaternion QuatFromAxisAng(Vec3AF axis, float ang);
/** @brief Quaternion from rotation matrix */
_NexInline Quaternion QuatFromMat4x4(Mat4x4F m);
/** @brief Quaternion from rotation matrix */
_NexInline Quaternion QuatFromMat3x4(Mat3x4F m);
/** Multiply two quaternions */
_NexInline Quaternion QuatMul(QuatF q1, QuatF q2);
/** Rotate/Transform a vector using a quaternion */
_NexInline Vector3A QuatTransVec3A(QuatF q, Vec3AF v);
/** Transform bound radius by quaternion */
_NexInline Vector3A QuatTransBoundRadius(QuatF q, Vec3AF r);
/** Perform quaternion slerp */
_NexInline Quaternion QuatSlerp(QuatF qStart, QuatF qEnd, float amount);
/** Perform quaternion lerp */
_NexInline Quaternion QuatLerp(QuatF from, QuatF to, float t);
/** Quaternion normalize */
_NexInline Quaternion QuatNormalize(QuatF vec);
/** Quaternion dot */
_NexInline float QuatDot(QuatF q1, QuatF q2);
/** @brief	Return true if any element in vec1 is greater than vec2 **/
_NexInline bool QuatGreaterAny(QuatF q1, QuatF q2);
/** @brief	Return true if all elements in q1 is greater than q2 **/
_NexInline bool QuatGreaterAll(QuatF q1, QuatF q2);
/** @brief	Return true if any element in q1 is lesser than q2 **/
_NexInline bool QuatLesserAny(QuatF q1, QuatF q2);
/** @brief	Return true if all elements in q1 is lesser than q2 **/
_NexInline bool QuatLesserAll(QuatF q1, QuatF q2);
/** @brief Returns the inverse of the quaternion, assuming the
 * quaternion has been normalized */
_NexInline Quaternion QuatInverse(QuatF q);

/**
 * @ Matrix3x4
 **/

/** @brief	Matrix3x4 from vector mapping creates a matrix that
 can rotate the vector axis1 to axis2 when post multiplied to axis1. **/
_NexInline Matrix3x4 Mat3x4FromVectorMapping(const Vector3& axis1,
		const Vector3& axis2);
/** @brief Matrix3x4 transpose **/
_NexInline Matrix3x4 Mat3x4Transpose(Mat3x4F m);
/** @brief rotate vector **/
_NexInline Vec3AF Mat3x4TransVec3A(Vec3AF v, Mat3x4F m);
/** @brief Returns a look at matrix based on a look at vector and up position **/
_NexInline Matrix3x4 Mat3x4FromViewUp(Vec3AF viewDir, Vec3AF upDir);

/**
 * @ Matrix4x4
 **/
#ifndef Mat4x4Row
#   error Matrix row function missing
#endif

/** @brief	Matrix4x4 muliplication. **/
_NexInline Matrix4x4 Mat4x4Mul(Mat4x4F m1, Mat4x4F m2);
/** @brief	Matrix4x4 from vector mapping. **/
_NexInline Matrix4x4 Mat4x4FromVectorMapping(const Vector3& axis1,
		const Vector3& axis2);
/** @brief	Matrix4x4 Transform vertices and project the w coord as 1.0. **/
_NexInline void Mat4x4TransAndProjVec3(Vector3* outstream, uint32 outstride,
		const Vector3* inpstream, uint32 inpstride, uint32 count, Mat4x4F m2);
/** @brief	Matrix4x4 Transform vertices. **/
_NexInline void Mat4x4TransVec3(Vector4A* outstream, uint32 outstride,
		const Vector3* inpstream, uint32 inpstride, uint32 count, Mat4x4F m2);
/** @brief	Matrix4x4 Transform vertices. **/
_NexInline void Mat4x4TransVec3(Vector4* outstream, uint32 outstride,
		const Vector3* inpstream, uint32 inpstride, uint32 count, Mat4x4F m2);
/** @brief	Matrix4x4 Transform vertex. **/
_NexInline Vector3A Mat4x4TransAndProjVec3A(Vec3AF v, Mat4x4F m2);
/** @brief	Matrix4x4 Transform vertex. **/
_NexInline Vector4A Mat4x4TransVec3A(Vec3AF v, Mat4x4F m2);
/** @brief	Matrix4x4 special transform for AABB bound radius. **/
_NexInline Vector4A Mat4x4TransBoundRadius(Vec3AF v, Mat4x4F m);
/** @brief	Matrix4x4 from rotation and postion. **/
_NexInline Matrix4x4 Mat4x4FromScaleRotPos(float scale, QuatF rot, Vec3AF pos);
/** @brief Matrix4x4 transpose **/
_NexInline Matrix4x4 Mat4x4Transpose(Mat4x4F m);
/** @brief Matrix4x4 transpose **/
_NexInline Matrix4x4 Mat4x4ViewFromWorld(Mat4x4F m);
/** @brief Matrix4x4 Creates a camera look at matrix **/
_NexInline Matrix4x4 Mat4x4FromCameraLookAt(Vec3AF eye, Vec3AF lookat,
		Vec3AF vup);
/** @brief Orthogonal matrix */
_NexInline Matrix4x4 Mat4x4FromOrtho(float width, float height, float nearPlane,
		float farPlane);
/** @brief Orthogonal matrix */
_NexInline Matrix4x4 Mat4x4FromPerspective(float fieldOfView, float aspectRatio,
		float nearPlane, float farPlane);

/**
 * @ Plane
 **/
/** @brief	Vector 4 Normalize (vec/sqrt(x*x + y*y + z*z)) **/
_NexInline Plane PlaneNormalize(PlaneF vec);
/** @brief	Dot operation (plane.normal dot vec) **/
_NexInline float PlaneNormalDotVec3A(PlaneF p, Vec3AF vec);
/** @brief	Dot operation (plane.normal dot vec) + d **/
_NexInline float PlaneDotVec3A(PlaneF p, Vec3AF vec);
/** @brief	Returns abs(plane.normal) **/
_NexInline Vector3A PlaneAbsNormal(PlaneF p);
#endif //NEXTAR__CORE_FUNCTION_DECL_H
