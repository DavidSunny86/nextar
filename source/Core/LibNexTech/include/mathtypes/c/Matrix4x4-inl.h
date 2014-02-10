namespace nextar {

#include "NexGenericTypes.h"

	inline void Mat4x4TransVec3(Vector4* outstream, uint32 outstride,
			const Vector3* inpstream, uint32 inpstride, uint32 count,
			Mat4x4F m) {
		NEX_ASSERT(outstream);
		NEX_ASSERT(inpstream);
		const uint8* inpVec = (const uint8*) inpstream;
		uint8* outVec = (uint8*) outstream;

		Quad v, x, y, z, r;

		for (uint32 i = 0; i < count; i++) {

			v.Load3((float*) inpVec);

			z = v.SplatZ();
			y = v.SplatY();
			x = v.SplatX();

			r = Vec3AMulAdd(z, m.Row(2), m.Row(3));
			r = Vec3AMulAdd(y, m.Row(1), r);
			r = Vec3AMulAdd(x, m.Row(0), r);

			r.Store4((float*) outVec);

			inpVec += inpstride;
			outVec += outstride;
		}
	}

	inline void Mat4x4TransAndProjVec3(Vector3* outstream, uint32 outstride,
			const Vector3* inpstream, uint32 inpstride, uint32 count,
			Mat4x4F m) {
		NEX_ASSERT(outstream);
		NEX_ASSERT(inpstream);

		NEX_ASSERT(outstream);
		NEX_ASSERT(inpstream);
		const uint8* inpVec = (const uint8*) inpstream;
		uint8* outVec = (uint8*) outstream;

		Quad v, x, y, z, r, w;

		for (uint32 i = 0; i < count; i++) {

			v.Load3((float*) inpVec);

			z = v.SplatZ();
			y = v.SplatY();
			x = v.SplatX();

			r = Vec3AMulAdd(z, m.Row(2), m.Row(3));
			r = Vec3AMulAdd(y, m.Row(1), r);
			r = Vec3AMulAdd(x, m.Row(0), r);
			r = Vec3AMulScalar(r, 1.f / r.w);

			r.Store4((float*) outVec);

			inpVec += inpstride;
			outVec += outstride;
		}
	}

	inline void Mat4x4TransVec3(Vector4A* outstream, uint32 outstride,
			const Vector3* inpstream, uint32 inpstride, uint32 count,
			Mat4x4F m2) {
		Mat4x4TransVec3((Vector4*) outstream, outstride, inpstream, inpstride,
				count, m2);
	}

	inline Vector3A Mat4x4TransAndProjVec3A(Vec3AF v, Mat4x4F m) {
		Quad r, x, y, z;

		z = v.SplatZ();
		y = v.SplatY();
		x = v.SplatX();

		r = Vec3AMulAdd(z, m.Row(2), m.Row(3));
		r = Vec3AMulAdd(y, m.Row(1), r);
		r = Vec3AMulAdd(x, m.Row(0), r);
		r = Vec3AMulScalar(r, 1.f / r.w);

		return r;
	}

	inline Vector4A Mat4x4TransVec3A(Vec3AF v, Mat4x4F m) {
		Quad r, x, y, z;

		z = v.SplatZ();
		y = v.SplatY();
		x = v.SplatX();

		r = Vec3AMulAdd(z, m.Row(2), m.Row(3));
		r = Vec3AMulAdd(y, m.Row(1), r);
		r = Vec3AMulAdd(x, m.Row(0), r);

		return r;
	}

	inline Vector4A Mat4x4TransBoundRadius(Vec3AF v, Mat4x4F m) {
		Vector3A ret;
		for (int i = 0; i < 3; i++) {
			ret.v[i] = 0.0f;
			for (int j = 0; j < 3; j++) {
				ret.v[i] += v.v[j] * Math::Abs(m.m[i + j * 4]);
			}
		}
		return ret;
	}

	inline Matrix4x4 Mat4x4FromVectorMapping(const Vector3& axis1,
			const Vector3& axis2) {
		Matrix3x4 m = Mat3x4FromVectorMapping(axis1, axis2);
		return Matrix4x4(m, Matrix4x4::IdentityMatrix.Row(3));
	}

	inline Matrix4x4 Mat4x4FromVectorMapping(const Vector3& axis1,
			const Vector3& axis2, const Vector3& t) {
		Matrix3x4 m = Mat3x4FromVectorMapping(axis1, axis2);
		return Matrix4x4(m, Vec4ASet(t.x, t.y, t.z, 1));
	}

	inline Matrix4x4 Mat4x4Mul(Mat4x4F m1, Mat4x4F m2) {
		Matrix4x4 res;
		for (int i = 0; i < 4; ++i) {
			float x = m1.m[i * 4 + 0];
			float y = m1.m[i * 4 + 1];
			float z = m1.m[i * 4 + 2];
			float w = m1.m[i * 4 + 3];
			res.m[i * 4 + 0] = (m2.m[0 * 4 + 0] * x) + (m2.m[1 * 4 + 0] * y)
					+ (m2.m[2 * 4 + 0] * z) + (m2.m[3 * 4 + 0] * w);
			res.m[i * 4 + 1] = (m2.m[0 * 4 + 1] * x) + (m2.m[1 * 4 + 1] * y)
					+ (m2.m[2 * 4 + 1] * z) + (m2.m[3 * 4 + 1] * w);
			res.m[i * 4 + 2] = (m2.m[0 * 4 + 2] * x) + (m2.m[1 * 4 + 2] * y)
					+ (m2.m[2 * 4 + 2] * z) + (m2.m[3 * 4 + 2] * w);
			res.m[i * 4 + 3] = (m2.m[0 * 4 + 3] * x) + (m2.m[1 * 4 + 3] * y)
					+ (m2.m[2 * 4 + 3] * z) + (m2.m[3 * 4 + 3] * w);
		}

		return res;
	}

	Matrix4x4::Matrix4x4(const Matrix3x4& m, const Vector4A& v) {

		Row(0) = m.Row(0);
		Row(1) = m.Row(1);
		Row(2) = m.Row(2);
		Row(3) = v;
	}

	Matrix4x4::Matrix4x4(float m00, float m01, float m02, float m03, float m10,
			float m11, float m12, float m13, float m20, float m21, float m22,
			float m23, float m30, float m31, float m32, float m33) {
		Row(0) = Vec4ASet(m00, m01, m02, m03);
		Row(1) = Vec4ASet(m10, m11, m12, m13);
		Row(2) = Vec4ASet(m20, m21, m22, m23);
		Row(3) = Vec4ASet(m30, m31, m32, m33);
	}

	/** C++ operators and functions **/
	Matrix4x4& Matrix4x4::operator=(const Matrix4x4& m) {
		Row(0) = m.Row(0);
		Row(1) = m.Row(1);
		Row(2) = m.Row(2);
		Row(3) = m.Row(3);
		return *this;
	}

	inline Matrix4x4 Mat4x4FromScaleRotPos(float scale, QuatF rot, Vec3AF pos) {
		Matrix4x4 ret;
		float xx = rot.x * rot.x;
		float yy = rot.y * rot.y;
		float zz = rot.z * rot.z;
		float xy = rot.x * rot.y;
		float xz = rot.x * rot.z;
		float yz = rot.y * rot.z;
		float wx = rot.w * rot.x;
		float wy = rot.w * rot.y;
		float wz = rot.w * rot.z;

		ret.m00 = scale * (1 - 2 * (yy + zz));
		ret.m01 = scale * (2 * (xy + wz));
		ret.m02 = scale * (2 * (xz - wy));

		ret.m10 = scale * (2 * (xy - wz));
		ret.m11 = scale * (1 - 2 * (xx + zz));
		ret.m12 = scale * (2 * (yz + wx));

		ret.m20 = scale * (2 * (xz + wy));
		ret.m21 = scale * (2 * (yz - wx));
		ret.m22 = scale * (1 - 2 * (xx + yy));

		ret.m03 = ret.m13 = ret.m23 = 0.0f;
		ret.m30 = pos.x;
		ret.m31 = pos.y;
		ret.m32 = pos.z;
		ret.m33 = 1.0f;

		return ret;
	}

	inline Matrix4x4 Mat4x4Transpose(Mat4x4F m) {

		Matrix4x4 ret;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				ret.m[i * 4 + j] = m.m[j * 4 + i];

		return ret;
	}

	inline Matrix4x4 Mat4x4ViewFromWorld(Mat4x4F m) {
		// View = [ T(R)       0 ]
		//        [ -vpos*T(R) 1 ]
		Matrix4x4 ret;
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j)
				ret.m[i * 4 + j] = m.m[j * 4 + i];
			ret.m[i * 4 + 3] = 0;
		}

		ret.m30 = -Vec3ADot(m.Row(3), m.Row(0));
		ret.m31 = -Vec3ADot(m.Row(3), m.Row(1));
		ret.m32 = -Vec3ADot(m.Row(3), m.Row(2));
		ret.m33 = 1;
		return ret;
	}

	inline Matrix4x4 Mat4x4FromCameraLookAt(Vec3AF eye, Vec3AF lookat,
			Vec3AF vup) {
		Vector3A zaxis = Vec3ANormalize(Vec3ASub(lookat, eye));
		Vector3A xaxis = Vec3ANormalize(Vec3ACross(vup, zaxis));
		Vector3A yaxis = Vec3ACross(zaxis, xaxis);
		return Matrix4x4(xaxis.x, yaxis.x, zaxis.x, 0, xaxis.y, yaxis.y,
				zaxis.y, 0, xaxis.z, yaxis.z, zaxis.z, 0, -Vec3ADot(xaxis, eye),
				-Vec3ADot(yaxis, eye), -Vec3ADot(zaxis, eye), 1);
	}

	inline Matrix4x4 Mat4x4FromOrtho(float w, float h, float zn, float zf) {
		float dzRecip = 1.0f / (zf - zn);

		return Matrix4x4(2 / w, 0, 0, 0, 0, 2 / h, 0, 0, 0, 0, -2 * dzRecip, 0,
				0, 0, -(zn + zf) * dzRecip, 1);
	}

	inline Matrix4x4 Mat4x4FromPerspective(float fieldOfView, float aspectRatio,
			float zn, float zf) {

		fieldOfView *= 0.5f;

		float yscale = 1 / Math::Tan(fieldOfView);
		float q = zf / (zf - zn);
		return Matrix4x4((yscale / aspectRatio), 0, 0, 0, 0, yscale, 0, 0, 0, 0,
				q, 1, 0, 0, -q * zn, 0);
	}


}  // namespace nextar
