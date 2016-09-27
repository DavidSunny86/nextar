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
	id = (uint16)(ptrdiff_t)this;
	ticket = 0;
}

ParameterBuffer::ParameterBuffer(const ParameterBuffer& pb) : ParameterBuffer() {
	*this = pb;
}

ParameterBuffer::ParameterBuffer(ParameterBuffer&& pb) : ParameterBuffer() {
	*this = std::move(pb);
}

ParameterBuffer::~ParameterBuffer() {
	Clear();
}

void ParameterBuffer::Clear() {
	uint8* buffer = data.get();
	if (!buffer || !size)
		return;
	data.reset(nullptr);
	assetRefs.clear();
}

bool ParameterBuffer::Validate(const ParamEntryTableItem& table) {
	paramTable = table;
	if (size != table.totalParamBufferSize)
		return false;
	return true;
}

void ParameterBuffer::Prepare(const ParamEntryTableItem& table) {
	paramTable = table;
	size = table.totalParamBufferSize;
	data.reset((uint8*) NEX_ALLOC(table.totalParamBufferSize, MEMCAT_CACHEALIGNED));
	std::memset(this->data.get(), 0, size);
	ticket++;
}

void ParameterBuffer::Prepare(void* data, uint32 size) {
	this->size = size;
	this->data.reset((uint8*) NEX_ALLOC(size, MEMCAT_CACHEALIGNED));
	if (data)
		std::memcpy(this->data.get(), data, size);
	else
		std::memset(this->data.get(), 0, size);
	ticket++;
}

void ParameterBuffer::Prepare(BufferPtr&& data, uint32 size) {
	this->size = size;
	this->data = std::move(data);
	ticket++;
}

ParamEntryTable::const_iterator ParameterBuffer::_Find(const String& name) const {
	auto it = paramTable.beginIt;
	while (it != paramTable.endIt) {
		NEX_ASSERT((*it).name);
		if (*(*it).name == name)
			return it;
		++it;
	}
	return paramTable.endIt;
}

std::pair<const ParamEntry*, uint32> ParameterBuffer::GetParamEntryAndOffset(const String& name) const {
	uint32 offset = 0;
	auto it = paramTable.beginIt;
	while (it != paramTable.endIt) {
		NEX_ASSERT((*it).name);
		if (*(*it).name == name)
			return std::pair<const ParamEntry*, uint32>(&(*it), offset);
		offset += (*it).maxSize;
		++it;
	}
	return std::pair<const ParamEntry*, uint32>(nullptr, 0);
}

ParamDataType ParameterBuffer::GetType(const String& name) const {
	auto it = paramTable.beginIt;
	while (it != paramTable.endIt) {
		NEX_ASSERT((*it).name);
		if (*(*it).name == name)
			return (*it).type;
		++it;
	}
	return ParamDataType::PDT_UNKNOWN;
}

uint32 ParameterBuffer::GetOffset(const String& name) const {
	uint32 offset = 0;
	auto it = paramTable.beginIt;
	while (it != paramTable.endIt) {
		NEX_ASSERT((*it).name);
		if (*(*it).name == name)
			return offset;
		offset += (*it).maxSize;
		++it;
	}
	return -1;
}

const ParamEntry* ParameterBuffer::_GetParameter(const String& name) const {
	auto it = paramTable.beginIt;
	while (it != paramTable.endIt) {
		NEX_ASSERT((*it).name);
		if (*(*it).name == name)
			return &(*it);
		++it;
	}
	return nullptr;
}

void ParameterBuffer::SetData(const void* data, uint32 offset, uint32 size) {
	if (data && this->data)
		std::memcpy(this->data.get() + offset, data, size);
	ticket++;
}

void ParameterBuffer::SetData(const TextureUnit* data, uint32 offset) {
	if (data && this->data) {
		TextureUnit& unit = *reinterpret_cast<TextureUnit*>(this->data.get() + offset);
		if (unit.texture) {
			if (unit.texture &&
				unit.texture->IsTextureAsset()) {
				AssetPtr asset = Bind(static_cast<TextureAsset*>(unit.texture));
				assetRefs.remove(asset);
			}
		}
		std::memcpy(this->data.get() + offset, data, sizeof(TextureUnit));
		if (unit.texture && unit.texture->IsTextureAsset()) {
			AssetPtr asset = Bind(static_cast<TextureAsset*>(unit.texture));
			assetRefs.push_back(asset);
		}
	}
	ticket++;
}

ParameterBuffer& ParameterBuffer::operator=(const ParameterBuffer& pb) {
	size = pb.size;
	paramTable = pb.paramTable;
	this->data.reset((uint8*) NEX_ALLOC(size, MEMCAT_CACHEALIGNED));
	if (pb.data)
		std::memcpy(this->data.get(), pb.data.get(), size);
	uint8* buffer = this->data.get();
	if (!buffer || !size)
		return *this;
	for (auto it = paramTable.beginIt; it != paramTable.endIt; ++it) {
		if ((*it).type == ParamDataType::PDT_TEXTURE) {
			TextureUnit* unit = reinterpret_cast<TextureUnit*>(buffer);
			if (unit->texture && unit->texture->IsTextureAsset()) 
				static_cast<TextureAsset*>(unit->texture)->AddRef();
		}
	}
	ticket++;
	return *this;
}

ParameterBuffer& ParameterBuffer::operator=(ParameterBuffer&& pb) {
	size = pb.size;
	paramTable = std::move(pb.paramTable);
	data = std::move(pb.data);
	assetRefs = std::move(pb.assetRefs);
	ticket++;
	return *this;
}

void ParameterBuffer::AsyncLoad(InputSerializer& ser, AssetStreamRequest* request) {
	ser >> size;
	if (size) {
		uint32 numParams;
		this->data.reset((uint8*)NEX_ALLOC(size, MEMCAT_CACHEALIGNED));
		std::memset(this->data.get(), 0, size);
		uint8* buffer = data.get();
		uint8 baseType;
		uint32 count = 0;
		ser >> numParams;

		for (uint32 i = 0; i < numParams; ++i) {
			ser >> baseType;
			switch (baseType) {
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
						assetPtr->AsyncRequestLoad();
					tu->texture = assetPtr;
					assetRefs.push_back(assetPtr);
				} else
					tu->texture = nullptr;
				count = sizeof(TextureUnit);
			}
			break;
			}
			buffer += count;
		}
	}
	ticket++;
}

void ParameterBuffer::AsyncSave(OutputSerializer& ser) const {
	ser << size;
	if (size) {
		const uint8* buffer = data.get();
		uint32 numParams = (uint32)std::distance(paramTable.beginIt, paramTable.endIt);
		ser << numParams;
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
				if (tu && tu->texture) {
					bool asset = tu->texture->IsTextureAsset();
					ser << asset;
					if (asset) {
						TextureAsset* assetPtr = static_cast<TextureAsset*>(tu->texture);
						TextureAsset::ID id;
						URL url = assetPtr->GetAssetLocator();
						assetPtr->GetID(id);
						ser << id;
						ser << url;
					}
				} else {
					ser << false;
				}
			}
				break;
			}
			buffer += (*it).maxSize;
		}
	}
}

} /* namespace nextar */
