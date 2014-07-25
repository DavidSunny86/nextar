#ifndef TRANSFORMDATA_H_
#define TRANSFORMDATA_H_

#include <NexMath.h>
#include <MathTypes.h>
#include <BoundingVolume.h>

namespace nextar {

class _NexBaseAPI TransformData: public AllocMathPool<TransformData,
NEX_MATRIX_POOL_NUM_PER_BLOCK> {
	enum {
		IDENTITY_TRANSFORM = 1 << 0,
		/** true when matrix needs recalculation */
		MATRIX_DIRTY = 1 << 1,
	};

	/* World rotation */
	Quaternion rotation;
	/* World postion */
	Vector3A translation;
	/* Cached matrix */
	Matrix4x4 matrix;
	/* variable placement takes into account the alignment */
	float scale;

	/* we have 3*32 to spare here, mass and volume? */
	uint32 matrixState;
	uint32 flags;
	uint32 padding;
public:
	inline TransformData() :
			scale(1.0), matrixState(0), flags(0), padding(0) {
	}

	inline void SetIdentity() {
		rotation = QuatIdentity();
		translation = Vec3AZero();
		scale = 1.0f;
		UpdateMatrix();
		flags |= IDENTITY_TRANSFORM;
	}

	inline bool IsMatrixDirty() const {
		return (flags & MATRIX_DIRTY);
	}

	inline void SetMatrixDirty(bool dirty) {
		flags = dirty ? flags | MATRIX_DIRTY : flags & ~MATRIX_DIRTY;
	}

	inline void UpdateMatrix() {
		matrix = Mat4x4FromScaleRotPos(scale, rotation, translation);
		matrixState++;
		flags &= ~MATRIX_DIRTY;
	}

	inline uint32 GetMatrixState() const {
		return matrixState;
	}

	inline const Matrix4x4& GetMatrix() const {
		return matrix;
	}

	inline Matrix4x4& GetMatrix() {
		return matrix;
	}

	inline Vec3AF GetTranslation() const {
		return translation;
	}

	inline QuatF GetRotation() const {
		return rotation;
	}

	inline float GetScaling() const {
		return scale;
	}

	inline void SetRotation(QuatF q) {
		rotation = q;
	}

	inline void SetTranslation(Vec3AF t) {
		rotation = t;
	}

	inline void SetScaling(float value) {
		scale = value;
	}
};
}

#endif //TRANSFORMDATA_H_

