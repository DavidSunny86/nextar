#include "NexHeaders.h"
#include "scene/ContainerNode.h"

namespace nextar {

	ContainerNode::ContainerNode(ComponentManager *creator,
			const String& name, bool allocMatrixBuff)
			: Movable(creator, name, allocMatrixBuff) {
		flags |= IS_CONTAINER;
	}

	ContainerNode::~ContainerNode() {
		// delete nodes
		NodeList::iterator it = children.begin();
		while (it != children.end()) {
			NEX_DELETE (*it);
			++it;
		}
	}

	void ContainerNode::AddChild(Movable* node) {
		Movable* par = node->GetParent();
		if (par != this) {
			if (par)
				par->RemoveChild(node);
			children.push_back(node);
			node->SetParent(this);
			if (IsChildNotificationActive()) {
				NotifyNodeAdded(node);
			} else if (regionNode && regionNode->IsChildNotificationActive()) {
				regionNode->NotifyNodeAdded(node);
			}
		}
	}

	void ContainerNode::RemoveChild(Movable* node) {
		Movable* par = node->GetParent();
		if (par == this) {
			children.remove(node);
			node->SetParent(0);

			if (IsChildNotificationActive()) {
				NotifyNodeRemoved(node);
			} else if (regionNode && regionNode->IsChildNotificationActive()) {
				regionNode->NotifyNodeRemoved(node);
			}
		}
	}

	void ContainerNode::UpdateBounds() {
		NodeList::iterator it = children.begin();
		BoundingVolume& vol = matrixData->bounds;
		vol.SetNull();
		while (it != children.end()) {
			vol.MergeBounds(*(*it)->GetBounds());
			++it;
		}
	}

	void ContainerNode::NotifyChildNeedsUpdate(Movable* child) {
		if (!IsUpdateChildrenEnabled())
			childUpdates.push_back(child);
	}

	void ContainerNode::NotifyUpdated(bool forceNotifyParent) {
		Movable::NotifyUpdated(forceNotifyParent);
		childUpdates.clear();
	}

	void ContainerNode::Update(bool parentChanged) {
		if (parentChanged || IsDeriveTransformsEnabled()) {
			DeriveFromParent();
			parentChanged = true;
		}

		bool boundsDirty = false;
		if (IsUpdateChildrenEnabled()) {
			NodeList::iterator it = children.begin();
			while (it != children.end()) {
				(*it)->Update(true);
				++it;
			}
			boundsDirty = true;
			childUpdates.clear();
		} else if (childUpdates.size()) {
			NodeList::iterator it = childUpdates.begin();
			while (it != childUpdates.end()) {
				(*it)->Update(true);
				++it;
			}
			childUpdates.clear();
			boundsDirty = true;
		}

		if (IsUpdateBoundsEnabled() && boundsDirty)
			UpdateBounds();
	}

	void ContainerNode::FindVisibles(SceneTraversal& traversal) {
		uint32 newMask;
		if (traversal.camera->IsVisible(*GetBounds(), newMask,
				lastFrustumPlane)) {
			uint32 oldMask = traversal.camera->GetCullMask();
			traversal.camera->SetCullMask(newMask);
			NodeList::iterator it = children.begin();
			while (it != children.end()) {
				(*it)->FindVisibles(traversal);
				++it;
			}
			traversal.camera->SetCullMask(oldMask);
		}
	}
}

