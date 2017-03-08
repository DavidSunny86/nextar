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
	uint32 arrayCount;
	uint32 size;
	ParamDataType type;
	ParameterContext context;
	AutoParamName autoName;

	static ParamDataType MapParamType(const String& typeName);
	static ParamDataBaseType GetBaseType(ParamDataType type);
	inline static char GetContextKey(ParameterContext context) {
		return '1' + (int8)context;
	}
	inline static ParameterContext GetContextFromKey(char c) {
		return static_cast<ParameterContext>(c - '1');
	}
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

	// This is the buffer offset in GPU buffer
	// it is supposed to be the offset in uniform buffer
	uint32 bufferOffset;
	static uint32 stride;
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

	static uint32 stride;
};

class _NexEngineAPI ParameterGroup: public ShaderParameter {
public:

	inline ConstantParameter* GetParamByIndex(uint32 index) {
		return ConstantParameter::At(parameter, index);
	}

	inline void InvalidateContents() {
		lastUpdateId = -1;
	}

	virtual void* Map(RenderContext* rc) = 0;
	virtual void SetRawBuffer(RenderContext* rc, const ConstantParameter& desc,
			const void* data) = 0;
	virtual void Unmap(RenderContext* rc) = 0;

	virtual void WriteRawData(RenderContext* rc, const void* data,
			size_t offset, size_t size) = 0;

	ConstantParameter* parameter;
	uint32 lastUpdateId;
	uint32 numParams;

protected:
	~ParameterGroup() {
	}
};

struct ParameterGroupData {
	ParameterGroup* group;
	// Internally this might refer to the actual index, where
	// this buffer is bound for the current pass, because
	// a single buffer might be shared by multiple passes.
	ptrdiff_t _reserved;

	inline ParameterGroup* operator ->() {
		return group;
	}

	inline const ParameterGroup* operator ->() const {
		return group;
	}
};
typedef vector<ParameterGroupData>::type ParameterGroupList;

struct ParameterGroupItem {
	// Given a paramter buffer, this will give us the offset
	// at which point the series of data belongs to this set of groups
	uint32 offsetInParamBuffer;
	ParameterGroupList::iterator beginIt;
	ParameterGroupList::iterator endIt;
	SamplerParameter* beginSamplerIt;
	SamplerParameter* endSamplerIt;

};

struct SamplerDesc {
	// comma/space seperated units bound to this sampler
	String unitsBound;
	TextureUnitParams texUnitParams;
};

typedef vector<SamplerDesc>::type TextureDescMap;
typedef array<ParameterGroupItem, (size_t) ParameterContext::CTX_COUNT>::type ParameterGroupEntries;

struct _NexEngineAPI AutoParam: public AllocGeneral {
	ParamDataType type;
	AutoParamName autoName;
	ParameterContext context;
	ParamProcessorProc processor;
	uint32 size;


	static ParamProcessorProc customConstantProcessor;
	static ParamProcessorProc customTextureProcessor;
	static ParamProcessorProc customStructProcessor;

	typedef array<AutoParam, (size_t)AutoParamName::AUTO_COUNT>::type AutoParamList;
	static AutoParamList autoParams;

	AutoParam() : autoName(AutoParamName::AUTO_INVALID_PARAM),
				processor(nullptr),
				type(PDT_UNKNOWN),
				context(ParameterContext::CTX_UNKNOWN),
				size(0) {
	}

	static inline ParamProcessorProc GetStructProcessor() {
		return customStructProcessor;
	}

	static inline ParamProcessorProc GetConstantProcessor() {
		return customConstantProcessor;
	}

	static inline ParamProcessorProc GetTextureProcessor() {
		return customTextureProcessor;
	}


	static void AddParamDef(AutoParamName autoName, ParamDataType type, ParameterContext context,
		ParamProcessorProc processor, uint32 size/*, const String& desc*/);

	static const AutoParam* MapParam(AutoParamName name);

	static uint32 MapSamplerParams(const String& name,
			const TextureDescMap& texMap, ParameterContext& context);

};

struct ParamEntry {
	ParameterContext context;
	ParamDataType type;
	AutoParamName autoName;
	uint32 arrayCount;
	uint32 maxSize;
	StringID name;
};

typedef vector<ParamEntry>::type ParamEntryTable;
struct ParamEntryTableItem {
	ParamEntryTable::const_iterator beginIt;
	ParamEntryTable::const_iterator endIt;
	uint32 totalParamBufferSize;
};


} // namespace nextar
#endif // SHADERPARAM_H_
