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
#include <NamedObject.h>

namespace nextar {

	class _NexEngineAPI Pass :
		public ContextObject,
		public NamedObject,
		public AllocGraphics {
	public:

		enum {
			NUM_STAGES = (uint32)RenderConstants::MAX_PROGRAM_STAGES,
		};

		enum Flags {
			PROGRAM_DATA_INITED = 1 << 5,
		};

		enum Message {
			MSG_PASS_COMPILE,
			MSG_PASS_UPDATE_PARAMS,
		};

		// todo 7 or 8 is a good cap for this array, and
		// can make do with a static array in that case.
		typedef vector<TextureBase*>::type TextureList;

		struct SamplerDesc {
			TextureUnitParams texUnitParams;
			TextureBase* defaultTexture;
		};

		typedef map<String, SamplerDesc>::type TextureDescMap;
		struct CompileParams {

			const StringUtils::WordList* compileOptions;
			String programSources[Pass::NUM_STAGES];
			RasterState rasterState;
			BlendState blendState;
			DepthStencilState depthStencilState;
			Pass::TextureDescMap textureStates;
		};

		class View : public ContextObject::View {
		public:

			View();

			void UpdateParams(CommitContext& context, ParameterContext paramContext, uint32 updateId);
			// Bind texture to a shader parameter. The number of units must match the desc->numUnits count.
			virtual void SetTexture(RenderContext* rc, const SamplerParameter& desc, const TextureUnit* tu) = 0;
			
			virtual void Update(nextar::RenderContext*, uint32 msg, ContextParamPtr);
			virtual void Compile(nextar::RenderContext*, const CompileParams&) = 0;

		protected:

			ParameterGroupList sharedParameters;
			ParameterGroupEntries paramGroupEntries;
			SamplerParameter* samplers;
			uint32 numSamplerCount;

		};

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

		TextureBase* GetDefaultTexture(const String& name, uint32 index) const;
		const TextureUnitParams* GetTextureUnit(const String& name) const;

		// todo refer to the static table as well as a custom table
		// created from parsing the shader script
		const AutoParam* MapParam(const String& name);

		static const SamplerDesc* MapSamplerParams(const String& name, const TextureDescMap& texMap);

		// override RequestUpdate
		virtual void RequestUpdate(uint32 updateMsg, ContextObject::ContextParamPtr);
		// Set texture states, called during pass creation

	protected:


		/* Unique number representing this pass */
		uint16 inputLayoutUniqueID;
				
		uint16 flags;
		
		typedef array<GpuProgram*, NUM_STAGES>::type GpuProgramArray;
		GpuProgramArray programs;

		
		//static AutoParam autoParams[AutoParamName::AUTO_COUNT];
		static AutoParamProcessor* customConstantProcessor;
		static AutoParamProcessor* customTextureProcessor;
		static AutoParamProcessor* customStructProcessor;
	
		typedef map<StringRef, AutoParam*>::type AutoParamMap;
		static AutoParamMap autoParams;
		friend class ShaderAsset;
	};

	typedef Pass* PassPtr;
	typedef vector<PassPtr>::type PassList;

} /* namespace nextar */
#endif /* PASS_H_ */
