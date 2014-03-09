/*
 * Pass.h
 *
 *  Created on: 23-Nov-2013
 *      Author: obhi
 */

#ifndef PASS_H_
#define PASS_H_

#include <DepthStencilState.h>
#include <BlendState.h>
#include <RasterState.h>
#include <TextureUnitState.h>
#include <GpuProgram.h>
#include <RenderConstants.h>
#include <ShaderParam.h>

namespace nextar {

	class AutoParamProcessor {
	public:
		virtual void Apply(RenderContext* rc, const ShaderParamDesc* d, CommitContext& context) = 0;
	protected:
		~AutoParamProcessor() {}
	};

	struct AutoParam {
		uint32 type;
		uint32 autoName;
		uint32 frequency;
		AutoParamProcessor* processor;
		// todo May not be useful except in UI
		StringRef name;
		String desc;
	};

	class Pass : public AllocGraphics {
	public:
		enum {
			NUM_STAGES = (uint32)RenderConstants::MAX_PROGRAM_STAGES
		};

		enum Flags {
			DIRTY_RASTER_STATE = 1 << 0,
			DIRTY_BLEND_STATE = 1 << 1,
			DIRTY_DEPTHSTENCIL_STATE = 1 << 2,
			DIRTY_TEXUNIT_STATE = 1 << 3,
			COMPILE_NEEDED = 1 << 4,
			PROGRAM_DATA_INITED = 1 << 5,
			DIRTY_FLAG = (DIRTY_RASTER_STATE|DIRTY_BLEND_STATE|DIRTY_DEPTHSTENCIL_STATE|
					DIRTY_TEXUNIT_STATE|COMPILE_NEEDED)
		};

		Pass();
		virtual ~Pass();
		
		inline bool IsProgramDataInited() const {
			return flags & PROGRAM_DATA_INITED;
		}

		inline void SetProgramDataInited(bool v) {
			flags = v ? flags | PROGRAM_DATA_INITED : flags & ~PROGRAM_DATA_INITED;
		}
				
		inline uint16 GetInputLayoutID() const {
			return inputLayoutUniqueID;
		}

		inline size_t GetParamBufferSize() const {
			return shaderParamBufferSize;
		}

		inline ShaderParamIterator GetSamplerIterator() {
			return ShaderParamIterator(samplers, samplerStride, samplerCount);
		}

		inline void SetParamIndex(uint32 index) {
			paramIndex = index;
		}

		virtual bool NotifyUpdated(nextar::RenderContext*);
		virtual void NotifyDestroyed(nextar::RenderContext*);

		
		// const TextureAssetPtr& GetDefaultTexture(const String& name) const;
		// const TextureUnitParams* GetTextureUnit(const String& name) const;
		uint32 GetCustomParamCount() const;

		// todo
		virtual void UpdateParams(RenderContext* renderCtx, CommitContext& context, uint32 flags);
		// todo
		static size_t ParamSizeFromType(ParamDataType);
		// todo
		static const AutoParam* MapParam(const String& name);

		static AutoParamProcessor* GetCustomTextureProcessor() {
			return customTextureProcessor;
		}

		static AutoParamProcessor* GetCustomConstantProcessor() {
			return customConstantProcessor;
		}


		// Bind texture to a shader parameter. The number of units must match the desc->numUnits count.
		virtual void SetTextureImpl(RenderContext* rc, const TextureSamplerParamDesc* desc, const TextureUnit* tu) = 0;

	protected:

		inline void _ProcessShaderParamIterator(RenderContext* rc, CommitContext& ctx, ShaderParamIterator& it, uint32 flags) {
			while(it.Next()) {
				const ShaderParamDesc& d = it.Get();
				if (d.frequency & flags) {
					NEX_ASSERT(d.processor);
					d.processor->Apply(rc, &d, ctx);
				}
			}
		}

		virtual bool Compile(nextar::RenderContext*) = 0;
		virtual void Decompile(nextar::RenderContext*) = 0;

		virtual bool UpdateTextureStates(nextar::RenderContext*) = 0;
		virtual bool UpdateBlendStates(nextar::RenderContext*) = 0;
		virtual bool UpdateRasterStates(nextar::RenderContext*) = 0;
		virtual bool UpdateDepthStencilStates(nextar::RenderContext*) = 0;
		
		/* Unique number representing this pass */
		uint16 inputLayoutUniqueID;

		/* Base index to the parameter buffer */
		uint16 baseParamIndex;
		uint16 flags;

		size_t shaderParamBufferSize;
				
		GpuProgram* programs[NUM_STAGES];

		RasterState rasterState;
		BlendState blendState;
		DepthStencilState depthStencilState;

		typedef vector<ConstantBufferPtr>::type ConstantBufferList;
		ConstantBufferList constantBuffers;
		// updated per frame
		//ConstantBufferPtr frameParameters;
		// updated per view
		//ConstantBufferPtr viewParameters;
		// regularly updated
		//ConstantBufferPtr objectParameters;

		TextureSamplerParamDesc* samplers;
		uint32 samplerStride;
		uint32 samplerCount;
		uint32 customSamplerCount;

		uint32 paramIndex;

		uint32 lastFrameUpdate;
		uint32 lastViewUpdate;
		//static AutoParam autoParams[AutoParamName::AUTO_COUNT];
		static AutoParamProcessor* customConstantProcessor;
		static AutoParamProcessor* customTextureProcessor;
		friend class ShaderAsset;
	};

} /* namespace nextar */
#endif /* PASS_H_ */
