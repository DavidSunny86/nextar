/*
 * ShaderParam.h
 *
 *  Created on: 06-Oct-2013
 *      Author: obhi
 */

#ifndef SHADERPARAM_H_
#define SHADERPARAM_H_

#include <NexBase.h>
#include <TextureUnitState.h>

namespace nextar {


class CommitContext;
class ShaderParameter;
typedef void(*ParamProcessorProc) (CommitContext& context, const ShaderParameter* d);

/**
 * This struct serves as a parameter entry. 
 * Materials, Objects, Cameras will contain cpu buffers with paramter data.
 * Most individual paramters will be grouped into structures (identified by groupId)
 * so that they can be easily passed to a constant buffer and hence updated in one go.
 * The other way is to obtain the data from various places, and update the buffer
 * gradually.
 * todo Profile the usage of this struct with pure uint32 ints.
 */
class _NexEngineAPI ShaderParameter: public AllocGeneral {
public:
	ParamProcessorProc processor;
	ParamDataType type;
	uint16 arrayCount;
	AutoParamName autoName;
	uint32 size;

	static ParamDataType MapParamType(const String& typeName);
	static ParamDataBaseType GetBaseType(ParamDataType type);
};

class _NexEngineAPI ConstantParameter: public ShaderParameter {
public:
	inline static ConstantParameter* Next(ConstantParameter* parameter) {
		return reinterpret_cast<ConstantParameter*>(reinterpret_cast<uint8*>(parameter)
				+ ConstantParameter::stride);
	}

	inline static ConstantParameter* At(ConstantParameter* start,
			uint32 index) {
		return reinterpret_cast<ConstantParameter*>(reinterpret_cast<uint8*>(start)
				+ index * ConstantParameter::stride);
	}

	static uint32 stride;
	uint32 bufferOffset;
};

class _NexEngineAPI SamplerParameter: public ShaderParameter {
public:
	inline static SamplerParameter* Next(SamplerParameter* parameter) {
		return reinterpret_cast<SamplerParameter*>(reinterpret_cast<uint8*>(parameter)
				+ SamplerParameter::stride);
	}

	inline static SamplerParameter* At(SamplerParameter* start, uint32 index) {
		return reinterpret_cast<SamplerParameter*>(reinterpret_cast<uint8*>(start)
				+ index * SamplerParameter::stride);
	}

	ParameterContext context;
	static uint32 stride;
};

class _NexEngineAPI ParameterGroup: public ShaderParameter {
public:

	inline ConstantParameter* GetParamByIndex(uint32 index) {
		return ConstantParameter::At(parameter, index);
	}

	virtual void* Map(RenderContext* rc) = 0;
	virtual void SetRawBuffer(RenderContext* rc, const ConstantParameter& desc,
			const void* data) = 0;
	virtual void Unmap(RenderContext* rc) = 0;

	virtual void WriteRawData(RenderContext* rc, const void* data,
			size_t offset, size_t size) = 0;

	uint32 lastUpdateId;
	ParameterContext context;
	ConstantParameter* parameter;
	uint32 numParams;

protected:
	~ParameterGroup() {
	}
};

typedef vector<ParameterGroup*>::type ParameterGroupList;

struct ParameterGroupItem {
	ParameterGroupList::iterator beginIt;
	ParameterGroupList::iterator endIt;
	SamplerParameter* beginSamplerIt;
	SamplerParameter* endSamplerIt;

	uint32 offsetInParamBuffer;

};

typedef array<ParameterGroupItem, (size_t) ParameterContext::CTX_COUNT>::type ParameterGroupEntries;

struct AutoParam: public AllocGeneral {
	ParamDataType type;
	AutoParamName autoName;
	ParameterContext context;
	ParamProcessorProc processor;
	String desc;

	AutoParam() : autoName(AutoParamName::AUTO_INVALID_PARAM),
			processor(nullptr),
			type(PDT_UNKNOWN),
			context(ParameterContext::CTX_UNKNOWN) {
	}
};

struct ParamEntry {
	ParameterContext context;
	uint8 passIndex;
	AutoParamName autoName;
	ParamDataType type;
	uint16 arrayCount;
	uint32 maxSize;
	String name;
};

typedef vector<ParamEntry>::type ParamEntryTable;
struct ParamEntryTableItem {
	ParamEntryTable::const_iterator beginIt;
	ParamEntryTable::const_iterator endIt;
	uint32 totalParamBufferSize;
};

} // namespace nextar
#endif // SHADERPARAM_H_
