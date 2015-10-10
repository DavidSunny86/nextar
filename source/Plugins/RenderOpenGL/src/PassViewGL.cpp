#include <RenderOpenGL.h>
#include <PassViewGL.h>
#include <RenderContext_Base_GL.h>
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
	RenderContext_Base_GL* ctx = static_cast<RenderContext_Base_GL*>(rc);
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
			params.autoNames,
			produceReflectionData ? params.parameters : nullptr);
	// read samplers
	ctx->ReadSamplers(this, params.passIndex, iGlProgram,
			params.autoNames,
			produceReflectionData ? params.parameters : nullptr,
			params.textureStates);

	this->inputSemantics = layout.second;
	this->inputLayoutId = layout.first;

	SetupStates(params);
}

void PassViewGL::SetupStates(const Pass::CompileParams& params) {
	SetupRasterState(params.rasterState);
	SetupDepthStencilState(params.depthStencilState);
	SetupBlendState(params.blendState);
}

void PassViewGL::SetupBlendState(const BlendState& rs) {
	blendState.alphaToCoverage = rs.alphaToCoverage;
	blendState.enabled = rs.enabled;
	blendState.numRenderTargets = (uint8)rs.numRenderTargets;
	if (blendState.enabled && blendState.numRenderTargets) {
		blendState.sameBlendOpAllTarget = true;
		blendState.sameMaskAllTarget = true;
		RenderTargetBlendOp rt = rs.blendOp[0];
		for (uint32 i = 0; i < rs.numRenderTargets; ++i) {
			auto& src = rs.blendOp[i];
			if (rt.mask != src.mask)
				blendState.sameMaskAllTarget = false;
			auto& dest = blendState.blendOp[i];
			dest.mask = src.mask;
			if (rt.alphaOp != src.alphaOp ||
				rt.colOp != src.colOp ||
				rt.destAlpha != src.destAlpha ||
				rt.destCol != src.destCol ||
				rt.enabled != src.enabled ||
				rt.srcAlpha != src.srcAlpha ||
				rt.srcCol != src.srcCol)
				blendState.sameBlendOpAllTarget = false;

			dest.enabled = src.enabled;
			dest.alphaOp = RenderContext_Base_GL::GetGlBlendEquation(src.alphaOp);
			dest.colOp = RenderContext_Base_GL::GetGlBlendEquation(src.colOp);
			dest.destAlpha = RenderContext_Base_GL::GetGlBlendDataSource(src.destAlpha);
			dest.destCol = RenderContext_Base_GL::GetGlBlendDataSource(src.destCol);
			dest.srcAlpha = RenderContext_Base_GL::GetGlBlendDataSource(src.srcAlpha);
			dest.srcCol = RenderContext_Base_GL::GetGlBlendDataSource(src.srcCol);
		}
	}
}

void PassViewGL::SetupRasterState(const RasterState& rs) {
	rasterState.constantDepthBias = rs.constantDepthBias;
	rasterState.depthBiasClamp = rs.depthBiasClamp;
	rasterState.depthClip = rs.depthClip;
	rasterState.frontIsCCW = !rs.trianglesAreClockwise;
	rasterState.slopeScaledDepthBias = rs.slopeScaledDepthBias;
	rasterState.useScissors = rs.usingScissors;
	rasterState.usingMultisample = rs.usingMultisample;
	rasterState.usingLineAA = rs.usingLineAa;

	switch (rs.fill) {
	case FillMode::FM_POINT:
		rasterState.fillMode = GL_POINT; break;
	case FillMode::FM_SOLID:
		rasterState.fillMode = GL_FILL; break;
	case FillMode::FM_WIREFRAME:
		rasterState.fillMode = GL_LINE; break;
	}

	switch (rs.cull) {
	case CullMode::CULL_ALL:
		rasterState.cullMode = GL_FRONT_AND_BACK; break;
	case CullMode::CULL_BACK:
		rasterState.cullMode = GL_BACK;	break;
	case CullMode::CULL_FRONT:
		rasterState.cullMode = GL_FRONT; break;
	case CullMode::CULL_NONE:
		rasterState.cullMode = 0; break;
	}
}

void PassViewGL::SetupDepthStencilState(const DepthStencilState& desc) {
	depthStencilState.depthTest = desc.depthTest;
	depthStencilState.depthWrite = desc.depthWrite;
	depthStencilState.stencilTest = desc.stencilTest;

	depthStencilState.stencilFrontMask = desc.front.stencilMask;
	depthStencilState.stencilBackMask = desc.back.stencilMask;
	depthStencilState.stencilFrontRef = desc.front.stencilRef;
	depthStencilState.stencilBackRef = desc.back.stencilRef;

	depthStencilState.stencilFrontFunc = RenderContext_Base_GL::GetGlCompareFunc(desc.front.stencilFunc);
	depthStencilState.stencilBackFunc = RenderContext_Base_GL::GetGlCompareFunc(desc.back.stencilFunc);

	depthStencilState.frontStencilFail = RenderContext_Base_GL::GetGlStencilOp(desc.front.stencilFail);
	depthStencilState.frontStencilPass = RenderContext_Base_GL::GetGlStencilOp(desc.front.stencilPass);
	depthStencilState.frontDepthPass = RenderContext_Base_GL::GetGlStencilOp(desc.front.depthPass);

	depthStencilState.backStencilFail = RenderContext_Base_GL::GetGlStencilOp(desc.back.stencilFail);
	depthStencilState.backStencilPass = RenderContext_Base_GL::GetGlStencilOp(desc.back.stencilPass);
	depthStencilState.backDepthPass = RenderContext_Base_GL::GetGlStencilOp(desc.back.depthPass);

	depthStencilState.depthCompare = RenderContext_Base_GL::GetGlCompareFunc(desc.depthCompareFunc);
	depthStencilState.stencilSeparateOp = false;

	if (depthStencilState.stencilFrontFunc != depthStencilState.stencilBackFunc ||
		depthStencilState.frontStencilFail != depthStencilState.backStencilFail ||
		depthStencilState.frontStencilPass != depthStencilState.backStencilPass ||
		depthStencilState.frontDepthPass != depthStencilState.backDepthPass ||
		depthStencilState.stencilFrontMask != depthStencilState.stencilBackMask)
		depthStencilState.stencilSeparateOp = true;

}

void PassViewGL::SetTexture(RenderContext* rc, const SamplerParameter& desc,
		const TextureUnit* tu) {
	RenderContext_Base_GL* ctx = static_cast<RenderContext_Base_GL*>(rc);
	const SamplerState* samplerState = static_cast<const SamplerState*>(&desc);
	TextureViewGL* texture = static_cast<TextureViewGL*>(ctx->GetView(
			tu->texture));
	ctx->SetTexture(samplerState->index, samplerState->sampler, texture);
}

void PassViewGL::Destroy(RenderContext* _ctx) {
	for (nextar::uint32 i = 0; i < Pass::STAGE_COUNT; ++i) {
		programs[i].Destroy(_ctx);
	}

	RenderContext_Base_GL* ctx = static_cast<RenderContext_Base_GL*>(_ctx);
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
