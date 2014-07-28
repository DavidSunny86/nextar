#include <NexEngine.h>
#include <Spatial.h>
#include <NexMath.h>
#include <CommonPools.h>
#include <Moveable.h>

namespace nextar {

Spatial::Spatial(const StringID name, const StringID factory,  Component* parent) :
		Component(name, factory, parent), cullingData(0), moveable(nullptr), culler(
				nullptr), matrixState(0) {
	worldMatrix = NEX_NEW(Matrix4x4);
	*worldMatrix = Matrix4x4::IdentityMatrix;
}

Spatial::~Spatial() {
	if (!moveable) {
		NEX_DELETE((worldMatrix));
		worldMatrix = nullptr;
	}
}

void Spatial::SetMoveable(Moveable* moveable) {
	if (this->moveable) {
		worldMatrix = nullptr;
	}

	if (moveable) {
		if (worldMatrix)
			NEX_DELETE((worldMatrix));
		worldMatrix = &moveable->GetWorldMatrix();
	} else {
		if (!worldMatrix)
			worldMatrix = NEX_NEW(Matrix4x4);
		*worldMatrix = Matrix4x4::IdentityMatrix;
	}

	matrixState = 0;
	this->moveable = moveable;
}

void Spatial::Update() {
	uint32 oldMatrixState =  matrixState;
	if (!moveable || matrixState != (oldMatrixState=moveable->GetMatrixState()) ) {
		matrixState = oldMatrixState;
		bounds.UpdateBounds(*worldMatrix);
	}

	SetBoundsDirty(false);
	SetUpdateRequired(false);
}

}
