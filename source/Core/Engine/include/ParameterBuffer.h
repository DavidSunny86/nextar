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

	ParameterBuffer();
	ParameterBuffer(const ParameterBuffer&);
	ParameterBuffer(ParameterBuffer&&);
	virtual ~ParameterBuffer();

	void Prepare(const ParamEntryTableItem& table);
	void Prepare(void* data, uint32 size);
	void Prepare(BufferPtr&& data, uint32 size);

	const TextureUnit* AsTexture(uint32 offset) const {
		NEX_ASSERT(offset < size);
		return reinterpret_cast<const TextureUnit*>(data.get() + offset);
	}

	const void* AsRawData(uint32 offset) const {
		NEX_ASSERT(offset < size);
		return static_cast<const void*>(data.get() + offset);
	}

	void SetParamEntryTable(const ParamEntryTableItem& paramTable) {
		this->paramTable = paramTable;
	}

	inline uint32 GetSize() const {
		return size;
	}

	void AsyncLoad(InputSerializer& ser, AssetStreamRequest* request = nullptr);
	void AsyncSave(OutputSerializer& ser) const;

	void SetData(const void* data, uint32 offset, uint32 size);
	void SetData(const TextureUnit* data, uint32 offset);
	uint32 GetOffset(const String& name) const;

	ParameterBuffer& operator =(const ParameterBuffer& pb);
	ParameterBuffer& operator =(ParameterBuffer&& pb);
protected:

	const ParamEntry* _GetParameter(const String& name) const;

	uint32 textureAssetsBitField;
	uint32 size;

	ParamEntryTableItem paramTable;
	//uint8 smallBuffer[16];
	BufferPtr data;
};

} /* namespace nextar */

#endif /* PROPERTYBUFFER_H_ */
