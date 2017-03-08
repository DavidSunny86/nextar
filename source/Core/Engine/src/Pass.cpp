/*
 * Pass.cpp
 *
 *  Created on: 23-Nov-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <RenderSystem.h>
#include <Pass.h>
#include <CommitContext.h>
#include <ShaderParam.h>
#include <RenderContext.h>

namespace nextar {

Pass::Pass(StringID name, uint16 number) :
		NamedObject(name)
		,ContextObject(TYPE_PASS, 0)
		,passNumber(number)
		//,inputLayoutUniqueID(-1)
		,flags(0) {
}

Pass::Pass(Pass&& p) :
	NamedObject(p.GetID())
	,ContextObject(std::move(*static_cast<ContextObject*>(&p)))
	, passNumber(p.passNumber)
	,flags(0) {
}

Pass::Pass(const Pass& p) {
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	flags = 0;
}

Pass::~Pass() {
}


/****************************************************/
/* Pass::View
 /****************************************************/
Pass::View::View() :
		samplers(nullptr), numSamplerCount(0),
		viewNumber(0), frameNumber(0), passNumber(-1) {
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
	//if (passNumber != context.passNumber)
	{
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
		ParameterGroup* group = (*it).group;
		NEX_ASSERT(group->context == type);
		if (group->lastUpdateId == id) {
			ctx.paramContext.first += group->size;
			if (type == ParameterContext::CTX_OBJECT)
				group->lastUpdateId = id;
			continue;
		}

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
} /* namespace nextar */
