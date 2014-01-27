/* 
 * File:   LightNode.h
 * Author: obhi
 *
 * Created on May 15, 2011, 4:18 PM
 */

#ifndef LIGHTNODE_H
#define	LIGHTNODE_H

#include "Node.h"
#include "VisiblePrimitive.h"
#include "VisibleLight.h"

namespace nextar {

	class _NexExport LightNode: public Movable, public VisibleLight {
	public:

		enum {
			SHADOWS_ENABLED = Movable::LAST_FLAG << 0,
		};

		enum {
			TYPE = COMPONENT_LIGHT_NODE,
		};

		LightNode(ComponentManager *creator, 
			const String& name = StringUtils::Null, VisibleLight::Type = VisibleLight::DIRECTIONAL);
		virtual ~LightNode();

		inline bool IsDirectional() const {
			return lightType == DIRECTIONAL;
		}

		inline void SetShadowEnabled(bool shadowEnabled) {
			SetFlag(SHADOWS_ENABLED, shadowEnabled);
		}

		inline bool IsShadowEnabled() const {
			return IsFlagSet(SHADOWS_ENABLED);
		}

		virtual int 	GetType() const;
		void 			SetDirection(Vec3AF dir);
		Vector3A		GetDerivedDirection();
		void			SetLightMaterial(ShaderAsset* customShader);
		MaterialAssetPtr&	GetLightMaterial() const;
		virtual void 	FindVisibles(SceneTraversal& trav);

	private:

		struct LightNodeMatrix: public AllocMathCore {
			MatrixBuffer matrixBuff;
		};

		uint16 importance;
		MaterialAssetPtr lightMaterial;
	};

	typedef vector<LightNode*>::type LightList;
}

#endif	/* LIGHTNODE_H */

