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
	Quat::type rotation;
	/* World postion */
	Vec3A::type translation;
	/* Cached matrix */
	Mat4::type matrix;
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
		rotation = Quat::Identity();
		translation = Vec3A::Zero();
		scale = 1.0f;
		matrix = Mat4::type::IdentityMatrix;
		matrixState++;
		flags |= IDENTITY_TRANSFORM;
	}

	inline bool IsMatrixDirty() const {
		return (flags & MATRIX_DIRTY);
	}

	inline void SetMatrixDirty(bool dirty) {
		flags = dirty ? flags | MATRIX_DIRTY : flags & ~MATRIX_DIRTY;
	}

	inline void UpdateMatrix() {
		matrix = Mat4::FromScaleRotPos(scale, rotation, translation);
		matrixState++;
		flags &= ~MATRIX_DIRTY;
	}

	inline uint32 GetMatrixState() const {
		return matrixState;
	}

	inline const Mat4::type& GetMatrix() const {
		return matrix;
	}

	inline Mat4::type& GetMatrix() {
		return matrix;
	}

	inline Vec3A::pref GetTranslation() const {
		return translation;
	}

	inline Quat::pref GetRotation() const {
		return rotation;
	}

	inline float GetScaling() const {
		return scale;
	}

	inline void SetRotation(Quat::pref q) {
		rotation = q;
	}

	inline void SetTranslation(Vec3A::pref t) {
		translation = t;
	}

	inline void SetScaling(float value) {
		scale = value;
	}
};

}

#endif //TRANSFORMDATA_H_

