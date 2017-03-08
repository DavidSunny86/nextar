/*
 * ShaderParam.cpp
 *
 *  Created on: 06-Oct-2013
 *      Author: obhi
 */

#include <ShaderParam.h>

namespace nextar {

uint32 ConstantParameter::stride = 0;
uint32 SamplerParameter::stride = 0;


AutoParam::AutoParamList AutoParam::autoParams;

ParamDataType ShaderParameter::MapParamType(const String& typeName) {
	typedef std::pair<const char*, ParamDataType> ParamTypeNamePair;
	static ParamTypeNamePair paramTypeNameTable[] = {
		ParamTypeNamePair("bool", ParamDataType::PDT_BOOL),
		ParamTypeNamePair("count", ParamDataType::PDT_COUNT),
		ParamTypeNamePair("float", ParamDataType::PDT_FLOAT),
		ParamTypeNamePair("int", ParamDataType::PDT_INT),
		ParamTypeNamePair("ivec2", ParamDataType::PDT_IVEC2),
		ParamTypeNamePair("ivec3", ParamDataType::PDT_IVEC3),
		ParamTypeNamePair("ivec4", ParamDataType::PDT_IVEC4),
		ParamTypeNamePair("mat3x4", ParamDataType::PDT_MAT3x4),
		ParamTypeNamePair("mat4x4", ParamDataType::PDT_MAT4x4),
		ParamTypeNamePair("struct", ParamDataType::PDT_STRUCT),
		ParamTypeNamePair("texture", ParamDataType::PDT_TEXTURE),
		ParamTypeNamePair("uint", ParamDataType::PDT_UINT),
		ParamTypeNamePair("unknown", ParamDataType::PDT_UNKNOWN),
		ParamTypeNamePair("vec2", ParamDataType::PDT_VEC2),
		ParamTypeNamePair("vec3", ParamDataType::PDT_VEC3),
		ParamTypeNamePair("vec4", ParamDataType::PDT_VEC4),
	};
	static const uint32 arraySize = sizeof(paramTypeNameTable) / sizeof(paramTypeNameTable[0]);
	ParamTypeNamePair* last = paramTypeNameTable + arraySize;
	ParamTypeNamePair* ptr = std::lower_bound(paramTypeNameTable, last, typeName,
		[](const ParamTypeNamePair& p1, const String& searchText) -> bool {
		return searchText.compare(p1.first) > 0;
	});


	if (ptr && ptr != last && !(typeName.compare(ptr->first) < 0)) {
		return ptr->second;
	}

	return ParamDataType::PDT_UNKNOWN;
}

ParamDataBaseType ShaderParameter::GetBaseType(ParamDataType type) {
	switch (type) {
	case ParamDataType::PDT_BOOL:
		return ParamDataBaseType::BASE_BYTE;
	case ParamDataType::PDT_UINT:
	case ParamDataType::PDT_INT:
	case ParamDataType::PDT_IVEC2:
	case ParamDataType::PDT_IVEC3:
	case ParamDataType::PDT_IVEC4:
		return ParamDataBaseType::BASE_INT32;
	case ParamDataType::PDT_TEXTURE:
		// pointer??
		return ParamDataBaseType::BASE_TEXTURE;
	case ParamDataType::PDT_FLOAT:
	case ParamDataType::PDT_VEC2:
	case ParamDataType::PDT_VEC3:
	case ParamDataType::PDT_VEC4:
	case ParamDataType::PDT_MAT4x4:
	case ParamDataType::PDT_MAT3x4:
		return ParamDataBaseType::BASE_FLOAT;
	case ParamDataType::PDT_STRUCT:
	case ParamDataType::PDT_UNKNOWN:
	default:
		return ParamDataBaseType::BASE_UNKNOWN;
	}
}


/****************************************************/
/* CustomTextureProcessor
 /****************************************************/
void CustomTextureProcessorApply(CommitContext& context,
		const ShaderParameter* param) {
	NEX_ASSERT(param->autoName == AutoParamName::AUTO_CUSTOM_CONSTANT);
	const SamplerParameter* sampler =
			reinterpret_cast<const SamplerParameter*>(param);
	CommitContext::ParamContext& pc = context.paramContext;
	//@urgent Need provision for default parameters
	NEX_ASSERT(pc.second);

	const TextureUnit* tu = pc.second->AsTexture(pc.first);
	context.pass->SetTexture(context.renderContext, *sampler,
			tu->texture);
	pc.first += sampler->size;
}

/****************************************************/
/* CustomParameterProcessor
 /****************************************************/
void CustomParameterProcessorApply(CommitContext& context,
		const ShaderParameter* param) {
	NEX_ASSERT(param->autoName == AutoParamName::AUTO_CUSTOM_CONSTANT);
	const ConstantParameter* constParam =
			reinterpret_cast<const ConstantParameter*>(param);
	CommitContext::ParamContext& pc = context.paramContext;
	//@urgent Need provision for default parameters
	NEX_ASSERT(pc.second);

	context.paramGroup->SetRawBuffer(context.renderContext, *constParam,
			pc.second->AsRawData(pc.first));
	pc.first += param->size;
}

/****************************************************/
/* CustomStructProcessor
 /****************************************************/
void CustomStructProcessorApply(CommitContext& context,
		const ShaderParameter* param) {
	NEX_ASSERT(param->type == ParamDataType::PDT_STRUCT);

	CommitContext::ParamContext& pc = context.paramContext;
	//@urgent Need provision for default parameters
	NEX_ASSERT(pc.second);

	uint32 size = param->size;
	context.paramGroup->WriteRawData(context.renderContext,
			pc.second->AsRawData(pc.first), 0, size);
	pc.first += size;
}

const AutoParam* AutoParam::MapParam(AutoParamName name) {
	if(name >= 0 && name < AutoParamName::AUTO_COUNT) {
		if (autoParams[name].autoName != AutoParamName::AUTO_INVALID_PARAM)
			return &autoParams[name];
	}
	return nullptr;
}

uint32 AutoParam::MapSamplerParams(const String& name,
	const TextureDescMap& texMap, ParameterContext& context) {
	for(uint32 i = 0; i < texMap.size(); ++i) {
		size_t w = texMap[i].unitsBound.find(name);
		if (w != String::npos) {
			size_t dot = texMap[i].unitsBound.find('.', w);
			if (dot != String::npos && dot + 1 < texMap[i].unitsBound.length()) {
				context = ShaderParameter::GetContextFromKey(texMap[i].unitsBound[dot + 1]);
			} else {
				context = ParameterContext::CTX_UNKNOWN;
			}
			return i;
		}
	}
	return -1;
}

void AutoParam::AddParamDef(AutoParamName autoName, ParamDataType type, ParameterContext context,
	ParamProcessorProc processor, uint32 size/*, const String& desc*/) {
	auto& param = autoParams[autoName];
	param.autoName = autoName;
	param.context = context;
	param.size = size;
	//param.desc = desc;
	param.processor = processor;
	param.type = type;
}

ParamProcessorProc AutoParam::customConstantProcessor =
		&CustomParameterProcessorApply;
ParamProcessorProc AutoParam::customTextureProcessor =
		&CustomTextureProcessorApply;
ParamProcessorProc AutoParam::customStructProcessor =
		&CustomStructProcessorApply;

} /* namespace nextar */

