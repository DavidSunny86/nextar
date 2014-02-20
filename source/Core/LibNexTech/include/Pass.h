/*
 * Pass.h
 *
 *  Created on: 23-Nov-2013
 *      Author: obhi
 */

#ifndef PASS_H_
#define PASS_H_

#include "DepthStencilState.h"
#include "BlendState.h"
#include "RasterState.h"
#include "TextureUnitState.h"
#include "GpuProgram.h"
#include "RenderConstants.h"
#include "ShaderParam.h"

namespace nextar {

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

		struct DefaultTextureUnit {
			TextureUnitParams params;
			TextureUnit defaultTexture;
		};

		// Will be used by the UI when displaying params
		class ParamIterator {
		public:
			virtual const String& GetName() const = 0;
			virtual ParamDataType GetType() const = 0;
			virtual size_t GetSize() const = 0;
			virtual ~ParamIterator() {}
		};
		/* shader programs */
		typedef map<String, DefaultTextureUnit>::type DefaultTextureUnitMap;

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

		virtual bool NotifyUpdated(nextar::RenderContext*);
		virtual void NotifyDestroyed(nextar::RenderContext*);

		
		const TextureAssetPtr& GetDefaultTexture(const String& name) const;
		const TextureUnitParams* GetTextureUnit(const String& name) const;

		void FinalizeUpdate();

		// todo
		virtual void UpdateParams(RenderContext* renderCtx, CommitContext& context, uint32 flags);
		// todo
		static size_t ParamSizeFromType(ParamDataType);
	protected:

		virtual bool Compile(nextar::RenderContext*) = 0;
		virtual void Decompile(nextar::RenderContext*) = 0;

		virtual bool UpdateTextureStates(nextar::RenderContext*) = 0;
		virtual bool UpdateBlendStates(nextar::RenderContext*) = 0;
		virtual bool UpdateRasterStates(nextar::RenderContext*) = 0;
		virtual bool UpdateDepthStencilStates(nextar::RenderContext*) = 0;
		
		void _CalculateParamBufferSize();
		/* Unique number representing this pass */
		uint16 inputLayoutUniqueID;

		/* Base index to the parameter buffer */
		uint16 baseParamIndex;
		uint16 flags;

		DefaultTextureUnitMap defaultTextureUnits;

		size_t shaderParamBufferSize;
				
		GpuProgram* programs[NUM_STAGES];

		RasterState rasterState;
		BlendState blendState;
		DepthStencilState depthStencilState;

		friend class ShaderAsset;
	};

} /* namespace nextar */
#endif /* PASS_H_ */
