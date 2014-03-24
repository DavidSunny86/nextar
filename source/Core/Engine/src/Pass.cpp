/*
 * Pass.cpp
 *
 *  Created on: 23-Nov-2013
 *      Author: obhi
 */

#include <Pass.h>

namespace nextar {

	Pass::Pass() : passParamByteOffset(-1)
	,materialParamByteOffset(-1)
	,objectParamByteOffset(-1)
	,inputLayoutUniqueID(-1)
	,flags(0)
	,programs({0})
	,samplers(nullptr)
	,samplerStride(0)
	,samplerCount(0)
	,lastFrameUpdate(-1)
	,lastViewUpdate(-1) {
		// TODO Auto-generated constructor stub

	}

	Pass::~Pass() {
		// TODO Auto-generated destructor stub
	}

	bool Pass::NotifyUpdated(RenderContext* rc) {
		bool useFallback = false;
		/** todo Change to bool inliners */
		if( (flags & DIRTY_RASTER_STATE) )
			useFallback &= UpdateRasterStates(rc);
		if( (flags & DIRTY_BLEND_STATE) && !useFallback)
			useFallback &= UpdateBlendStates(rc);
		if( (flags & DIRTY_DEPTHSTENCIL_STATE) && !useFallback)
			useFallback &= UpdateDepthStencilStates(rc);
		if( (flags & DIRTY_TEXUNIT_STATE) && !useFallback )
			useFallback &= UpdateTextureStates(rc);
		if( (flags & COMPILE_NEEDED) && !useFallback )
			useFallback &= Compile(rc);
		return useFallback;

	}

	void Pass::NotifyDestroyed(RenderContext* rc) {
		Decompile(rc);
		flags |= COMPILE_NEEDED;
	}

	void Pass::UpdateParams(RenderContext* rc, CommitContext& ctx, uint32 flags) {

		if (flags & UpdateFrequency::PER_FRAME) {
			if (lastFrameUpdate == ctx.frameNumber)
				flags &= ~UpdateFrequency::PER_FRAME;
			else
				lastFrameUpdate = ctx.frameNumber;
		}

		if (flags & UpdateFrequency::PER_VIEW) {
			if (lastViewUpdate == ctx.viewNumber)
				flags &= ~UpdateFrequency::PER_VIEW;
			else
				lastViewUpdate = ctx.viewNumber;
		}

		NEX_ASSERT(this == ctx.pass);
		ctx.passParamOffset = passParamByteOffset;
		ctx.materialParamOffset = materialParamByteOffset;
		ctx.objectParamOffset = objectParamByteOffset;
		for (auto& a : constantBuffers) {
			if ( (a->GetFrequency() & flags) ) {
				ctx.cbuffer = a;
				a->BeginUpdate(rc, flags);
				ShaderParamIterator it = a->GetParamIterator();
				_ProcessShaderParamIterator(rc, ctx, it, flags);
				a->EndUpdate(rc);
			}
		}

		ctx.cbuffer = nullptr;
		ShaderParamIterator it(samplers, samplerStride, samplerCount);
		_ProcessShaderParamIterator(rc, ctx, it, flags);
	}

	/****************************************************/
	/* MaterialTextureProcessor
	/****************************************************/
	class PassTextureProcessor : public AutoParamProcessor {
	public:
		static PassTextureProcessor processor;

