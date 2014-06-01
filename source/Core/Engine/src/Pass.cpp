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
#include <ConstantBuffer.h>

namespace nextar {

	uint32 Pass::samplerStride(0);
	Pass::AutoParamMap Pass::autoParams;
	

	Pass::Pass(StringID name) :
	NamedObject(name)
	,inputLayoutUniqueID(-1)
	,flags(0)
	,programs() // todo is this enough?? or do null out
	,samplers(nullptr)
	,samplerCount(0)
	,numConstBuffers(0) {
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
			CompileParams& p = *reinterpret_cast<CompileParams*>(param);
			//blendState = p.blendState;
			//depthStencilState = p.depthStencilState;
			//rasterState = p.rasterState;
			//textureDescMap.swap(p.textureStates);

			for(uint32 i = 0; i < Pass::NUM_STAGES; ++i) {
				GpuProgram::Type t = (GpuProgram::Type)i;
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
	Pass::View::View() :
	passParamByteOffset(0)
	,materialParamByteOffset(0)
	,objectParamByteOffset(0)
	,lastFrameUpdate(-1)
	,lastViewUpdate(-1) {
	}

	void Pass::View::Update(RenderContext* rc, uint32 msg, ContextParamPtr param) {
		if (msg == Pass::MSG_PASS_COMPILE) {
			CompileParams& p = *reinterpret_cast<CompileParams*>(param);
			Compile(rc, p);
		} else if (msg == Pass::MSG_PASS_UPDATE_OFFSET) {
			OffsetParams& p = *reinterpret_cast<OffsetParams*>(param);
			passParamByteOffset = p.passParamByteOffset;
			materialParamByteOffset = p.materialParamByteOffset;
			objectParamByteOffset = p.objectParamByteOffset;
		}
	}

	// todo This function is currently not implemented correctly.
	void Pass::View::UpdateParams(RenderContext* rc, CommitContext& ctx, UpdateFrequency flags) {
		NEX_ASSERT(this == ctx.pass);
		ctx.shaderParamContext[CommitContext::PASS_PARAM_CONTEXT].first = passParamByteOffset;
		ctx.shaderParamContext[CommitContext::MATERIAL_PARAM_CONTEXT].first = materialParamByteOffset;
		ctx.shaderParamContext[CommitContext::OBJECT_PARAM_CONTEXT].first = objectParamByteOffset;

		if (Test(flags & UpdateFrequency::PER_FRAME)) {
			if (lastFrameUpdate == ctx.frameNumber)
				flags &= ~UpdateFrequency::PER_FRAME;
			else
				lastFrameUpdate = ctx.frameNumber;
		}

		if (Test(flags & UpdateFrequency::PER_VIEW)) {
			if (lastViewUpdate == ctx.viewNumber)
				flags &= ~UpdateFrequency::PER_VIEW;
			else
				lastViewUpdate = ctx.viewNumber;
		}

		for(uint32 i = 0; i < numConstBuffers; ++i) {
			ConstantBufferPtr& cb = sharedParameters[i];
			ConstantBuffer::View* cbView = static_cast<ConstantBuffer::View*>(
					rc->GetView(cb));
			if (Test(cb->GetFrequency() & flags)) {
				ctx.cbuffer = cbView;
				ctx.cbufferSize = cb->GetSize();
				AutoParamProcessor* proc = cb->GetProcessor();
				if (proc) {
					// we just update it in one go
					proc->Apply(rc, nullptr, ctx);
				} else {
					cbView->BeginUpdate(rc, flags);
					ShaderParamIterator it = cb->GetParamIterator();
					_ProcessShaderParamIterator(rc, ctx, it, flags);
					cbView->EndUpdate(rc);
				}
			}
		}

		ctx.cbuffer = nullptr;
		ShaderParamIterator it(&samplers->paramDesc, samplerStride, samplerCount);
		_ProcessShaderParamIterator(rc, ctx, it, flags);
	}

	/****************************************************/
	/* CustomTextureProcessor
	/****************************************************/
	class CustomTextureProcessor : public AutoParamProcessor {
	public:
		static CustomTextureProcessor passProcessor;
		static CustomTextureProcessor materialProcessor;
		static CustomTextureProcessor objectProcessor;

		virtual void Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context);
	protected:
		CustomTextureProcessor(uint32 context) : paramContext(context) {}
		~CustomTextureProcessor() {}
		const uint32 paramContext;
	};

	void CustomTextureProcessor::Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context) {
		NEX_ASSERT (d->autoName == AutoParamName::AUTO_CUSTOM_CONSTANT);
		const TextureSamplerParamDesc* sampler = reinterpret_cast<const TextureSamplerParamDesc*>(d);
		CommitContext::ParamContext& pc = context.shaderParamContext[paramContext];
		context.pass->SetTextureImpl(rc, sampler, pc.second->AsTexture(pc.first));
		pc.first += sampler->paramDesc.size;
	}

