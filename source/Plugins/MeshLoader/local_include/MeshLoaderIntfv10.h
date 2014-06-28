/*
 * MeshLoaderIntfv10.h
 *
 *  Created on: 16-Nov-2013
 *      Author: obhi
 */

#ifndef MESHLOADERINTFV10_H_
#define MESHLOADERINTFV10_H_

#include <MeshLoaderImpl.h>

using namespace nextar;
namespace MeshLoader {

class MeshLoaderImplv1_0: public MeshLoaderIntf, public AllocGeneral {

	NEX_LOG_HELPER(MeshLoaderImplv1_0)
	;

public:
	MeshLoaderImplv1_0();
	virtual ~MeshLoaderImplv1_0();

	virtual void Load(InputSerializer&, AssetLoader& loader);

	MeshVertexData* ReadVertexData(MeshAsset::StreamRequest*,
			InputSerializer& ser, VertexElement* vertexElement,
			uint16 vertexElementCount, VertexLayoutType type);
	void ReadMeshHeader(MeshAsset::StreamRequest*, InputSerializer& ser);
	BoundsInfo ReadBoundsInfo(MeshAsset::StreamRequest*, InputSerializer& ser);
	MeshIndexData* ReadIndexData(MeshAsset::StreamRequest*,
			InputSerializer& ser);
	MaterialAssetPtr ReadMaterialData(MeshAsset::StreamRequest*,
			InputSerializer& ser);

	void ReadSubMesh(MeshAsset::StreamRequest*, InputSerializer& ser);

	void ReadMeshChunk(MeshAsset::StreamRequest*, InputSerializer& ser);
	void ReadVertexElementData(MeshAsset::StreamRequest*, InputSerializer& ser,
			uint16& layoutType, uint8& vertexElementCount, VertexElement*& vertexElement);
	void ReadVertexBufferData(MeshAsset::StreamRequest*, InputSerializer& ser,
			MeshVertexData*& vertexData);

	void FindStreamVertexElements(MeshAsset::StreamRequest*,
			const VertexElement*& start, const VertexElement*& end,
			uint16 streamIndex, const VertexElement* elements,
			uint16 numVertexElements);
};

}

#endif /* MESHLOADERINTFV10_H_ */
