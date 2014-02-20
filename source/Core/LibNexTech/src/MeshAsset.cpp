
#include "NexHeaders.h"
#include "MeshAsset.h"
#include "BufferManager.h"

namespace nextar {

	MeshVertexData::~MeshVertexData() {
		if (binding)
			NEX_DELETE binding;
		if (vertexElements)
			NEX_DELETE [] vertexElements;
		if (layout)
			NEX_DELETE layout;
		vertexElements = 0;

	}

	/*********************************
	 * Mesh
	 *********************************/
	MeshAsset::MeshAsset(const String& name)
			: Asset(name), vertexDeformationsEnabled(false), sharedVertexData(
					0), sharedIndexData(0) {
	}

	MeshAsset::~MeshAsset() {
	}

	uint32 MeshAsset::GetClassID() const {
		return CLASS_ID;
	}

	void MeshAsset::LoadImpl(nextar::StreamRequest* req, bool isAsync) {
		Loader loader(req);
		loader.Serialize();
	}

	void MeshAsset::UnloadImpl(nextar::StreamRequest* req, bool isAsync) {
		if (sharedVertexData) {
			NEX_DELETE sharedVertexData;
			sharedVertexData = 0;
		}
		if (sharedIndexData) {
			NEX_DELETE sharedIndexData;
			sharedIndexData = 0;
		}
	}

	void MeshAsset::_FillVertexData(MeshVertexData* data, MeshBufferData::BufferList::iterator& vertexBufferIt) {
		/* create vertex binding */
		data->binding = BufferManager::Instance().CreateVertexBufferBinding();
		data->binding->SetBufferCount(data->numVertexBuffers);
		for(uint32 stream = 0; stream < data->numVertexBuffers; ++stream, ++vertexBufferIt) {
			ByteStream& byteData = (*vertexBufferIt);
			VertexBufferPtr& streamPtr = data->streams[stream];
			streamPtr = BufferManager::Instance().CreateVertexBuffer(byteData.size(),
							Buffer::GPU_READ, RelocationPolicy::NEVER_RELEASE);
			streamPtr->Write(byteData.data(), 0, byteData.size());
			data->binding->BindBuffer(stream, streamPtr);
		}
		if (data->vertexElements) {
			data->layout = BufferManager::Instance().CreateVertexLayout(data->vertexElements,
					data->numVertexElements);
			NEX_DELETE [] data->vertexElements;
			data->vertexElements = nullptr;
		}
	}

	void MeshAsset::_FillIndexData(MeshIndexData* data, MeshBufferData::BufferList::iterator& indexBufferIt) {
		ByteStream& byteData = (*indexBufferIt);
		data->ibdata = BufferManager::Instance().CreateIndexBuffer(byteData.size(),
				Buffer::GPU_READ, data->twoBytePerElement ?
						IndexBuffer::TYPE_16BIT : IndexBuffer::TYPE_32BIT);
		indexBufferIt++;
	}

	void MeshAsset::NotifyAssetLoaded() {
		MeshAsset::StreamRequest* request = static_cast<MeshAsset::StreamRequest*>(GetStreamRequest());
		defaultSharedMaterial = request->sharedMaterial;
		sharedIndexData = request->sharedIndexData;
		sharedVertexData = request->sharedVertexData;
		boundsInfo = request->boundsInfo;
		primitives.swap(request->primitives);
		/* Ready vertex buffers */
		MeshBufferData& bufferData = request->bufferData;
		MeshBufferData::BufferList::iterator vertexBufferIt = bufferData.vertexBuffers.begin();
		MeshBufferData::BufferList::iterator indexBufferIt = bufferData.indexBuffers.begin();
		if (sharedVertexData) {
			_FillVertexData(sharedVertexData, vertexBufferIt);
		}
		if (sharedIndexData) {
			_FillIndexData(sharedIndexData, indexBufferIt);
		}

		for(auto &p : primitives) {
			if(p.vertexData) {
				_FillVertexData(p.vertexData, vertexBufferIt);
			}
			if(p.indexData) {
				_FillIndexData(p.indexData, indexBufferIt);
			}
		}
	}

	/*********************************
	 * Mesh::StreamRequest
	 *********************************/
	MeshAsset::StreamRequest::StreamRequest(MeshAsset* mesh) :
			AssetStreamRequest(mesh), sharedVertexData(0),
			sharedIndexData(0) {
	}

	ByteStream& MeshAsset::StreamRequest::AddIndexBuffer() {
		bufferData.indexBuffers.resize(bufferData.indexBuffers.size()+1);
		return bufferData.indexBuffers.back();
	}

	ByteStream& MeshAsset::StreamRequest::AddVertexBuffer() {
		bufferData.vertexBuffers.resize(bufferData.vertexBuffers.size()+1);
		return bufferData.vertexBuffers.back();
	}

	/*********************************
	 * Mesh::Loader
	 *********************************/
	MeshAsset::Loader::Loader(nextar::StreamRequest* req) : meshRequest(req) {
	}

	MeshAsset::Loader::~Loader() {
	}

	void MeshAsset::Loader::EndianFlip(void* data, const VertexElement* veBegin,
			const VertexElement* veEnd, size_t count) {

		uint16 stride = veBegin->desc.stride;

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
				case COMP_BASE_TYPE_UNSIGNED_SHORT:	{
						uint16& value = *reinterpret_cast<uint16*> (udata + element.desc.offset);
						value = Endian::Swap16(value);
					}
					break;
				}
			}
		}

	}

	void MeshAsset::Loader::Serialize() {
		MeshAsset* meshPtr = static_cast<MeshAsset*>(meshRequest->GetStreamedObject());
		const URL& location = meshPtr->GetAssetLocator();
		String ext = location.GetExtension();
		StringUtils::ToUpper(ext);
		MeshAsset::LoaderImpl* impl = GetImpl(ext);
		if (!impl) {
			Error("No mesh loader for type.");
			NEX_THROW_GracefulError(EXCEPT_MISSING_PLUGIN);
		}

		InputStreamPtr input = FileSystem::Instance().OpenRead(location);

		if (input) {
			impl->Load(input, *this);
		} else {
			Error(
					String("Could not open mesh file: ")
							+ meshPtr->GetAssetLocator().ToString());
			NEX_THROW_GracefulError(EXCEPT_COULD_NOT_LOCATE_ASSET);
		}
	}

	/*********************************
	 * MeshAsset::Factory
	 *********************************/
	MeshAsset::Factory::Factory(const String& name) : Asset::Factory(name) {
	}

	Component* MeshAsset::Factory::AsyncCreate(uint32 classId, const String& name) {
		if (classId == MeshAsset::CLASS_ID) {
			return NEX_NEW MeshAsset(name);
		}
		return nullptr;
	}

	MeshAssetPtr MeshAsset::Factory::AsyncCreateInstance(const String& name, 
		const URL& location) {
		MeshAssetPtr mesh = Bind(AsyncCreate(MeshAsset::CLASS_ID, name));
		if (mesh) {
			mesh->SetAssetLocator(location);
		}
		return mesh;
	}

	/*********************************
	 * MeshBufferData
	 *********************************/
}

