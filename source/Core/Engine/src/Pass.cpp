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

Pass::AutoParamMap Pass::autoParams;

Pass::Pass() :
		ContextObject(TYPE_PASS), NamedObject(StringUtils::NullID)
		//,inputLayoutUniqueID(-1)
				, flags(0) {
}

Pass::~Pass() {
}

const AutoParam* Pass::MapParam(const String& name) {
	auto ap = autoParams.find(name);
	if (ap == autoParams.end())
		return nullptr;
	// todo map parsed params
	return (*ap).second;
}

const Pass::SamplerDesc* Pass::MapSamplerParams(const String& name,
		const TextureDescMap& texMap) {
	auto sp = texMap.find(name);
	if (sp == texMap.end())
		return nullptr;
	// todo map parsed params
	return &(*sp).second;
}

/****************************************************/
/* Pass::View
 /****************************************************/
Pass::View::View() :
		samplers(nullptr), numSamplerCount(0) {
}

void Pass::View::Update(RenderContext* rc, uint32 msg, ContextParamPtr param) {
	if (msg == Pass::MSG_PASS_COMPILE) {
		const CompileParams& p = *reinterpret_cast<const CompileParams*>(param);
		//p.inputLayoutId = &inputLayoutUniqueID;
		p.inited.store(0, std::memory_order_relaxed);
		Compile(rc, p);
	} else if (msg == Pass::MSG_PASS_UPDATE_PARAMBUFFER_OFFSET) {
		const ParamBufferOffsetParams& p =
				*reinterpret_cast<const ParamBufferOffsetParams*>(param);
		uint32 offsetAt = 0;
		for (auto& e : paramGroupEntries) {
			e.offsetInParamBuffer = p.offset[offsetAt++];
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
	ctx.currentParamContext = CommitContext::ParamContext(
			item.offsetInParamBuffer, ctx.paramBuffers[(uint32) type]);

	for (auto it = item.beginIt; it != item.endIt; ++it) {
		ParameterGroup* group = (*it);
		NEX_ASSERT(group->context == type);
		if (group->lastUpdateId == id)
			continue;
		ctx.currentGroup = group;
		if (group->processor) {
			group->processor->Apply(ctx, group);
		} else {
			group->Map(ctx.renderContext);
			for (uint32 i = 0; i < group->numParams; ++i) {
				ConstantParameter* parameter = group->GetParamByIndex(i);
				NEX_ASSERT(parameter->processor);
				parameter->processor->Apply(ctx, parameter);
			}
			group->Unmap(ctx.renderContext);
		}
		(*it)->lastUpdateId = id;
	}

	ctx.currentGroup = nullptr;
	for (auto it = item.beginSamplerIt; it != item.endSamplerIt; it =
			SamplerParameter::Next(it)) {
		NEX_ASSERT(it->processor);
		if (it->context == type)
			it->processor->Apply(ctx, it);
	}
}

/****************************************************/
/* CustomTextureProcessor
 /****************************************************/
class CustomTextureProcessor: public AutoParamProcessor {
public:
	static CustomTextureProcessor instance;

	virtual void Apply(CommitContext& context, const ShaderParameter* param);
protected:
	~CustomTextureProcessor() {
	}
};

void CustomTextureProcessor::Apply(CommitContext& context,
		const ShaderParameter* param) {
	NEX_ASSERT(param->autoName == AutoParamName::AUTO_CUSTOM_CONSTANT);
	const SamplerParameter* sampler =
			reinterpret_cast<const SamplerParameter*>(param);
	CommitContext::ParamContext& pc = context.currentParamContext;
	context.pass->SetTexture(context.renderContext, *sampler,
			pc.second->AsTexture(pc.first));
	pc.first += sampler->size;
}

/****************************************************/
/* CustomParameterProcessor
 /****************************************************/
class CustomParameterProcessor: public AutoParamProcessor {
public:
	static CustomParameterProcessor instance;

	virtual void Apply(CommitContext& context, const ShaderParameter* param);
protected:
	~CustomParameterProcessor() {
	}
};

void CustomParameterProcessor::Apply(CommitContext& context,
		const ShaderParameter* param) {
	NEX_ASSERT(param->autoName == AutoParamName::AUTO_CUSTOM_CONSTANT);
	const ConstantParameter* constParam =
			reinterpret_cast<const ConstantParameter*>(param);
	CommitContext::ParamContext& pc = context.currentParamContext;
	context.currentGroup->SetRawBuffer(context.renderContext, *constParam,
			pc.second->AsRawData(pc.first));
	pc.first += param->size;
}

/****************************************************/
/* CustomStructProcessor
 /****************************************************/
class CustomStructProcessor: public AutoParamProcessor {
public:
	static CustomStructProcessor instance;

	virtual void Apply(CommitContext& context, const ShaderParameter* param);
protected:
	~CustomStructProcessor() {
	}
	const uint32 paramContext;
};

void CustomStructProcessor::Apply(CommitContext& context,
		const ShaderParameter* param) {
	NEX_ASSERT(param->type == ParamDataType::PDT_STRUCT);

	CommitContext::ParamContext& pc = context.currentParamContext;
	size_t size = pc.second->GetSize();
	context.currentGroup->WriteRawData(context.renderContext,
			pc.second->AsRawData(pc.first), 0, size);
	pc.first += size;
}

AutoParamProcessor* Pass::customConstantProcessor =
		&CustomParameterProcessor::instance;
AutoParamProcessor* Pass::customTextureProcessor =
		&CustomTextureProcessor::instance;
AutoParamProcessor* Pass::customStructProcessor =
		&CustomStructProcessor::instance;
} /* namespace nextar */
