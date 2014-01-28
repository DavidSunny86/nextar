
#include "NexHeaders.h"
#include "Moveable.h"

namespace nextar {

	Moveable::Moveable(ComponentManager *creator, const String& nodeName, bool allocMBuff)
			: Component(creator, nodeName), matrixNumber(0), lastFrustumPlane(0), matrixData(0) {
		flags = DEFAULT_FLAGS;
		if (allocMBuff) {
			matrixData = NEX_NEW MatrixBuffer();
			SetIdentityTransforms();
			matrixData->bounds.SetNull();
		}
	}

	Moveable::~Moveable() {
		if (matrixData)
			NEX_DELETE matrixData;
	}

	void Moveable::SetIdentityTransforms() {
		NEX_ASSERT(matrixData);
		matrixData->ipos = matrixData->wpos = Vec3ASet(0, 0, 0);
		matrixData->iscale = matrixData->wscale = 1.0f;
		matrixData->irot = matrixData->wrot = QuatIdentity();
		SetMatrixDirty(true);
	}

	void Moveable::SetInitialTransforms() {
		NEX_ASSERT(matrixData);
		matrixData->wpos = matrixData->ipos;
		matrixData->wscale = matrixData->iscale;
		matrixData->wrot = matrixData->irot;
		SetMatrixDirty(true);
	}
	
	void Moveable::UpdateBounds() {
		matrixData->bounds.UpdateBounds(GetFullTransform());
	}

	void Moveable::NotifyNodeAdded(Moveable*) {
	}

	void Moveable::NotifyNodeRemoved(Moveable*) {
	}

	void Moveable::NotifyUpdated() {
		SetMatrixDirty(true);
	}

	/** @brief Get node type */
	int Moveable::GetComponentType() const {
		return TYPE;
	}

	const Matrix4x4& Moveable::GetFullTransform() {
		if (IsMatrixDirty()) {
			matrixData->cached = Mat4x4FromScaleRotPos(matrixData->wscale,
					matrixData->wrot, matrixData->wpos);
			SetMatrixDirty(false);
			matrixNumber++;
		}
		return matrixData->cached;
	}
}
