/* 
 * File:   LightNode.cpp
 * Author: obhi
 * 
 * Created on May 15, 2011, 4:18 PM
 */
#include "NexHeaders.h"
#include "scene/LightNode.h"

namespace nextar {

	LightNode::LightNode(ComponentManager *creator, const String& name, VisibleLight::Type type)
			: MovableComponent(creator, name), VisibleLight(type), importance(0) {
		if (lightType == DIRECTIONAL)
			SetInheritPositionEnabled(false);
	}

	LightNode::~LightNode() {
	}

	int LightNode::GetType() const {
		return TYPE;
	}

	Vector3A LightNode::GetDerivedDirection() {
		return Mat4x4Row(GetFullTransform(), 2);
	}

	void LightNode::SetDirection(Vec3AF dir) {
		Vector3A lookAt = Vec3ANormalize(dir);
		// current look at
		Mat4x4R r = GetFullTransform();
		Vector3A oldLookAt = Vec3ASet(0, 0, 1);
		Vector3A axis;
		// find angle
		float dotProd = Vec3ADot(lookAt, oldLookAt);
		if (dotProd < -0.9998f) {
			// take up a new axis
			Vector3A yAxis = Vec3ASet(0, 1, 0);
			axis = Vec3ACross(lookAt, yAxis);
			if (Math::Abs(Vec3ADot(axis, axis)) < Math::EPSILON
			)
				axis = Vec3ACross(lookAt, Vec3ASet(0, 0, 1));
		} else if (dotProd > 0.9998f) {
			NotifyUpdated();
			return;
		} else {
			axis = Vec3ACross(lookAt, oldLookAt);
		}
		if (dotProd > 1)
			dotProd = 1;
		else if (dotProd < -1)
			dotProd = -1;
		axis = Vec3ANormalize(axis);
		float angle = -Math::ArcCos(dotProd);
		matrixData->lrot = QuatFromAxisAng(axis, angle);
		NotifyUpdated();
	}
}
