#pragma once

namespace nextar {

	inline void Mat4x4TransVec3(Vector4* outstream, uint32 outstride,
			const Vector3* inpstream, uint32 inpstride, uint32 count, Mat4x4F m) {
		NEX_ASSERT(outstream);
		NEX_ASSERT(inpstream);
		const uint8* inpVec = (const uint8*) inpstream;
		uint8* outVec = (uint8*) outstream;

		for (uint32 i = 0; i < count; i++) {
			Quad x = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->x);
			Quad y = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->y);
			Quad res = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->z);
			res = _mm_mul_ps(res, m.r[2]);
			res = _mm_add_ps(res, m.r[3]);
			y = _mm_mul_ps(y, m.r[1]);
			res = _mm_add_ps(res, y);
			x = _mm_mul_ps(x, m.r[0]);
			res = _mm_add_ps(res, x);
			_mm_storeu_ps(reinterpret_cast<float *>(outVec), res);
			inpVec += inpstride;
			outVec += outstride;
		}
	}

	inline void Mat4x4TransAndProjVec3(Vector3* outstream, uint32 outstride,
			const Vector3* inpstream, uint32 inpstride, uint32 count, Mat4x4F m) {
		NEX_ASSERT(outstream);
		NEX_ASSERT(inpstream);

		const uint8 *inpVec = (uint8*) inpstream;
		uint8 *outVec = (uint8*) outstream;

		for (uint32 i = 0; i < count; i++) {
			Quad x = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->x);
			Quad y = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->y);
			Quad res = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->z);
			res = _mm_mul_ps(res, m.r[2]);
			res = _mm_add_ps(res, m.r[3]);
			y = _mm_mul_ps(y, m.r[1]);
			res = _mm_add_ps(res, y);
			x = _mm_mul_ps(x, m.r[0]);
			res = _mm_add_ps(res, x);

			x = _mm_shuffle_ps(res, res, _MM_SHUFFLE(3, 3, 3, 3));
			res = _mm_div_ps(res, x);
			_mm_store_ss(&reinterpret_cast<Vector3*>(outVec)->x, res);
			res = _mm_shuffle_ps(res, res, _MM_SHUFFLE(0, 3, 2, 1));
			_mm_store_ss(&reinterpret_cast<Vector3*>(outVec)->y, res);
			res = _mm_shuffle_ps(res, res, _MM_SHUFFLE(0, 3, 2, 1));
			_mm_store_ss(&reinterpret_cast<Vector3*>(outVec)->z, res);
			inpVec += inpstride;
			outVec += outstride;
		}

	}

	inline void Mat4x4TransVec3(Vector4A* outstream, uint32 outstride,
			const Vector3* inpstream, uint32 inpstride, uint32 count, Mat4x4F m) {
		NEX_ASSERT(outstream);
		NEX_ASSERT(inpstream);
		const uint8* inpVec = (const uint8*) inpstream;
		uint8* outVec = (uint8*) outstream;

		for (uint32 i = 0; i < count; i++) {
			Quad x = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->x);
			Quad y = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->y);
			Quad res = _mm_load_ps1(&reinterpret_cast<const Vector3*>(inpVec)->z);
			res = _mm_mul_ps(res, m.r[2]);
			res = _mm_add_ps(res, m.r[3]);
			y = _mm_mul_ps(y, m.r[1]);
			res = _mm_add_ps(res, y);
			x = _mm_mul_ps(x, m.r[0]);
			res = _mm_add_ps(res, x);
			_mm_store_ps(reinterpret_cast<float *>(outVec), res);
			inpVec += inpstride;
			outVec += outstride;
		}
	}

	inline Vector3A Mat4x4TransAndProjVec3A(Vec3AF v, Mat4x4F m) {
		Quad ret;
		ret = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
		ret = _mm_mul_ps(ret, m.r[0]);
		Quad vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
		vTemp = _mm_mul_ps(vTemp, m.r[1]);
		ret = _mm_add_ps(ret, vTemp);
		vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
		vTemp = _mm_mul_ps(vTemp, m.r[2]);
		ret = _mm_add_ps(ret, vTemp);
		ret = _mm_add_ps(ret, m.r[3]);
		vTemp = _mm_shuffle_ps(ret, ret, _MM_SHUFFLE(3, 3, 3, 3));
		ret = _mm_div_ps(ret, vTemp);
		return ret;
	}

	inline Vector4A Mat4x4TransVec3A(Vec3AF v, Mat4x4F m) {
		Quad ret;
		ret = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
		ret = _mm_mul_ps(ret, m.r[0]);
		Quad vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
		vTemp = _mm_mul_ps(vTemp, m.r[1]);
		ret = _mm_add_ps(ret, vTemp);
		vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
		vTemp = _mm_mul_ps(vTemp, m.r[2]);
		ret = _mm_add_ps(ret, vTemp);
		ret = _mm_add_ps(ret, m.r[3]);
		return ret;
	}

	inline Vector4A Mat4x4TransBoundRadius(Vec3AF v, Mat4x4F m) {
		Quad ret = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
		Quad clearSign = N3D_ClearSign.v;
		ret = _mm_mul_ps(ret, m.r[0]);
		ret = _mm_and_ps(ret, clearSign);
		Quad vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
		vTemp = _mm_mul_ps(vTemp, m.r[1]);
		vTemp = _mm_and_ps(vTemp, clearSign);
		ret = _mm_add_ps(ret, vTemp);
		vTemp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
		vTemp = _mm_mul_ps(vTemp, m.r[2]);
		vTemp = _mm_and_ps(vTemp, clearSign);
		ret = _mm_add_ps(ret, vTemp);
		return ret;
	}

	inline Matrix4x4 Mat4x4FromVectorMapping(const Vector3& axis1,
			const Vector3& axis2) {
		Matrix3x4 m = Mat3x4FromVectorMapping(axis1, axis2);
		return Matrix4x4(m, Matrix4x4::IdentityMatrix.r[3]);
	}

	inline Matrix4x4 Mat4x4FromVectorMapping(const Vector3& axis1,
			const Vector3& axis2, const Vector3& t) {
		Matrix3x4 m = Mat3x4FromVectorMapping(axis1, axis2);
		return Matrix4x4(m, Vec4ASet(t.x, t.y, t.z, 1));
	}

	inline Matrix4x4 Mat4x4Mul(Mat4x4F m1, Mat4x4F m2) {
		Matrix4x4 result;
		// Use vw to hold the original row
		Quad vw = m1.r[0];
		// Splat the component x,y,Z then W
		Quad vx = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(0, 0, 0, 0));
		Quad vy = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(1, 1, 1, 1));
		Quad vz = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(2, 2, 2, 2));
		vw = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(3, 3, 3, 3));
		// Perform the opertion on the first row
		vx = _mm_mul_ps(vx, m2.r[0]);
		vy = _mm_mul_ps(vy, m2.r[1]);
		vz = _mm_mul_ps(vz, m2.r[2]);
		vw = _mm_mul_ps(vw, m2.r[3]);
		// Perform a binary Add to reduce cumulative errors
		vx = _mm_add_ps(vx, vz);
		vy = _mm_add_ps(vy, vw);
		vx = _mm_add_ps(vx, vy);
		result.r[0] = vx;
		// Repeat for the other 3 rows
		vw = m1.r[1];
		vx = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(0, 0, 0, 0));
		vy = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(1, 1, 1, 1));
		vz = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(2, 2, 2, 2));
		vw = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(3, 3, 3, 3));
		vx = _mm_mul_ps(vx, m2.r[0]);
		vy = _mm_mul_ps(vy, m2.r[1]);
		vz = _mm_mul_ps(vz, m2.r[2]);
		vw = _mm_mul_ps(vw, m2.r[3]);
		vx = _mm_add_ps(vx, vz);
		vy = _mm_add_ps(vy, vw);
		vx = _mm_add_ps(vx, vy);
		result.r[1] = vx;
		vw = m1.r[2];
		vx = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(0, 0, 0, 0));
		vy = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(1, 1, 1, 1));
		vz = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(2, 2, 2, 2));
		vw = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(3, 3, 3, 3));
		vx = _mm_mul_ps(vx, m2.r[0]);
		vy = _mm_mul_ps(vy, m2.r[1]);
		vz = _mm_mul_ps(vz, m2.r[2]);
		vw = _mm_mul_ps(vw, m2.r[3]);
		vx = _mm_add_ps(vx, vz);
		vy = _mm_add_ps(vy, vw);
		vx = _mm_add_ps(vx, vy);
		result.r[2] = vx;
		vw = m1.r[3];
		vx = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(0, 0, 0, 0));
		vy = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(1, 1, 1, 1));
		vz = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(2, 2, 2, 2));
		vw = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(3, 3, 3, 3));
		vx = _mm_mul_ps(vx, m2.r[0]);
		vy = _mm_mul_ps(vy, m2.r[1]);
		vz = _mm_mul_ps(vz, m2.r[2]);
		vw = _mm_mul_ps(vw, m2.r[3]);
		vx = _mm_add_ps(vx, vz);
		vy = _mm_add_ps(vy, vw);
		vx = _mm_add_ps(vx, vy);
		result.r[3] = vx;
		return result;
	}

	inline Matrix4x4 Mat4x4FromScaleRotPos(float scale, QuatF rot,
			Vec3AF pos) {
		Matrix4x4 ret;
		Quad q0, q1;
		Quad v0, v1, v2;

		q0 = _mm_add_ps(rot, rot);
		q1 = _mm_mul_ps(rot, q0);

		v0 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3, 0, 0, 1));
		v0 = _mm_and_ps(v0, N3D_FFFO.v);
		v1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3, 1, 2, 2));
		v1 = _mm_and_ps(v1, N3D_FFFO.v);
		ret.r[0] = _mm_sub_ps(N3D_1110.v, v0);
		ret.r[0] = _mm_sub_ps(ret.r[0], v1);

		v0 = _mm_shuffle_ps(rot, rot, _MM_SHUFFLE(3, 1, 0, 0));
		v1 = _mm_shuffle_ps(q0, q0, _MM_SHUFFLE(3, 2, 1, 2));
		v0 = _mm_mul_ps(v0, v1);

		v1 = _mm_shuffle_ps(rot, rot, _MM_SHUFFLE(3, 3, 3, 3));
		v2 = _mm_shuffle_ps(q0, q0, _MM_SHUFFLE(3, 0, 2, 1));
		v1 = _mm_mul_ps(v1, v2);

		ret.r[1] = _mm_add_ps(v0, v1);
		ret.r[2] = _mm_sub_ps(v0, v1);

		v0 = _mm_shuffle_ps(ret.r[1], ret.r[2], _MM_SHUFFLE(1, 0, 2, 1));
		v0 = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(1, 3, 2, 0));
		v1 = _mm_shuffle_ps(ret.r[1], ret.r[2], _MM_SHUFFLE(2, 2, 0, 0));
		v1 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(2, 0, 2, 0));

		q1 = _mm_shuffle_ps(ret.r[0], v0, _MM_SHUFFLE(1, 0, 3, 0));
		q1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 3, 2, 0));

		Quad scaleQ = QuadReplicate(scale);
		ret.r[0] = _mm_mul_ps(scaleQ, q1);
		q1 = _mm_shuffle_ps(ret.r[0], v0, _MM_SHUFFLE(3, 2, 3, 1));
		q1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 3, 0, 2));
		ret.r[1] = _mm_mul_ps(scaleQ, q1);
		q1 = _mm_shuffle_ps(v1, ret.r[0], _MM_SHUFFLE(3, 2, 1, 0));
		ret.r[2] = _mm_mul_ps(scaleQ, q1);
		ret.r[3] = _mm_or_ps(_mm_and_ps(pos, N3D_FFFO.v), N3D_0001.v);
		return ret;
	}

	inline Matrix4x4 Mat4x4Transpose(Mat4x4F m) {
		// x.x,x.y,y.x,y.y
		Quad temp1 = _mm_shuffle_ps(m.r[0], m.r[1], _MM_SHUFFLE(1, 0, 1, 0));
		// x.z,x.w,y.z,y.w
		Quad temp3 = _mm_shuffle_ps(m.r[0], m.r[1], _MM_SHUFFLE(3, 2, 3, 2));
		// z.x,z.y,w.x,w.y
		Quad temp2 = _mm_shuffle_ps(m.r[2], m.r[3], _MM_SHUFFLE(1, 0, 1, 0));
		// z.z,z.w,w.z,w.w
		Quad temp4 = _mm_shuffle_ps(m.r[2], m.r[3], _MM_SHUFFLE(3, 2, 3, 2));
		Matrix4x4 result;

		// x.x,y.x,z.x,w.x
		result.r[0] = _mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(2, 0, 2, 0));
		// x.y,y.y,z.y,w.y
		result.r[1] = _mm_shuffle_ps(temp1, temp2, _MM_SHUFFLE(3, 1, 3, 1));
		// x.z,y.z,z.z,w.z
		result.r[2] = _mm_shuffle_ps(temp3, temp4, _MM_SHUFFLE(2, 0, 2, 0));
		// x.w,y.w,z.w,w.w
		result.r[3] = _mm_shuffle_ps(temp3, temp4, _MM_SHUFFLE(3, 1, 3, 1));

		return result;
	}

	inline Matrix4x4 Mat4x4ViewFromWorld(Mat4x4F m) {
		// View = [ T(R)       0 ]
		//        [ -vpos*T(R) 1 ]
		Matrix4x4 ret;
		ret.r[0] = _mm_shuffle_ps(m.r[1], m.r[2], _MM_SHUFFLE(3, 0, 0, 3));
		ret.r[0] = _mm_move_ss(ret.r[0], m.r[0]);
		ret.r[1] = _mm_shuffle_ps(m.r[0], m.r[1], _MM_SHUFFLE(3, 1, 3, 1));
		ret.r[1] = _mm_shuffle_ps(ret.r[1], m.r[2], _MM_SHUFFLE(3, 1, 2, 0));
		ret.r[2] = _mm_shuffle_ps(m.r[0], m.r[1], _MM_SHUFFLE(3, 2, 3, 2));
		ret.r[2] = _mm_shuffle_ps(ret.r[2], m.r[2], _MM_SHUFFLE(3, 2, 2, 0));

		ret.r[3] = _mm_shuffle_ps(m.r[3], m.r[3], _MM_SHUFFLE(0, 0, 0, 0));
		ret.r[3] = _mm_mul_ps(ret.r[3], ret.r[0]);
		Quad vTemp = _mm_shuffle_ps(m.r[3], m.r[3], _MM_SHUFFLE(1, 1, 1, 1));
		vTemp = _mm_mul_ps(vTemp, ret.r[1]);
		ret.r[3] = _mm_add_ps(ret.r[3], vTemp);
		vTemp = _mm_shuffle_ps(m.r[3], m.r[3], _MM_SHUFFLE(2, 2, 2, 2));
		vTemp = _mm_mul_ps(vTemp, ret.r[2]);
		ret.r[3] = _mm_add_ps(ret.r[3], vTemp);
		ret.r[3] = _mm_xor_ps(ret.r[3], N3D_FlipSign.v);
		// and with 0001
		ret.r[3] = _mm_and_ps(ret.r[3], N3D_0001.v);

		NEX_ASSERT(
				ret.m03 == 0.f && ret.m13 == 0.f && ret.m23 == 0.f
						&& ret.m33 == 1.f);
		return ret;
	}

	inline Matrix4x4 Mat4x4FromCameraLookAt(Vec3AF eye, Vec3AF lookat,
			Vec3AF vup) {
		Matrix4x4 ret;
		Vector3A up;
		Vector3A dir = Vec3ASub(lookat, eye);
		ret.r[2] = Vec3ANormalize(dir);
		up = Vec3ACross(vup, dir);
		ret.r[1] = Vec3ANormalize(up);
		ret.r[0] = Vec3ACross(ret.r[2], ret.r[1]);
		ret.r[3] = eye;
		return ret;
	}

	inline Matrix4x4 Mat4x4FromOrtho(float w, float h, float zn, float zf) {

		float dzRecip = 1.0f / (zf - zn);
		return Matrix4x4(2 / w, 0, 0, 0,
						 0, 2 / h, 0, 0,
						 0, 0, -2 * dzRecip, 0,
						 0, 0, - (zn + zf) * dzRecip, 1);
	}

	inline Matrix4x4 Mat4x4FromPerspective(float fieldOfView, float aspectRatio,
			float zn, float zf) {

		fieldOfView *= 0.5f;

		float yscale = 1 / Math::Tan(fieldOfView);
		float q = zf / (zf - zn);
		return Matrix4x4((yscale / aspectRatio), 0, 0, 0, 0, yscale, 0, 0, 0, 0, q,
				1, 0, 0, -q * zn, 0);
	}

}
