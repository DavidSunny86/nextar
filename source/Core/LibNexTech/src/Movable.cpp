
#include "NexHeaders.h"
#include "Movable.h"

namespace nextar {

	Movable::Movable(ComponentManager *creator, const String& nodeName, bool allocMBuff)
			: Component(creator, nodeName), matrixNumber(0), lastFrustumPlane(0), matrixData(0) {
		flags = DEFAULT_FLAGS;
		if (allocMBuff) {
			matrixData = NEX_NEW MatrixBuffer();
			SetIdentityTransforms();
			matrixData->bounds.SetNull();
		}
	}

	Movable::~Movable() {
		if (matrixData)
			NEX_DELETE matrixData;
	}

	void Movable::SetIdentityTransforms() {
		NEX_ASSERT(matrixData);
		matrixData->ipos = matrixData->wpos = Vec3ASet(0, 0, 0);
		matrixData->iscale = matrixData->wscale = 1.0f;
		matrixData->irot = matrixData->wrot = QuatIdentity();
		SetMatrixDirty(true);
	}

	void Movable::SetInitialTransforms() {
		NEX_ASSERT(matrixData);
		matrixData->wpos = matrixData->ipos;
		matrixData->wscale = matrixData->iscale;
		matrixData->wrot = matrixData->irot;
		SetMatrixDirty(true);
	}
	
	void Movable::UpdateBounds() {
		matrixData->bounds.UpdateBounds(GetFullTransform());
	}

	void Movable::NotifyNodeAdded(Movable*) {
	}

	void Movable::NotifyNodeRemoved(Movable*) {
	}

	void Movable::NotifyUpdated() {
		SetMatrixDirty(true);
	}

	/** @brief Get node type */
	int Movable::GetType() const {
		return TYPE;
	}

	const Matrix4x4& Movable::GetFullTransform() {
		if (IsMatrixDirty()) {
			matrixData->cached = Mat4x4FromScaleRotPos(matrixData->wscale,
					matrixData->wrot, matrixData->wpos);
			SetMatrixDirty(false);
			matrixNumber++;
		}
		return matrixData->cached;
	}

}
