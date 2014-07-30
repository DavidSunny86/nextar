/*
 * PropertyBuffer.cpp
 *
 *  Created on: 12-Mar-2014
 *      Author: obhi
 */
#include <NexEngine.h>
#include <ParameterBuffer.h>
#include <Asset.h>

namespace nextar {

ParameterBuffer::ParameterBuffer() :
		size(0), data(nullptr) {
}

ParameterBuffer::ParameterBuffer(const ParameterBuffer& pb) {
	*this = pb;
}

ParameterBuffer::ParameterBuffer(ParameterBuffer&& pb) {
	*this = std::move(pb);
}

ParameterBuffer::~ParameterBuffer() {
	uint8* buffer = data.get();
	for (auto it = paramTable.beginIt; it != paramTable.endIt; ++it) {
		if( (*it).type == ParamDataType::PDT_TEXTURE ) {
			TextureUnit* unit = reinterpret_cast<TextureUnit*>(buffer);
			if (unit->texture && unit->texture->IsTextureAsset()) {
				static_cast<TextureAsset*>(unit->texture)->Release();
			}
		}
		buffer += (*it).maxSize;
	}
}

void ParameterBuffer::Prepare(const ParamEntryTableItem& table) {
	paramTable = table;
	size = table.totalParamBufferSize;
	data.reset((uint8*) NEX_ALLOC(table.totalParamBufferSize, MEMCAT_CACHEALIGNED));
}

void ParameterBuffer::Prepare(void* data, size_t size) {
	this->size = size;
	this->data.reset((uint8*) NEX_ALLOC(size, MEMCAT_CACHEALIGNED));
	if (data)
		std::memcpy(this->data.get(), data, size);
}

void ParameterBuffer::Prepare(BufferPtr&& data, size_t size) {
	this->size = size;
	this->data = std::move(data);
}

const ParamEntry* ParameterBuffer::_GetParameter(const String& name) const {
	auto it = paramTable.beginIt;
	while (it != paramTable.endIt) {
		if ((*it).name == name)
			return &(*it);
	}
	return nullptr;
}

void ParameterBuffer::SetData(const void* data, size_t offset, size_t size) {
	if (data && this->data)
		std::memcpy(this->data.get() + offset, data, size);
}

void ParameterBuffer::SetData(const TextureUnit* data, size_t offset) {
	if (data && this->data) {
		std::memcpy(this->data.get() + offset, data, sizeof(TextureUnit));
		if(data->texture &&
				data->texture->IsTextureAsset()) {
			static_cast<TextureAsset*>(data->texture)->AddRef();
		}
	}
}

ParameterBuffer& ParameterBuffer::operator=(const ParameterBuffer& pb) {
	size = pb.size;
	paramTable = pb.paramTable;
	this->data.reset((uint8*) NEX_ALLOC(size, MEMCAT_CACHEALIGNED));
	if (pb.data)
		std::memcpy(this->data.get(), pb.data.get(), size);
	return *this;
}

ParameterBuffer& ParameterBuffer::operator=(ParameterBuffer&& pb) {
	size = pb.size;
	paramTable = std::move(pb.paramTable);
	data = std::move(pb.data);
	return *this;
}

void ParameterBuffer::Load(InputSerializer& ser, AssetStreamRequest* request) {
	ser >> size;
	if (size) {
		this->data.reset((uint8*) NEX_ALLOC(size, MEMCAT_CACHEALIGNED));
		uint8* buffer = data.get();
		uint8 baseType;
		uint32 count = 0;
		ser >> baseType;
		switch(baseType) {
		case ParamDataBaseType::BASE_BYTE: {
			ser >> count;
			InputSerializer::ByteArray arr(reinterpret_cast<int8*>(buffer), count);
			ser >> arr;
		}
			break;
		case ParamDataBaseType::BASE_INT32: {
			ser >> count;
			InputSerializer::IntArray arr(reinterpret_cast<int32*>(buffer), count);
			ser >> arr;
			count *= 4;
		}
			break;
		case ParamDataBaseType::BASE_INT64:	{
			ser >> count;
			InputSerializer::Int64Array arr(reinterpret_cast<int64*>(buffer), count);
			ser >> arr;
			count *= 8;
		}
			break;
		case ParamDataBaseType::BASE_FLOAT: {
			ser >> count;
			InputSerializer::FloatArray arr(reinterpret_cast<float*>(buffer), count);
			ser >> arr;
			count *= 4;
		}
			break;
		case ParamDataBaseType::BASE_TEXTURE: {
			TextureUnit* tu = reinterpret_cast<TextureUnit*>(buffer);
			bool asset = false;
			ser >> asset;
			if (asset) {
				TextureAsset::ID id;
				URL url;
				ser >> id;
				ser >> url;
				TextureAssetPtr assetPtr = TextureAsset::Traits::Instance(id, url);
				if (request)
					request->GetMetaInfo().AddDependency(assetPtr);
				else
					assetPtr->Load(false);
				tu->texture = assetPtr;
				assetPtr->AddRef();
			} else
				tu->texture = nullptr;
			count = sizeof(TextureUnit);
		}
			break;
		}
		buffer += count;
	}
}

void ParameterBuffer::Save(OutputSerializer& ser) const {
	ser << size;
	if (size) {
		const uint8* buffer = data.get();
		for(auto it = paramTable.beginIt; it != paramTable.endIt; ++it) {
			uint8 baseType = ShaderParameter::GetBaseType((*it).type);
			uint32 count = 0;
			ser << baseType;
			switch(baseType) {
			case ParamDataBaseType::BASE_BYTE: {
				count = (*it).maxSize;
				ser << count;
				OutputSerializer::ByteArray arr(reinterpret_cast<const int8*>(buffer), count);
				ser << arr;
			}
				break;
			case ParamDataBaseType::BASE_INT32: {
				count = (*it).maxSize / 4;
				ser << count;
				OutputSerializer::IntArray arr(reinterpret_cast<const int32*>(buffer), count);
				ser << arr;
			}
				break;
			case ParamDataBaseType::BASE_INT64:	{
				count = (*it).maxSize / 8;
				ser << count;
				OutputSerializer::Int64Array arr(reinterpret_cast<const int64*>(buffer), count);
				ser << arr;
			}
				break;
			case ParamDataBaseType::BASE_FLOAT: {
				count = (*it).maxSize / 4;
				ser << count;
				OutputSerializer::FloatArray arr(reinterpret_cast<const float*>(buffer), count);
				ser << arr;
			}
				break;
			case ParamDataBaseType::BASE_TEXTURE: {
				const TextureUnit* tu = reinterpret_cast<const TextureUnit*>(buffer);
				bool asset = tu->texture->IsTextureAsset();
				ser << asset;
				if (asset && tu->texture) {
					TextureAsset* assetPtr = static_cast<TextureAsset*>(tu->texture);
					TextureAsset::ID id;
					URL url = assetPtr->GetAssetLocator();
					assetPtr->GetID(id);
					ser << id;
					ser << url;
				}
			}
				break;
			}
			buffer += (*it).maxSize;
		}
	}
}

} /* namespace nextar */