	/****************************************************/
	/* CustomParameterProcessor
	/****************************************************/
	class CustomParameterProcessor : public AutoParamProcessor {
	public:
		static CustomParameterProcessor passProcessor;
		static CustomParameterProcessor materialProcessor;
		static CustomParameterProcessor objectProcessor;

		virtual void Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context);
	protected:
		CustomParameterProcessor(uint32 context) : paramContext(context) {}
		~CustomParameterProcessor() {}
		const uint32 paramContext;
	};

	void CustomParameterProcessor::Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context) {
		NEX_ASSERT (d->autoName == AutoParamName::AUTO_CUSTOM_CONSTANT);
		const ConstBufferParamDesc* param = reinterpret_cast<const ConstBufferParamDesc*>(d);
		CommitContext::ParamContext& pc = context.shaderParamContext[paramContext];
		context.cbuffer->Write(rc, pc.second->AsRawData(pc.first),
				param->cbOffset, param->paramDesc.size);
		pc.first += param->paramDesc.size;
	}

	/****************************************************/
	/* CustomStructProcessor
	/****************************************************/
	class CustomStructProcessor : public AutoParamProcessor {
	public:
		static CustomStructProcessor passProcessor;
		static CustomStructProcessor materialProcessor;
		static CustomStructProcessor objectProcessor;

		virtual void Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context);
	protected:
		CustomStructProcessor(uint32 custom) : paramContext(custom) {}
		~CustomStructProcessor() {}
		const uint32 paramContext;
	};

	void CustomStructProcessor::Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context) {
		NEX_ASSERT (d->autoName == AutoParamName::AUTO_CUSTOM_CONSTANT);
		size_t s = context.cbufferSize;
		CommitContext::ParamContext& pc = context.shaderParamContext[paramContext];
		context.cbuffer->Write(rc, pc.second->AsRawData(pc.first), 0, s);
		pc.first += (uint32)s;
	}
		
	CustomTextureProcessor CustomTextureProcessor::passProcessor(CommitContext::PASS_PARAM_CONTEXT);
	CustomTextureProcessor CustomTextureProcessor::materialProcessor(CommitContext::MATERIAL_PARAM_CONTEXT);
	CustomTextureProcessor CustomTextureProcessor::objectProcessor(CommitContext::OBJECT_PARAM_CONTEXT);

	CustomParameterProcessor CustomParameterProcessor::passProcessor(CommitContext::PASS_PARAM_CONTEXT);
	CustomParameterProcessor CustomParameterProcessor::materialProcessor(CommitContext::MATERIAL_PARAM_CONTEXT);
	CustomParameterProcessor CustomParameterProcessor::objectProcessor(CommitContext::OBJECT_PARAM_CONTEXT);

	CustomStructProcessor CustomStructProcessor::passProcessor(CommitContext::PASS_PARAM_CONTEXT);
	CustomStructProcessor CustomStructProcessor::materialProcessor(CommitContext::MATERIAL_PARAM_CONTEXT);
	CustomStructProcessor CustomStructProcessor::objectProcessor(CommitContext::OBJECT_PARAM_CONTEXT);

	AutoParamProcessor* Pass::customConstantProcessorMaterial = &CustomParameterProcessor::materialProcessor;
	AutoParamProcessor* Pass::customConstantProcessorPass = &CustomParameterProcessor::passProcessor;
	AutoParamProcessor* Pass::customConstantProcessorObject = &CustomParameterProcessor::objectProcessor;
	
	AutoParamProcessor* Pass::customTextureProcessorMaterial = &CustomTextureProcessor::materialProcessor;
	AutoParamProcessor* Pass::customTextureProcessorPass = &CustomTextureProcessor::passProcessor;
	AutoParamProcessor* Pass::customTextureProcessorObject = &CustomTextureProcessor::objectProcessor;

	AutoParamProcessor* Pass::customStructProcessorMaterial = &CustomStructProcessor::materialProcessor;
	AutoParamProcessor* Pass::customStructProcessorPass = &CustomStructProcessor::passProcessor;
	AutoParamProcessor* Pass::customStructProcessorObject = &CustomStructProcessor::objectProcessor;
} /* namespace nextar */
