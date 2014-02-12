
#include "NexHeaders.h"
#include "Spatial.h"
#include "NexMath.h"
#include "CommonPools.h"

namespace nextar {

	Spatial::Spatial(ComponentManager *creator, const String& name, Component* parent) :
		Component(creator, name, parent), cullingData(0), moveable(nullptr), culler(nullptr) {
		worldMatrix = NEX_NEW Matrix4x4;
		*worldMatrix = Matrix4x4::IdentityMatrix;
	}

	Spatial::~Spatial() {
		if (!moveable) {
			NEX_DELETE (worldMatrix);
			worldMatrix = nullptr;
		}
	}

	void Spatial::SetMoveable(Moveable* moveable) {
		if (this->moveable) {
			worldMatrix = nullptr;
		}

		if (moveable) {
			if (worldMatrix)
				NEX_DELETE  (worldMatrix);
			worldMatrix = &moveable->GetWorldMatrix();
		} else {
			worldMatrix = NEX_NEW Matrix4x4;
			*worldMatrix = Matrix4x4::IdentityMatrix;
		}

		this->moveable = moveable;
	}

	void Spatial::UpdateBounds() {
		bounds.UpdateBounds(*worldMatrix);
		SetBoundsDirty(false);
	}
}
