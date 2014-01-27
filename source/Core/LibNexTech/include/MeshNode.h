/* 
 * File:   MeshNode.h
 * Author: obhi
 *
 * Created on January 24, 2011, 7:55 AM
 */

#ifndef NEXTAR_MESH_NODE_H
#define	NEXTAR_MESH_NODE_H

#include "NexSys.h"
#include "Node.h"
#include "Mesh.h"
#include "TextureUnitState.h"

namespace nextar {

	class Mesh;
	class Deformer;

	class _NexExport MeshNode: public Movable {
	public:
		enum {
			TRANSPARENCY_ENABLED = Movable::LAST_FLAG << 0,
			SHADOWS_ENABLED = Movable::LAST_FLAG << 1,
		};

		enum {
			TYPE = COMPONENT_MESH_NODE
		};


		struct Renderable: public VisiblePrimitive {
			NEX_LOG_HELPER(RenderMesh);
			uint8 layer;
			uint32 sortKey;
		public:

		};

		MeshNode(ComponentManager *creator, const String& name);
		virtual ~MeshNode();

		inline bool IsShadowEnabled() const {
			return IsFlagSet(SHADOWS_ENABLED);
		}

		inline Deformer* GetDeformer() const {
			return deformer;
		}

		/** @brief Get node type */
		virtual int GetType() const;

		/** @copydoc Node::FindVisibles **/
		virtual void FindVisibles(SceneTraversal& traversal);

		/** @remarks Construct the mesh renderables from a given mesh **/
		virtual void SetMesh(MeshPtr& m);
		/** @remarks Deconstruct the mesh **/
		virtual void ResetMesh();

	protected:

		typedef vector<Renderable>::type RenderMeshList;

		float maxDrawDistanceSq;
		uint32 visibilityMask;

		Deformer* deformer;
		MeshPtr mesh;

		RenderMeshList renderMeshes;
	};
}

#endif	/* NEXTAR_MESH_NODE_H */

