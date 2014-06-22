#include <NexEngine.h>
#include <MeshAsset.h>
#include <BufferManager.h>
#include <VertexElement.h>

namespace nextar {

MeshVertexData::CommonVertexElement
MeshVertexData::commonElementLayout[VertexLayoutType::VERTEX_LAYOUT_COUNT] = {
	{
		2,
		{
			VertexElement(VertexDesc( VertexSemantic(COMP_POSITION, 0, COMP_TYPE_FLOAT3), 0 ), 0, 0),
			VertexElement(VertexDesc( VertexSemantic(COMP_NORMAL, 0, COMP_TYPE_FLOAT3), 12 ), 0, 0)
		}
	},
	{
		3,
		{
			VertexElement(VertexDesc( VertexSemantic(COMP_POSITION, 0, COMP_TYPE_FLOAT3), 0 ), 0, 0),
			VertexElement(VertexDesc( VertexSemantic(COMP_NORMAL, 0, COMP_TYPE_FLOAT3), 12 ), 0, 0),
			VertexElement(VertexDesc( VertexSemantic(COMP_TEXTURE_COORDINATE, 0, COMP_TYPE_FLOAT2), 12 ), 0, 0)
		}
	},
	{
		2,
		{
			VertexElement(VertexDesc( VertexSemantic(COMP_POSITION, 0, COMP_TYPE_FLOAT3), 0 ), 0, 0),
			VertexElement(VertexDesc( VertexSemantic(COMP_NORMAL, 0, COMP_TYPE_FLOAT3), 12 ), 1, 0)
		}
	},
	{
		3,
		{
			VertexElement(VertexDesc( VertexSemantic(COMP_POSITION, 0, COMP_TYPE_FLOAT3), 0 ), 0, 0),
			VertexElement(VertexDesc( VertexSemantic(COMP_NORMAL, 0, COMP_TYPE_FLOAT3), 12 ), 1, 0),
			VertexElement(VertexDesc( VertexSemantic(COMP_TEXTURE_COORDINATE, 0, COMP_TYPE_FLOAT2), 12 ), 1, 0)
		}
	}
};

MeshVertexData::~MeshVertexData() {
	if (vertexElements)
		NEX_DELETE_ARRAY(vertexElements);
	layout.Clear();
	if (layoutType != VertexLayoutType::CUSTOM_LAYOUT) {
		if(commonLayouts[layoutType]->GetRefCount() == 1) {
			commonLayouts[layoutType].Clear();
		}
	}
	vertexElements = 0;

}

void MeshVertexData::ApplyCustomLayout() {
	if (vertexElements) {
		layout = Assign( NEX_NEW(VertexLayout()) );
		layout->Create(numVertexElements, vertexElements);
		NEX_DELETE_ARRAY(vertexElements);
		vertexElements = nullptr;
	}
}

void MeshVertexData::SetLayoutFromType() {
	if (layoutType == VertexLayoutType::CUSTOM_LAYOUT) {
		ApplyCustomLayout();
	} else if ((uint32)layoutType < (uint32)VertexLayoutType::VERTEX_LAYOUT_COUNT ){
		if (!commonLayouts[layoutType]) {
			commonLayouts[layoutType] = Assign( NEX_NEW(VertexLayout()) );
			commonLayouts[layoutType]->Create(
					commonElementLayout[layoutType].numElements,
					commonElementLayout[layoutType].elements);
		}
		layout = commonLayouts[layoutType];
	}
}

/*********************************
 * Mesh
 *********************************/
MeshAsset::MeshAsset(const StringID name) :
		Asset(name), vertexDeformationsEnabled(false), sharedVertexData(0), sharedIndexData(
				0) {
}

MeshAsset::~MeshAsset() {
}

uint32 MeshAsset::GetClassID() const {
	return CLASS_ID;
}

void MeshAsset::UnloadImpl(nextar::StreamRequest* req, bool isAsync) {
	if (sharedVertexData) {
		NEX_DELETE(sharedVertexData);
		sharedVertexData = 0;
	}
	if (sharedIndexData) {
		NEX_DELETE(sharedIndexData);
		sharedIndexData = 0;
	}
	PrimitiveGroupList localPrimitives;
	primitives.swap(localPrimitives);
}

void MeshAsset::_FillVertexData(MeshVertexData* data,
		MeshBufferData::BufferList::iterator& vertexBufferIt) {
	/* create vertex binding */
	uint32 numVertexBuffers = data->binding.GetBufferCount();
	for (uint32 stream = 0; stream < numVertexBuffers;
			++stream, ++vertexBufferIt) {
		ByteStream& byteData = (*vertexBufferIt);
		uint8* dataPtr = byteData.data();
		uint32 stride = *reinterpret_cast<uint32*>(dataPtr);
		dataPtr += 4;
		VertexBuffer buffer(GpuBuffer::RelocationPolicy::NEVER_RELEASED);
		buffer.CreateBuffer(byteData.size(), stride, dataPtr);
		data->binding.BindBuffer(stream, buffer);
	}

	data->SetLayoutFromType();
}

void MeshAsset::_FillIndexData(MeshIndexData* data,
		MeshBufferData::BufferList::iterator& indexBufferIt) {
	ByteStream& byteData = (*indexBufferIt);
	data->ibdata = IndexBuffer(GpuBuffer::RelocationPolicy::NEVER_RELEASED);
	data->ibdata.CreateBuffer(byteData.size(), data->twoBytePerElement ?
			IndexBuffer::TYPE_16BIT : IndexBuffer::TYPE_32BIT, byteData.data());
	indexBufferIt++;
}

void MeshAsset::NotifyAssetLoaded() {
	MeshAsset::StreamRequest* request =
			static_cast<MeshAsset::StreamRequest*>(GetStreamRequest());
	defaultSharedMaterial = request->sharedMaterial;
	sharedIndexData = request->sharedIndexData;
	sharedVertexData = request->sharedVertexData;
	boundsInfo = request->boundsInfo;
	primitives.swap(request->primitives);
	/* Ready vertex buffers */
	MeshBufferData& bufferData = request->bufferData;
	MeshBufferData::BufferList::iterator vertexBufferIt =
			bufferData.vertexBuffers.begin();
	MeshBufferData::BufferList::iterator indexBufferIt =
			bufferData.indexBuffers.begin();
	if (sharedVertexData) {
		_FillVertexData(sharedVertexData, vertexBufferIt);
	}
	if (sharedIndexData) {
		_FillIndexData(sharedIndexData, indexBufferIt);
	}

	for (auto &p : primitives) {
		if (p.vertexData) {
			_FillVertexData(p.vertexData, vertexBufferIt);
		}
		if (p.indexData) {
			_FillIndexData(p.indexData, indexBufferIt);
		}
	}
}

void MeshAsset::NotifyAssetUnloaded() {
	// todo
}

void MeshAsset::NotifyAssetUpdated() {
	// todo
}

StreamRequest* MeshAsset::CreateStreamRequestImpl(bool load) {
	return NEX_NEW(MeshAsset::StreamRequest(this));
}

void MeshAsset::DestroyStreamRequestImpl(StreamRequest*& request,
		bool load/*=true*/) {
	NEX_DELETE(request);
	request = nullptr;
}

void MeshAsset::EndianFlip(void* data, const VertexElement* veBegin,
		const VertexElement* veEnd, uint32 count, uint32 stride) {
	for (const VertexElement* j = veBegin; j != veEnd; ++j) {
		uint8* tempBuff = static_cast<uint8*>(data);
		const VertexElement& element = (*j);
		for (size_t i = 0; i < count; ++i) {
			uint8* udata = tempBuff + i * stride;
			switch (VertexSemantic::GetComponentBaseType(
					(VertexComponentType) element.desc.semantic.type)) {
			case COMP_BASE_TYPE_UNSIGNED_INT:
			case COMP_BASE_TYPE_FLOAT: {
				uint32& value = *reinterpret_cast<uint32*>(udata
						+ element.desc.offset);
				value = Endian::Swap32(value);
			}
				break;
			case COMP_BASE_TYPE_UNSIGNED_SHORT: {
				uint16& value = *reinterpret_cast<uint16*>(udata
						+ element.desc.offset);
				value = Endian::Swap16(value);
			}
				break;
			}
		}
	}

}

/*********************************
 * Mesh::StreamRequest
 *********************************/
MeshAsset::StreamRequest::StreamRequest(MeshAsset* mesh) :
		AssetStreamRequest(mesh), sharedVertexData(0), sharedIndexData(0) {
}

ByteStream& MeshAsset::StreamRequest::AddIndexBuffer() {
	bufferData.indexBuffers.resize(bufferData.indexBuffers.size() + 1);
	return bufferData.indexBuffers.back();
}

ByteStream& MeshAsset::StreamRequest::AddVertexBuffer() {
	bufferData.vertexBuffers.resize(bufferData.vertexBuffers.size() + 1);
	return bufferData.vertexBuffers.back();
}

/*********************************
 * MeshBufferData
 *********************************/
}
