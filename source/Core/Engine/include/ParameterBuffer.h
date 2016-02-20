/*
 * PropertyBuffer.h
 *
 *  Created on: 12-Mar-2014
 *      Author: obhi
 */

#ifndef PROPERTYBUFFER_H_
#define PROPERTYBUFFER_H_

#include <NexBase.h>
#include <ShaderParam.h>
#include <TextureUnitState.h>

namespace nextar {

class AssetStreamRequest;
class ParamterItearator;
/*
 *
 */
class _NexEngineAPI ParameterBuffer: public AllocGraphics {
public:
	typedef BasicUniquePtr<uint8, MEMCAT_CACHEALIGNED>::type BufferPtr;
	typedef STDPoolAllocator<AssetPtr,
		32,
		MEMCAT_GENERAL> PointerAllocator;

	typedef list<AssetPtr, AllocatorGeneral, PointerAllocator>::type AssetList;

	ParameterBuffer();
	ParameterBuffer(const ParameterBuffer&);
	ParameterBuffer(ParameterBuffer&&);
	~ParameterBuffer();

	void Clear();
	void Prepare(const ParamEntryTableItem& table);
	void Prepare(void* data, uint32 size);
	void Prepare(BufferPtr&& data, uint32 size);
	const TextureUnit* AsTexture(uint32 offset) const;
	const void* AsRawData(uint32 offset) const;
	void SetParamEntryTable(const ParamEntryTableItem& paramTable);
	uint32 GetSize() const;
	uint32 GetNumber() const;

	void AsyncLoad(InputSerializer& ser, AssetStreamRequest* request = nullptr);
	void AsyncSave(OutputSerializer& ser) const;

	void SetData(const void* data, uint32 offset, uint32 size);
	void SetData(const TextureUnit* data, uint32 offset);
	ParamDataType GetType(const String& name) const;
	uint32 GetOffset(const String& name) const;
	std::pair<const ParamEntry*, uint32> GetParamEntryAndOffset(const String& name) const;

	ParameterBuffer& operator =(const ParameterBuffer& pb);
	ParameterBuffer& operator =(ParameterBuffer&& pb);
	AssetList& GetAssets() {
		return assetRefs;
	}

protected:

	ParamEntryTable::const_iterator _Find(const String& name) const;

	const ParamEntry* _GetParameter(const String& name) const;

	AssetList assetRefs;
	union {
		struct {
			uint16 id;
			uint16 ticket;
		};
		uint32 number;
	};

	uint32 size;

	ParamEntryTableItem paramTable;
	//uint8 smallBuffer[16];
	BufferPtr data;
};

inline const TextureUnit* ParameterBuffer::AsTexture(uint32 offset) const {
	NEX_ASSERT(offset < size);
	return reinterpret_cast<const TextureUnit*>(data.get() + offset);
}

inline const void* ParameterBuffer::AsRawData(uint32 offset) const {
	NEX_ASSERT(offset < size);
	return static_cast<const void*>(data.get() + offset);
}

inline void ParameterBuffer::SetParamEntryTable(
		const ParamEntryTableItem& paramTable) {
	this->paramTable = paramTable;
}

inline uint32 ParameterBuffer::GetSize() const {
	return size;
}

inline uint32 ParameterBuffer::GetNumber() const {
	return number;
}

} /* namespace nextar */

#endif /* PROPERTYBUFFER_H_ */
