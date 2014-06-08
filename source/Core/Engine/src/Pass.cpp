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
	

	Pass::Pass(StringID name) :
	ContextObject(TYPE_PASS)
	,NamedObject(name)
	,inputLayoutUniqueID(-1)
	,flags(0)
	,programs() // todo is this enough?? or do null out
	{
		// null all programs
		for(uint32 i = 0; i < Pass::NUM_STAGES; ++i) {
			programs[i] = nullptr;
		}
	}

	Pass::~Pass() {
		for(uint32 i = 0; i < Pass::NUM_STAGES; ++i) {
			if (programs[i]) {
				NEX_DELETE(programs[i]);
				programs[i] = nullptr;
			}
		}
	}

	void Pass::RequestUpdate(uint32 msg, ContextObject::ContextParamPtr param) {
		if (msg == Pass::MSG_PASS_COMPILE) {
			const CompileParams& p = *reinterpret_cast<const CompileParams*>(param);
			//blendState = p.blendState;
			//depthStencilState = p.depthStencilState;
			//rasterState = p.rasterState;
			//textureDescMap.swap(p.textureStates);

			for(uint32 i = 0; i < Pass::NUM_STAGES; ++i) {
				GpuProgram::ProgramType t = (GpuProgram::ProgramType)(GpuProgram::TYPE_VERTEX + i);
				const String& source = p.programSources[i];
				if (programs[i])
					NEX_DELETE(programs[i]);
				programs[i] = nullptr;
				if (source.length()) {
					programs[i] = GpuProgram::Instance(t);
					GpuProgram::CompileParam update = {
							source.c_str(),
							p.compileOptions
					};
					programs[i]->RequestUpdate(GpuProgram::MSG_COMPILE,
							reinterpret_cast<ContextObject::ContextParamPtr>(&update));
				}
			}
		} else if (msg == Pass::MSG_PASS_UPDATE_PARAMS) {

		}
		ContextObject::RequestUpdate(msg, reinterpret_cast<ContextObject::ContextParamPtr>(param));
	}


	const AutoParam* Pass::MapParam(const String& name) {
		auto ap = autoParams.find(name);
		if (ap == autoParams.end())
			return nullptr;
		// todo map parsed params
		return (*ap).second;
	}

	const Pass::SamplerDesc* Pass::MapSamplerParams(const String& name, const TextureDescMap& texMap) {
		auto sp = texMap.find(name);
		if (sp == texMap.end())
			return nullptr;
		// todo map parsed params
		return &(*sp).second;
	}

	/****************************************************/
	/* Pass::View
	/****************************************************/
	Pass::View::View() : samplers(nullptr)
	,numSamplerCount(0) {
	}

	void Pass::View::Update(RenderContext* rc, uint32 msg, ContextParamPtr param) {
		if (msg == Pass::MSG_PASS_COMPILE) {
			const CompileParams& p = *reinterpret_cast<const CompileParams*>(param);
			Compile(rc, p);
		}
	}
	
	// todo Optimize.
	void Pass::View::UpdateParams(CommitContext& ctx, ParameterContext type, uint32 id) {
		NEX_ASSERT(this == ctx.pass);
		
		ParameterGroupItem& item = paramGroupEntries[(uint32)type];
		ctx.currentParamContext = CommitContext::ParamContext(item.offsetInParamBuffer, 
			ctx.paramBuffers[(uint32)type]);

		for(auto it = item.beginIt; it != item.endIt; ++it) {
			ParameterGroup* group = (*it);
			NEX_ASSERT(group->context == type);
			if (group->lastUpdateId == id)
				continue;
			ctx.currentGroup = group;
			if (group->processor) {
				group->processor->Apply(ctx, group);
			} else {
				group->Map(ctx.context);
				for(uint32 i = 0; i < group->numParams; ++i) {
					ConstantParameter* parameter = group->GetParamByIndex(i);
					NEX_ASSERT (parameter->processor);
					parameter->processor->Apply(ctx, parameter);
				}
				group->Unmap(ctx.context);
			}
			(*it)->lastUpdateId = id;
		}

		ctx.currentGroup = nullptr;
		for(auto it = item.beginSamplerIt; it != item.endSamplerIt; 
			it = SamplerParameter::Next(it)) {
			NEX_ASSERT(it->processor);
			it->processor->Apply(ctx, it);
		}
	}

	/****************************************************/
	/* CustomTextureProcessor
	/****************************************************/
	class CustomTextureProcessor : public AutoParamProcessor {
	public:
		static CustomTextureProcessor instance;

		virtual void Apply(CommitContext& context, const ShaderParameter* param);
	protected:
		~CustomTextureProcessor() {}
	};

	void CustomTextureProcessor::Apply(CommitContext& context, const ShaderParameter* param) {
		NEX_ASSERT (param->autoName == AutoParamName::AUTO_CUSTOM_CONSTANT);
		const SamplerParameter* sampler = reinterpret_cast<const SamplerParameter*>(param);
		CommitContext::ParamContext& pc = context.currentParamContext;
		context.pass->SetTexture(context.context, *sampler, pc.second->AsTexture(pc.first));
		pc.first += sampler->size;
	}

	/****************************************************/
	/* CustomParameterProcessor
	/****************************************************/
	class CustomParameterProcessor : public AutoParamProcessor {
	public:
		static CustomParameterProcessor instance;

		virtual void Apply(CommitContext& context, const ShaderParameter* param);
	protected:
		~CustomParameterProcessor() {}
	};

	void CustomParameterProcessor::Apply(CommitContext& context, const ShaderParameter* param) {
		NEX_ASSERT (param->autoName == AutoParamName::AUTO_CUSTOM_CONSTANT);
		const ConstantParameter* constParam = reinterpret_cast<const ConstantParameter*>(param);
		CommitContext::ParamContext& pc = context.currentParamContext;
		context.currentGroup->SetRawBuffer(context.context, *constParam, pc.second->AsRawData(pc.first));
		pc.first += param->size;
	}

	/****************************************************/
	/* CustomStructProcessor
	/****************************************************/
	class CustomStructProcessor : public AutoParamProcessor {
	public:
		static CustomStructProcessor instance;

		virtual void Apply(CommitContext& context, const ShaderParameter* param);
	protected:
		~CustomStructProcessor() {}
		const uint32 paramContext;
	};

	void CustomStructProcessor::Apply(CommitContext& context, const ShaderParameter* param) {
		NEX_ASSERT (param->type == ParamDataType::PDT_STRUCT);
		
		CommitContext::ParamContext& pc = context.currentParamContext;
		size_t size = pc.second->GetSize();
		context.currentGroup->WriteRawData(context.context, 
			pc.second->AsRawData(pc.first), 0, size);
		pc.first += size;
	}

	AutoParamProcessor* Pass::customConstantProcessor = &CustomParameterProcessor::instance;
	AutoParamProcessor* Pass::customTextureProcessor = &CustomTextureProcessor::instance;
	AutoParamProcessor* Pass::customStructProcessor = &CustomStructProcessor::instance;
} /* namespace nextar */
