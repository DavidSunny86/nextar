/*
 * Matrix4x4.h
 *
 *  Created on: Feb 10, 2014
 *      Author: obhi
 */

#ifndef MATRIX4X4_H_
#define MATRIX4X4_H_

#include <MathBase.h>
#include <MathTypes.h>

namespace nextar {
/**
 * @ Matrix4x4
 **/
#ifndef Mat4x4Row
#	define Mat4x4Row(m, i) (m).r[i]
#endif

/** @brief	Matrix4x4 muliplication. **/
inline Matrix4x4 Mat4x4Mul(Mat4x4F m1, Mat4x4F m2);
/** @brief	Matrix4x4 from vector mapping. **/
inline Matrix4x4 Mat4x4FromVectorMapping(const Vector3& axis1,
		const Vector3& axis2);
/** @brief	Matrix4x4 Transform vertices and project the w coord as 1.0. **/
inline void Mat4x4TransAndProjVec3(Vector3* outstream, uint32 outstride,
		const Vector3* inpstream, uint32 inpstride, uint32 count, Mat4x4F m2);
/** @brief	Matrix4x4 Transform vertices. **/
inline void Mat4x4TransVec3(Vector4A* outstream, uint32 outstride,
		const Vector3* inpstream, uint32 inpstride, uint32 count, Mat4x4F m2);
/** @brief	Matrix4x4 Transform vertices. **/
inline void Mat4x4TransVec3(Vector4* outstream, uint32 outstride,
		const Vector3* inpstream, uint32 inpstride, uint32 count, Mat4x4F m2);
/** @brief	Matrix4x4 Transform vertex. **/
inline Vector3A Mat4x4TransAndProjVec3A(Vec3AF v, Mat4x4F m2);
/** @brief	Matrix4x4 Transform vertex. **/
inline Vector4A Mat4x4TransVec3A(Vec3AF v, Mat4x4F m2);
/** @brief	Matrix4x4 special transform for AABB bound radius. **/
inline Vector4A Mat4x4TransBoundRadius(Vec3AF v, Mat4x4F m);
/** @brief	Matrix4x4 from rotation and postion. **/
inline Matrix4x4 Mat4x4FromScaleRotPos(float scale, QuatF rot, Vec3AF pos);
/** @brief	Matrix4x4 from scaling. **/
inline Matrix4x4 Mat4x4FromScale(Vec3AF scale);
/** @brief	Matrix4x4 from quaternion. **/
inline Matrix4x4 Mat4x4FromRot(QuatF rot);
/** @brief	Matrix4x4 from translation. **/
inline Matrix4x4 Mat4x4FromPos(Vec3AF rot);
/** @brief	Scale a Matrix4x4 uniformly. **/
inline Matrix4x4 Mat4x4Scale(float scale, Mat4x4F m);
/** @brief Matrix4x4 transpose **/
inline Matrix4x4 Mat4x4Transpose(Mat4x4F m);
/** @brief Matrix4x4 transpose **/
inline Matrix4x4 Mat4x4ViewFromWorld(Mat4x4F m);
/** @brief Matrix4x4 Creates a camera look at matrix **/
inline Matrix4x4 Mat4x4FromCameraLookAt(Vec3AF eye, Vec3AF lookat, Vec3AF vup);
/** @brief Orthogonal matrix */
inline Matrix4x4 Mat4x4FromOrtho(float width, float height, float nearPlane,
		float farPlane);
/** @brief Orthogonal matrix */
inline Matrix4x4 Mat4x4FromPerspective(float fieldOfView, float aspectRatio,
		float nearPlane, float farPlane);
/** @urgent generic matrix inverse */
inline Matrix4x4 Mat4x4Inverse(Mat4x4F m);

}

#endif /* MATRIX4X4_H_ */
