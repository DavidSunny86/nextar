/* 
 * File:   SubMesh.h
 * Author: obhi
 *
 * Created on May 21, 2011, 11:04 PM
 */

#ifndef NEXSYS_SUBMESH_H
#define	NEXSYS_SUBMESH_H

#include <NexBase.h>

namespace nextar {

	static const uint32 MAX_STREAM_COUNT = 8;

	/**
	 * @class	SubMesh
	 *
	 * @remarks SubMesh class for meshes separated by material data
	 **/
	class _NexEngineAPI SubMesh: public AllocGeneral {
		friend class MeshLoader;
		friend class MeshLoaderImplv1_0;

	public:

		SubMesh(const String&, MeshAsset* origMesh);
		~SubMesh();

		PrimitiveType GetPrimitiveType() const {
			return primitiveType;
		}

		inline const MaterialAsset* GetMaterial() const {
			return material;
		}

		inline MaterialAsset* GetMaterial() {
			return material;
		}

		inline uint32 GetStartVertex() const {
			return startVertex;
		}

		inline uint32 GetVertexCount() const {
			return vertexCount;
		}

		inline uint32 GetStartIndex() const {
			return startIndex;
		}

		inline uint32 GetIndexCount() const {
			return indexCount;
		}

		String GetName() const {
			return name;
		}

		MeshIndexData* GetIndexData() const {
			return indexData;
		}

		MeshVertexData* GetVertexData() const {
			return vertexData;
		}

		void Prepare();

	private:

		bool vertexDeformationsEnabled;

		BoundsInfo boundsInfo;
		/**/
		MeshAsset* mesh;
		/* Type, may vary depending upon pass */
		PrimitiveType primitiveType;
		/* Material, may be shared */
		MaterialAsset* material;
		/* Vertex data, buffers may be shared */
		MeshVertexData* vertexData;
		/* Index data, buffers may be shared */
		MeshIndexData* indexData;
		/* start vertex */
		uint32 startVertex;
		/* vertex count */
		uint32 vertexCount;
		/* index start */
		uint32 startIndex;
		/* index count */
		uint32 indexCount;
		/* sub name */
		String name;
	};
}

#endif	/* NEXSYS_SUBMESH_H */

