/*
 * MeshSaverImplv10.h
 *
 *  Created on: 03-Oct-2014
 *      Author: obhi
 */

#ifndef MESHSAVERIMPLV10_H_
#define MESHSAVERIMPLV10_H_

#include <Asset.h>
#include <MeshTemplate.h>

namespace nextar {

class MeshSaverImplv1_0: public AssetSaverImpl {
public:
	static MeshSaverImplv1_0 saver;
	MeshSaverImplv1_0();
	virtual ~MeshSaverImplv1_0();

	virtual void Save(OutputStreamPtr&, AssetSaver&);

	void WriteVersion(OutputStreamPtr&);
	void WriteMeshHeader(MeshTemplate* templ, ChunkOutputStream& outStream);
	void WriteBoundsInfo(const BoundsInfo& bounds, ChunkOutputStream& outStream);
	void WriteVertexData(MeshBuffer* buffer, ChunkOutputStream& outStream);
	void WriteVertexBufferData(MeshBuffer* buffer, uint32 numVB, ChunkOutputStream& outStream);
	void WriteIndexData(MeshBuffer* buffer, ChunkOutputStream& outStream);
	void WriteElementData(MeshBuffer* buffer, ChunkOutputStream& outStream);
	void WriteMaterialData(const MaterialTemplatePtr& material, ChunkOutputStream& outStream);
	void WriteSubMeshData(MeshTemplate* templ, const MeshTemplate::PrimitiveGroup& gr, ChunkOutputStream& outStream);
};

} /* namespace nextar */

#endif /* MESHSAVERIMPLV10_H_ */
