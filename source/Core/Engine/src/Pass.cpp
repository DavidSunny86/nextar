/*
 * Pass.cpp
 *
 *  Created on: 23-Nov-2013
 *      Author: obhi
 */

#include "Pass.h"

namespace nextar {

	Pass::Pass() {
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

	uint32 Pass::GetCustomParamCount() const {
		uint32 count = 0;
		for (auto& a : constantBuffers) {
			count += a->GetCustomParamCount();
		}
		return count + customSamplerCount;
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
		ctx.passParamIndex = paramIndex;
		for (auto& a : constantBuffers) {
			if ( (a->GetFrequency() & flags) ) {
				ctx.cbuffer = a;
				a->BeginWrite(rc);
				ShaderParamIterator it = a->GetParamIterator();
				_ProcessShaderParamIterator(rc, ctx, it, flags);
				a->EndWrite(rc);
			}
		}

		ctx.cbuffer = nullptr;
		ShaderParamIterator it(samplers, samplerStride, samplerCount);
		_ProcessShaderParamIterator(rc, ctx, it, flags);
		ctx.passParamIndex = -1;
	}

	/****************************************************/
	/* CustomTextureProcessor
	/****************************************************/
	class CustomTextureProcessor : public AutoParamProcessor {
	public:

		virtual void Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context);
	protected:
		~CustomTextureProcessor() {}
	};

	void CustomTextureProcessor::Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context) {
		NEX_ASSERT (d->type == AutoParamName::AUTO_CONSTANT);
		const TextureSamplerParamDesc* sampler = static_cast<const TextureSamplerParamDesc*>(d);
		context.pass->SetTextureImpl(rc, sampler, context.material->GetParameterAsTexture(context.passParamIndex++));
	}

	/****************************************************/
	/* CustomParameterProcessor
	/****************************************************/
	class CustomParameterProcessor : public AutoParamProcessor {
	public:

		virtual void Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context);
	protected:
		~CustomParameterProcessor() {}
	};

	void CustomParameterProcessor::Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context) {
		NEX_ASSERT (d->type == AutoParamName::AUTO_CONSTANT);
		const ConstBufferParamDesc* param = static_cast<const ConstBufferParamDesc*>(d);
		context.cbuffer->Write(rc, context.material->GetParameterData(context.passParamIndex++),
				param->cbOffset, param->size);
	}

} /* namespace nextar */
