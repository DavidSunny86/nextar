/*
 * MeshLoaderIntfv10.h
 *
 *  Created on: 16-Nov-2013
 *      Author: obhi
 */

#ifndef MESHLOADERINTFV10_H_
#define MESHLOADERINTFV10_H_

#include "MeshLoaderImpl.h"

using namespace nextar;
namespace MeshLoader {

	class MeshLoaderImplv1_0: public MeshLoaderIntf, public AllocGeneral {

		NEX_LOG_HELPER(MeshLoaderImplv1_0);

	public:
		MeshLoaderImplv1_0();
		virtual ~MeshLoaderImplv1_0();

		virtual void Load(InputSerializer&, Mesh::Loader& loader);

		MeshVertexData* ReadVertexData(Mesh::StreamRequest*, InputSerializer& ser,
						VertexElement* vertexElement, uint16 vertexElementCount);
		void ReadMeshHeader(Mesh::StreamRequest*, InputSerializer& ser);
		BoundsInfo ReadBoundsInfo(Mesh::StreamRequest*, InputSerializer& ser);
		MeshIndexData* ReadIndexData(Mesh::StreamRequest*, InputSerializer& ser);
		Material* ReadMaterialData(Mesh::StreamRequest*, InputSerializer& ser);

		void ReadSubMesh(Mesh::StreamRequest*, InputSerializer& ser);

		void ReadMeshChunk(Mesh::StreamRequest*, InputSerializer& ser);
		void ReadVertexElementData(Mesh::StreamRequest*, InputSerializer& ser,
				VertexElement*& vertexElement, uint16& vertexDataCount);
		void ReadVertexBufferData(Mesh::StreamRequest*, InputSerializer& ser,
				MeshVertexData*& vertexData);

		void FindStreamVertexElements(Mesh::StreamRequest*, const VertexElement*& start,
				const VertexElement*& end, uint16 streamIndex,
				const VertexElement* elements, uint16 numVertexElements);
	};

}

#endif /* MESHLOADERINTFV10_H_ */
