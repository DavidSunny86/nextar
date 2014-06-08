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

	enum class ParamDataType : uint16 {
		PDT_UNKNOWN,
		PDT_BOOL,
		PDT_UINT,
		PDT_INT,
		PDT_FLOAT,
		PDT_TEXTURE,
		PDT_VEC2,
		PDT_VEC3,
		PDT_VEC4,
		PDT_IVEC2,
		PDT_IVEC3,
		PDT_IVEC4,
		PDT_MAT4x4,
		PDT_MAT3x4,
		PDT_STRUCT,
		PDT_COUNT,
	};

	enum class AutoParamName : uint16 {
		// Time in seconds
		AUTO_TIME,
		// Elapsed time in seconds between two frames ( f(i) and f(i-1) )
		AUTO_ELAPSED_TIME,
		// Frame Number
		AUTO_FRAME_NUMBER,
		// Diffuse Color
		AUTO_DIFFUSE_COLOR,
		// Specualar power
		AUTO_SPECULAR_POWER,
		// World matrix.
		AUTO_WORLD,
		// Inverse world matrix, only 1 accepted
		AUTO_INV_WORLD,
		// View matrix.
		AUTO_VIEW,
		// Projection matrix.
		AUTO_PROJECTION,
		// World*View matrix.
		AUTO_WORLD_VIEW,
		// View*Projection matrix.
		AUTO_VIEW_PROJECTION,
		// World*View*Projection matrix.
		AUTO_WORLD_VIEW_PROJECTION,
		// Light position
		AUTO_LIGHT_POSITION,
		// Light direction
		AUTO_LIGHT_DIRECTION,
		// Light position
		AUTO_LIGHT_OBJ_SPACE_POSITION,
		// Light direction
		AUTO_LIGHT_OBJ_SPACE_DIRECTION,
		// Light color
		AUTO_LIGHT_SPECUALR_COLOR,
		// Ambient color
		AUTO_LIGHT_AMBIENT_COLOR,
		// Diffuse color
		AUTO_LIGHT_DIFFUSE_COLOR,
		// Atten range + coefficients color
		AUTO_LIGHT_ATTENUATION,
		// Light pov matrix used for shadow coordinates
		AUTO_LIGHT_POV_MATRIX,
		// Custom generic parameter, could be texture or constant
		// accessed from property buffer.
		AUTO_CUSTOM_CONSTANT,
		
		// Count
		AUTO_COUNT,
	};

	enum class ParameterContext : uint8 {
		// changes per frame: time
		// Type: shared
		CTX_FRAME = 0,
		// changes per view: view matrix
		// Type: shared
		CTX_VIEW,
		// Per pass change
		CTX_PASS,
		// changes per material instance: diffuseColor
		// Type: local to material
		CTX_MATERIAL,
		// changes per object instance: worldMatrix
		// Type: local to object/stored in shader
		CTX_OBJECT,
		CTX_COUNT
	};

	NEX_ENUM_UINT(ParameterContext, uint8);
	
	class AutoParamProcessor;
	
	/**
	 * This struct serves as a parameter entry. 
	 * Materials, Objects, Cameras will contain cpu buffers with paramter data.
	 * Most individual paramters will be grouped into structures (identified by groupId)
	 * so that they can be easily passed to a constant buffer and hence updated in one go.
	 * The other way is to obtain the data from various places, and update the buffer
	 * gradually.
	 * todo Profile the usage of this struct with pure uint32 ints.
	 */
	class _NexEngineAPI ShaderParameter : public AllocGeneral {
	public:
		AutoParamProcessor* processor;
		ParamDataType type;
		uint16 arrayCount;
		AutoParamName autoName;
		uint32 size;
	};

	class _NexEngineAPI ConstantParameter : public ShaderParameter {
	public:
		inline static ConstantParameter* Next(ConstantParameter* parameter) {
			return reinterpret_cast<ConstantParameter*>(
				reinterpret_cast<uint8*>(parameter) + ConstantParameter::stride);
		}

		inline static ConstantParameter* At(ConstantParameter* start, uint32 index) {
			return reinterpret_cast<ConstantParameter*>(
				reinterpret_cast<uint8*>(start) + index*ConstantParameter::stride);
		}

		static uint32 stride;
		ShaderParameter paramDesc;
		uint32 bufferOffset;
	};

	class _NexEngineAPI SamplerParameter : public ShaderParameter {
	public:
		inline static SamplerParameter* Next(SamplerParameter* parameter) {
			return reinterpret_cast<SamplerParameter*>(
				reinterpret_cast<uint8*>(parameter) + SamplerParameter::stride);
		}

		inline static SamplerParameter* At(SamplerParameter* start, uint32 index) {
			return reinterpret_cast<SamplerParameter*>(
				reinterpret_cast<uint8*>(start) + index*SamplerParameter::stride);
		}

		static uint32 stride;
		TextureUnit defaultTexture;
	};

	class _NexEngineAPI ParameterGroup : public ShaderParameter {
	public:

		inline ConstantParameter* GetParamByIndex(uint32 index) {
			return ConstantParameter::At(parameter, index);
		}

		virtual void Map(RenderContext* rc) = 0;
		virtual void SetRawBuffer(RenderContext* rc, const ConstantParameter& desc, const void* data) = 0;
		virtual void Unmap(RenderContext* rc) = 0;

		virtual void WriteRawData(RenderContext* rc, const void* data, size_t offset = 0, size_t size = 0) = 0;

		uint32 lastUpdateId;
		ParameterContext context;
		ConstantParameter* parameter;
		uint32 numParams;
	};

	typedef vector<ParameterGroup*>::type ParameterGroupList;
	
	struct ParameterGroupItem {
		ParameterGroupList::iterator beginIt;
		ParameterGroupList::iterator endIt;
		SamplerParameter* beginSamplerIt;
		SamplerParameter* endSamplerIt;

		uint32 offsetInParamBuffer;
	};

	typedef array<ParameterGroupItem, (size_t)ParameterContext::CTX_COUNT>::type ParameterGroupEntries;

	class AutoParamProcessor {
	public:
		virtual void Apply(CommitContext& context, const ShaderParameter* d) = 0;
	protected:
		~AutoParamProcessor() {}
	};

	struct AutoParam : public AllocGeneral {
		uint32 type;
		AutoParamName autoName;
		ParameterContext context;
		AutoParamProcessor* processor;
		// todo May not be useful except in UI
		String name;
		String desc;
	};

	struct ParamEntry {
		AutoParamName autoName;
		uint16 type;
		uint16 arrayCount;
		uint32 maxSize;
		String name;
	};

	typedef vector<ParamEntry>::type ParamEntryTable;
	typedef std::pair<ParamEntryTable::iterator, ParamEntryTable::iterator> ParamEntryTableItem;
	typedef std::pair<ParamEntryTable::const_iterator, ParamEntryTable::const_iterator> ConstParamEntryTableItem;

} // namespace nextar
#endif // SHADERPARAM_H_
