
#include <RenderOpenGL.h>
#include <PassGL.h>
#include <RenderContextGL.h>

namespace RenderOpenGL {

	PassGL::PassGL() : inputSemantics(nullptr), iGlProgram(0) {
		samplerStride = sizeof(SamplerState);
	}

	PassGL::~PassGL() {
	}

	void PassGL::Decompile(RenderContext* _ctx) {
		RenderContextGL* ctx = static_cast<RenderContextGL*>(_ctx);
		ctx->DestroyProgram(iGlProgram);
	}

	bool PassGL::Compile(RenderContext* _ctx) {
		RenderContextGL* ctx = static_cast<RenderContextGL*>(_ctx);
		GLuint programStages[NUM_STAGES] = {0};
		for(nextar::uint32 i = 0; i < NUM_STAGES; ++i) {
			if(programs[i]) {
				GpuProgramGL* shader = static_cast<GpuProgramGL*>(programs[i]);
				if (shader) {
					programStages[i] = shader->GetShaderObject();
				}
			}
		}

		iGlProgram = ctx->CreateProgram(programStages);
		// read input/output semantics and lock before assignment
		VertexSemanticGL inputSemantics[VertexElement::MAX_VERTEX_ELEMENT];
		//todo We might need sorting the semantics for more coherency
		uint32 numSemantics;
		if(!(numSemantics = ctx->ReadProgramSemantics(iGlProgram, inputSemantics))) {
			Error("Failed to read semantics!");
			return false;
		}
		std::pair<uint16, VertexSemanticListGL*> layout = MapLayout(inputSemantics, numSemantics);
		// this->inputLoc.reserve(numSemantics);
		// this->inputLoc.assign(inputLoc, inputLoc + numSemantics);
		// read uniform data and register to uniform buffer manager
		ctx->ReadUniforms(this, iGlProgram);
		// read samplers
		ctx->ReadSamplers(this, iGlProgram);

		/* Feedback data */
		if (!IsProgramDataInited()) {
			this->inputSemantics = layout.second;
			this->inputLayoutUniqueID = layout.first;
			// create the feedback uniform table for materials
			/*for(uint32 i = 0; i < uniforms.size(); ++i) {
				UniformBufferGL& ubg = *uniforms[i];
				if(ubg.numUnmappedParams) {
					for (uint32 p = 0; p < ubg.uniforms.size(); ++p) {
						if(ubg.uniforms[p].autoName == (uint16)AutoParamName::AUTO_CUSTOM) {
							ShaderParamInfo spi;
							spi.name = ubg.uniforms[p].name;
							spi.type = ubg.uniforms[p].type;
							spi.count = ubg.uniforms[p].arrayCount;
							shaderParams.push_back(spi);
						}
					}
				}
			}
			// create the  sampler information
			for(uint32 s=0; s < samplers.size(); ++s) {
				if(samplers[s].autoName == (uint16)AutoParamName::AUTO_CUSTOM) {
					//ShaderParamInfo spi;
					spi.name = samplers[s].name;
					spi.count = samplers[s].samplerCount;
					spi.type = (uint16)ParamDataType::PDT_TEXTURE;
					// s is the index to which this sampler gets bound,
					// so textures bound to this index will be bound to this sampler
					spi.index = s;
					shaderParams.push_back(spi);
				}
			}*/

			SetProgramDataInited(true);
		}
		else {

			if (NEX_IS_DEBUG_MODE()) {
				// verify semantics match
				if(this->inputLayoutUniqueID != layout.first) {
					Error("Semantics mis-match!");
					return false;
				}
				// Can also match up shader parameters
				// todo
			}
		}

		return true;
	}
	
	std::pair<uint16, VertexSemanticListGL*> PassGL::MapLayout(const VertexSemanticGL* semantics, uint32 numSemantics) {
		uint16 index = 0;
		for (auto& it : registeredSignatures) {
			if (it.size() == numSemantics) {
				if (std::equal(it.begin(), it.end(), semantics))
					return std::pair<uint16, VertexSemanticListGL*>(index, &it);
			}
			index++;
		}

		registeredSignatures.push_back(VertexSemanticListGL(semantics, semantics+numSemantics));
		return std::pair<uint16, VertexSemanticListGL*>(index, &registeredSignatures.back());
	}
}
