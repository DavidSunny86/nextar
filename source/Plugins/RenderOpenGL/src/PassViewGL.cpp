#include <RenderOpenGL.h>
#include <PassViewGL.h>
#include <RenderContextGL.h>
#include <TextureViewGL.h>

namespace RenderOpenGL {

GLenum PassViewGL::stagesMap[Pass::STAGE_COUNT] = {
	GL_VERTEX_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER,
	GL_GEOMETRY_SHADER,
	GL_FRAGMENT_SHADER,
};

PassViewGL::VertexSemanticListList PassViewGL::registeredSignatures;

PassViewGL::PassViewGL() :
		inputSemantics(nullptr), iGlProgram(0), inputLayoutId(-1) {
}

PassViewGL::~PassViewGL() {
	if (samplers) {
		SamplerState* samplerStates = static_cast<SamplerState*>(samplers);
		NEX_DELETE_ARRAY(samplerStates);
		samplers = nullptr;
	}
}

void PassViewGL::Compile(nextar::RenderContext* rc,
		const Pass::CompileParams& params) {
	RenderContextGL* ctx = static_cast<RenderContextGL*>(rc);
	GLuint programStages[Pass::STAGE_COUNT] = { 0 };
	for (nextar::uint32 i = 0; i < Pass::STAGE_COUNT; ++i) {
		if (params.programSources[i].length() > 1) {
			GpuProgramGL& shader = programs[i];
			if (!shader.Compile(stagesMap[i], rc,
					params.programSources[i].c_str(),
					params.compileOptions ?
							*params.compileOptions : StringUtils::Null)) {
				Error(
						"Program compilation failed due to failed shader compilation!");
				return;
			}
			programStages[i] = shader.GetShaderObject();
		}
	}

	iGlProgram = ctx->CreateProgram(programStages);
	if (!iGlProgram) {
		Error("Failed to compile program!");
		return;
	}
	// read input/output semantics and lock before assignment
	VertexSemanticGL inputSemantics[VertexElement::MAX_VERTEX_ELEMENT];
	//todo We might need sorting the semantics for more coherency
	uint32 numSemantics;
	if (!(numSemantics = ctx->ReadProgramSemantics(iGlProgram, inputSemantics))) {
		Error("Failed to read semantics!");
		return;
	}
	std::pair<uint16, VertexSemanticListGL*> layout = MapLayout(inputSemantics,
			numSemantics);

	long int preCompileStatus = 0;
	bool produceReflectionData = !params.inited.test_and_set(std::memory_order_relaxed);
	// read uniform data and register to uniform buffer manager
	ctx->ReadUniforms(this, params.passIndex, iGlProgram,
			produceReflectionData ? params.parameters : nullptr);
	// read samplers
	ctx->ReadSamplers(this, params.passIndex, iGlProgram,
			produceReflectionData ? params.parameters : nullptr,
			params.textureStates);

	this->inputSemantics = layout.second;
	this->inputLayoutId = layout.first;

}

void PassViewGL::SetTexture(RenderContext* rc, const SamplerParameter& desc,
		const TextureUnit* tu) {
	RenderContextGL* ctx = static_cast<RenderContextGL*>(rc);
	const SamplerState* samplerState = static_cast<const SamplerState*>(&desc);
	TextureViewGL* texture = static_cast<TextureViewGL*>(ctx->GetView(
			tu->texture));
	ctx->SetTexture(samplerState->index, samplerState->sampler, texture);
}

void PassViewGL::Destroy(RenderContext* _ctx) {
	for (nextar::uint32 i = 0; i < Pass::STAGE_COUNT; ++i) {
		programs[i].Destroy(_ctx);
	}

	RenderContextGL* ctx = static_cast<RenderContextGL*>(_ctx);
	ctx->DestroyProgram(iGlProgram);
}

std::pair<uint16, VertexSemanticListGL*> PassViewGL::MapLayout(
		const VertexSemanticGL* semantics, uint32 numSemantics) {
	// @urgent Make thread safe
	uint16 index = 0;
	for (auto& it : registeredSignatures) {
		if (it.size() == numSemantics) {
			if (std::equal(it.begin(), it.end(), semantics))
				return std::pair<uint16, VertexSemanticListGL*>(index, &it);
		}
		index++;
	}
	registeredSignatures.push_back(
			VertexSemanticListGL(semantics, semantics + numSemantics));
	return std::pair<uint16, VertexSemanticListGL*>(index,
			&registeredSignatures.back());
}

}
