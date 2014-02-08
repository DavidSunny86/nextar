
#include "NexHeaders.h"
#include "Spatial.h"
#include "NexMath.h"
#include "CommonPools.h"

namespace nextar {

	Spatial::Spatial(ComponentManager *creator, const String& name, Component* parent) :
		Component(creator, name, parent), cullingData(0), moveable(nullptr), culler(nullptr) {
		worldMatrix = Matrix4x4Pool::Alloc();
		*worldMatrix = Matrix4x4::IdentityMatrix;
	}

	Spatial::~Spatial() {
		if (!moveable) {
			Matrix4x4Pool::Free(worldMatrix);
			worldMatrix = nullptr;
		}
	}

	void Spatial::SetMoveable(Moveable* moveable) {
		if (this->moveable) {
			worldMatrix = nullptr;
		}

		if (moveable) {
			if (worldMatrix)
				Matrix4x4Pool::Free(worldMatrix);
			worldMatrix = moveable->GetTransformPtr();
		} else {
			worldMatrix = Matrix4x4Pool::Alloc();
			*worldMatrix = Matrix4x4::IdentityMatrix;
		}

		this->moveable = moveable;
	}

	void Spatial::UpdateBounds() {
		bounds.UpdateBounds(*worldMatrix);
		SetBoundsDirty(false);
	}
}
