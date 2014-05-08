
#include <NexEngine.h>
#include <Moveable.h>
#include <CommonPools.h>

namespace nextar {

	Moveable::Moveable(const StringID nodeName, Component* parent)
			: Component(nodeName, parent), transform(NEX_NEW( TransformData )) {
		flags = DEFAULT_FLAGS;
		SetIdentityTransforms();
	}

	Moveable::~Moveable() {
		NEX_DELETE((transform));
		transform = nullptr;
	}

	void Moveable::SetIdentityTransforms() {
		NEX_ASSERT(transform);
		transform->SetIdentity();
	}

	void Moveable::NotifyNodeAdded(Moveable*) {
	}

	void Moveable::NotifyNodeRemoved(Moveable*) {
	}

	void Moveable::NotifyUpdated() {
		transform->SetMatrixDirty(true);
	}

	/** @brief Get node type */
	uint32 Moveable::GetClassID() const {
		return CLASS_ID;
	}

}
