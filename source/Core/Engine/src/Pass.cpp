/*
 * Pass.cpp
 *
 *  Created on: 23-Nov-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <Pass.h>
#include <CommitContext.h>
#include <ShaderParam.h>
#include <RenderContext.h>

namespace nextar {

Pass::AutoParamList Pass::autoParams;

Pass::Pass(StringID name) :
		NamedObject(name),
		ContextObject(TYPE_PASS, 0)
		//,inputLayoutUniqueID(-1)
		,flags(0) {
}

Pass::Pass(Pass&& p) :
	NamedObject(p.GetID())
	,ContextObject(std::move(*static_cast<ContextObject*>(&p)))
	,flags(0) {
}

Pass::Pass(const Pass& p) {
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	flags = 0;
}

Pass::~Pass() {
}

const AutoParam* Pass::MapParam(AutoParamName name) {
	if(name >= 0 && name < AutoParamName::AUTO_COUNT) {
		if (autoParams[name].autoName != AutoParamName::AUTO_INVALID_PARAM)
			return &autoParams[name];
	}
	return nullptr;
}

uint32 Pass::MapSamplerParams(const String& name,
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

void Pass::AddParamDef(AutoParamName autoName, ParamDataType type, ParameterContext context,
	ParamProcessorProc processor, const String& desc) {
	auto& param = autoParams[autoName];
	param.autoName = autoName;
	param.context = context;
	param.desc = desc;
	param.processor = processor;
	param.type = type;
}

/****************************************************/
/* Pass::View
 /****************************************************/
Pass::View::View() :
		samplers(nullptr), numSamplerCount(0),
		viewNumber(0), frameNumber(0), passNumber(0) {
}

void Pass::View::Update(RenderContext* rc, uint32 msg, ContextParamPtr param) {
	if (msg == Pass::MSG_PASS_COMPILE) {
		const CompileParams& p = *reinterpret_cast<const CompileParams*>(param);
		//p.inputLayoutId = &inputLayoutUniqueID;
		p.inited.clear(std::memory_order_relaxed);
		Compile(rc, p);
	}
	else if (msg == Pass::MSG_PASS_UPDATE_PARAMBUFFER_OFFSET) {
		const ParamBufferOffsetParams& p =
			*reinterpret_cast<const ParamBufferOffsetParams*>(param);

		auto enParamIt = sharedParameters.end();
		auto paramIt = sharedParameters.begin();
		auto samplerIt = samplers;
		auto enSamplerIt = SamplerParameter::At(samplers, numSamplerCount);


		for (uint32 context = 0; context < (uint32)ParameterContext::CTX_COUNT; ++context) {
			auto& e = paramGroupEntries[context];
			
			if (paramIt != enParamIt && (*paramIt)->context == (ParameterContext)context) {
				e.beginIt = paramIt;
				do {
					paramIt++;
				} while (paramIt != enParamIt && (*paramIt)->context == (ParameterContext)context);
				e.endIt = paramIt;
			} else {
				e.beginIt = e.endIt = enParamIt;
			}
			
			if (samplerIt != enSamplerIt && (*samplerIt).context == (ParameterContext)context) {
				e.beginSamplerIt = samplerIt;
				do {
					samplerIt = SamplerParameter::Next(samplerIt);
				} while (samplerIt != enSamplerIt && (*samplerIt).context == (ParameterContext)context);
				e.endSamplerIt = samplerIt;
			} else {
				e.beginSamplerIt = e.endSamplerIt = nullptr;
			}
			
			e.offsetInParamBuffer = p.offset[context];
		}
	}
}

void Pass::View::SwitchAndUpdateParams(CommitContext& context) {
	context.renderContext->SwitchPass(context, this);
	if (frameNumber != context.frameNumber) {
		UpdateParams(context, ParameterContext::CTX_FRAME, context.frameNumber);
		frameNumber = context.frameNumber;
	}
	if (viewNumber != context.viewNumber) {
		UpdateParams(context, ParameterContext::CTX_VIEW, context.viewNumber);
		viewNumber = context.viewNumber;
	}
	if (passNumber != context.passNumber) {
		UpdateParams(context, ParameterContext::CTX_PASS, context.passNumber);
		passNumber = context.passNumber;
	}
}

// todo Optimize.
void Pass::View::UpdateParams(CommitContext& ctx, ParameterContext type,
		uint32 id) {
	NEX_ASSERT(this == ctx.pass);

	ParameterGroupItem& item = paramGroupEntries[(uint32) type];
	ctx.paramContext = CommitContext::ParamContext(
			item.offsetInParamBuffer, ctx.paramBuffers[(uint32) type]);

	for (auto it = item.beginIt; it != item.endIt; ++it) {
		ParameterGroup* group = (*it);
		NEX_ASSERT(group->context == type);
		if (group->lastUpdateId == id)
			continue;
		ctx.paramGroup = group;
		if (group->processor) {
			ctx.groupDataPtr = nullptr;
			group->Map(ctx.renderContext);
			group->processor(ctx, group);
			group->Unmap(ctx.renderContext);
		} else {
			ctx.groupDataPtr = group->Map(ctx.renderContext);
			for (uint32 i = 0; i < group->numParams; ++i) {
				ConstantParameter* parameter = group->GetParamByIndex(i);
				NEX_ASSERT(parameter->processor);
				parameter->processor(ctx, parameter);
			}
			group->Unmap(ctx.renderContext);
		}
		(*it)->lastUpdateId = id;
	}

	ctx.paramGroup = nullptr;
	for (auto it = item.beginSamplerIt; it != item.endSamplerIt; it =
			SamplerParameter::Next(it)) {
		NEX_ASSERT(it->processor);
		if (it->context == type)
			it->processor(ctx, it);
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
		
	context.pass->SetTexture(context.renderContext, *sampler,
			pc.second->AsTexture(pc.first));
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

ParamProcessorProc Pass::customConstantProcessor =
		&CustomParameterProcessorApply;
ParamProcessorProc Pass::customTextureProcessor =
		&CustomTextureProcessorApply;
ParamProcessorProc Pass::customStructProcessor =
		&CustomStructProcessorApply;
} /* namespace nextar */