		virtual void Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context);
	protected:
		~PassTextureProcessor() {}
	};

	void PassTextureProcessor::Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context) {
		NEX_ASSERT (d->type == AutoParamName::AUTO_CUSTOM_CONSTANT);
		const TextureSamplerParamDesc* sampler = static_cast<const TextureSamplerParamDesc*>(d);
		context.pass->SetTextureImpl(rc, sampler, context.passParameters->AsTexture(context.passParamOffset));
		context.passParamOffset += sampler->paramDesc.size;
	}

	/****************************************************/
	/* MaterialParameterProcessor
	/****************************************************/
	class PassParameterProcessor : public AutoParamProcessor {
	public:
		static PassParameterProcessor processor;

		virtual void Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context);
	protected:
		~PassParameterProcessor() {}
	};

	void PassParameterProcessor::Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context) {
		NEX_ASSERT (d->type == AutoParamName::AUTO_CUSTOM_CONSTANT);
		const ConstBufferParamDesc* param = static_cast<const ConstBufferParamDesc*>(d);
		context.cbuffer->Write(rc, context.passParameters->AsRawData(context.passParamOffset),
				param->cbOffset, param->paramDesc.size);
		context.passParamOffset += param->paramDesc.size;
	}

	/****************************************************/
	/* MaterialTextureProcessor
	/****************************************************/
	class MaterialTextureProcessor : public AutoParamProcessor {
	public:
		static MaterialTextureProcessor processor;

		virtual void Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context);
	protected:
		~MaterialTextureProcessor() {}
	};

	void MaterialTextureProcessor::Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context) {
		NEX_ASSERT (d->type == AutoParamName::AUTO_CUSTOM_CONSTANT);
		const TextureSamplerParamDesc* sampler = static_cast<const TextureSamplerParamDesc*>(d);
		context.pass->SetTextureImpl(rc, sampler, context.materialParameters->AsTexture(context.materialParamOffset));
		context.materialParamOffset += sampler->paramDesc.size;
	}

	/****************************************************/
	/* MaterialParameterProcessor
	/****************************************************/
	class MaterialParameterProcessor : public AutoParamProcessor {
	public:
		static MaterialParameterProcessor processor;

		virtual void Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context);
	protected:
		~MaterialParameterProcessor() {}
	};

	void MaterialParameterProcessor::Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context) {
		NEX_ASSERT (d->type == AutoParamName::AUTO_CUSTOM_CONSTANT);
		const ConstBufferParamDesc* param = static_cast<const ConstBufferParamDesc*>(d);
		context.cbuffer->Write(rc, context.materialParameters->AsRawData(context.materialParamOffset),
				param->cbOffset, param->paramDesc.size);
		context.materialParamOffset += param->paramDesc.size;
	}

	/****************************************************/
	/* ObjectTextureProcessor
	/****************************************************/
	class ObjectTextureProcessor : public AutoParamProcessor {
	public:
		static ObjectTextureProcessor processor;

		virtual void Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context);
	protected:
		~ObjectTextureProcessor() {}
	};

	void ObjectTextureProcessor::Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context) {
		NEX_ASSERT (d->type == AutoParamName::AUTO_CUSTOM_CONSTANT);
		const TextureSamplerParamDesc* sampler = static_cast<const TextureSamplerParamDesc*>(d);
		context.pass->SetTextureImpl(rc, sampler, context.objectParameters->AsTexture(context.objectParamOffset));
		context.objectParamOffset += sampler->paramDesc.size;
	}

	/****************************************************/
	/* ObjectParameterProcessor
	/****************************************************/
	class ObjectParameterProcessor : public AutoParamProcessor {
	public:
		static ObjectParameterProcessor processor;

		virtual void Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context);
	protected:
		~ObjectParameterProcessor() {}
	};

	void ObjectParameterProcessor::Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context) {
		NEX_ASSERT (d->type == AutoParamName::AUTO_CUSTOM_CONSTANT);
		const ConstBufferParamDesc* param = static_cast<const ConstBufferParamDesc*>(d);
		context.cbuffer->Write(rc, context.objectParameters->AsRawData(context.objectParamOffset),
				param->cbOffset, param->paramDesc.size);
		context.objectParamOffset += param->paramDesc.size;
	}

	PassTextureProcessor PassTextureProcessor::processor;
	PassParameterProcessor PassParameterProcessor::processor;
	MaterialTextureProcessor MaterialTextureProcessor::processor;
	MaterialParameterProcessor MaterialParameterProcessor::processor;
	ObjectTextureProcessor ObjectTextureProcessor::processor;
	ObjectParameterProcessor ObjectParameterProcessor::processor;

	AutoParamProcessor* Pass::customConstantProcessorMaterial = &MaterialParameterProcessor::processor;
	AutoParamProcessor* Pass::customTextureProcessorMaterial = &MaterialTextureProcessor::processor;
	AutoParamProcessor* Pass::customConstantProcessorPass = &PassParameterProcessor::processor;
	AutoParamProcessor* Pass::customTextureProcessorPass = &PassTextureProcessor::processor;
	AutoParamProcessor* Pass::customConstantProcessorObject = &ObjectParameterProcessor::processor;
	AutoParamProcessor* Pass::customTextureProcessorObject = &ObjectTextureProcessor::processor;

} /* namespace nextar */
