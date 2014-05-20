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
#include <ConstantBuffer.h>
#include <NamedObject.h>

namespace nextar {

	class Pass :
			public NamedObject,
			public AllocGraphics {
	public:

		enum {
			NUM_STAGES = (uint32)RenderConstants::MAX_PROGRAM_STAGES,
			MAX_CBUFFER = (uint32)RenderConstants::MAX_CBUFFER_PER_PASS,
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
		// todo 7 or 8 is a good cap for this array, and
		// can make do with a static array in that case.
		typedef vector<TextureBase*>::type TextureList;

		struct SamplerDesc {
			TextureUnitParams texUnitParams;
			TextureBase* defaultTexture;
		};

		typedef map<String, SamplerDesc>::type TextureDescMap;
		typedef array<ConstantBufferPtr, (uint32)MAX_CBUFFER>::type ConstantBufferList;

		Pass(StringID name);
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

		inline ShaderParamIterator GetSamplerIterator() {
			return ShaderParamIterator(&samplers->paramDesc, samplerStride, samplerCount);
		}

		inline ConstantBufferList& GetConstantBuffers() {
			return sharedParameters;
		}

		virtual bool NotifyUpdated(nextar::RenderContext*);
		virtual void NotifyDestroyed(nextar::RenderContext*);
		
		TextureBase* GetDefaultTexture(const String& name, uint32 index) const;
		const TextureUnitParams* GetTextureUnit(const String& name) const;

		// todo refer to the static table as well as a custom table
		// created from parsing the shader script
		const AutoParam* MapParam(const String& name);
		const SamplerDesc* MapSamplerParams(const String& name);

		// todo
		virtual void UpdateParams(RenderContext* renderCtx, CommitContext& context, UpdateFrequency flags);
		// todo
		static size_t ParamSizeFromType(ParamDataType);


		// Set texture states, called during pass creation

		// Bind texture to a shader parameter. The number of units must match the desc->numUnits count.
		virtual void SetTextureImpl(RenderContext* rc, const TextureSamplerParamDesc* desc, const TextureUnit* tu) = 0;

	protected:

		static inline void _ProcessShaderParamIterator(RenderContext* rc, CommitContext& ctx, ShaderParamIterator& it, UpdateFrequency flags) {
			while(it) {
				const ShaderParamDesc& d = (*it);
				if (Test(d.frequency & flags)) {
					NEX_ASSERT(d.processor);
					d.processor->Apply(rc, &d, ctx);
				}
				++it;
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
				
		uint16 flags;
		
		typedef array<GpuProgram*, NUM_STAGES>::type GpuProgramArray;
		GpuProgramArray programs;

		RasterState rasterState;
		BlendState blendState;
		DepthStencilState depthStencilState;
		TextureDescMap textureDescMap;

		// updated per frame
		ConstantBufferList sharedParameters;
		uint32 numConstBuffers;

		TextureSamplerParamDesc* samplers;
		uint32 samplerCount;

		uint32 passParamByteOffset;
		uint32 materialParamByteOffset;
		uint32 objectParamByteOffset;

		uint32 lastFrameUpdate;
		uint32 lastViewUpdate;
		//static AutoParam autoParams[AutoParamName::AUTO_COUNT];
		static AutoParamProcessor* customConstantProcessorMaterial;
		static AutoParamProcessor* customTextureProcessorMaterial;
		static AutoParamProcessor* customConstantProcessorPass;
		static AutoParamProcessor* customTextureProcessorPass;
		static AutoParamProcessor* customConstantProcessorObject;
		static AutoParamProcessor* customTextureProcessorObject;
		static AutoParamProcessor* customStructProcessorMaterial;
		static AutoParamProcessor* customStructProcessorPass;
		static AutoParamProcessor* customStructProcessorObject;

		static uint32 samplerStride;

		typedef map<StringRef, AutoParam*>::type AutoParamMap;

		static AutoParamMap autoParams;

		friend class ShaderAsset;
	};

	typedef Pass* PassPtr;
	typedef vector<PassPtr>::type PassList;

} /* namespace nextar */
#endif /* PASS_H_ */
