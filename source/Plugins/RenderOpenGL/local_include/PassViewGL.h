#ifndef PASSGL_H_
#define PASSGL_H_

#include <Pass.h>
#include <UniformBufferGL.h>
#include <GpuProgramGL.h>
#include <VertexElementGL.h>

namespace RenderOpenGL {

class SamplerState: public SamplerParameter {
public:
	uint32 index;
	uint32 samplerRefCount;
	GLint location;
	GLuint sampler;
};

struct RasterStateGL {
	bool usingLineAA;
	bool usingMultisample;
	bool useScissors;
	bool depthClip;
	bool frontIsCCW;
	bool depthBiasEnabled;
	GLenum cullMode;
	GLenum fillMode;
	float slopeScaledDepthBias;
	float constantDepthBias;
	float depthBiasClamp;

	RasterStateGL() : frontIsCCW(true), cullMode(GL_BACK), fillMode(GL_FILL),
		depthClip(false), useScissors(false), usingMultisample(false),
		usingLineAA(false), slopeScaledDepthBias(0), constantDepthBias(0), depthBiasClamp(0),
		depthBiasEnabled(false) {}
};

struct DepthStencilStateGL {

	bool depthTest;
	bool depthWrite;
	bool stencilTest;
	bool stencilSeparateOp;

	GLenum depthCompare;

	GLuint stencilFrontMask;
	GLuint stencilBackMask;
	GLint stencilFrontRef;
	GLint stencilBackRef;

	GLenum stencilFrontFunc;
	GLenum stencilBackFunc;

	GLenum frontStencilFail;
	GLenum frontStencilPass;
	GLenum frontDepthPass;

	GLenum backStencilFail;
	GLenum backStencilPass;
	GLenum backDepthPass;

	DepthStencilStateGL() : depthTest(false), depthWrite(true), stencilTest(false), stencilSeparateOp(false),
		depthCompare(GL_LESS), stencilFrontMask((GLuint)-1), stencilBackMask((GLuint)-1),
		stencilFrontRef(0), stencilBackRef(0), 
		stencilFrontFunc(GL_ALWAYS), stencilBackFunc(GL_ALWAYS),
		frontStencilFail(GL_KEEP), frontStencilPass(GL_KEEP), frontDepthPass(GL_KEEP),
		backStencilFail(GL_KEEP), backStencilPass(GL_KEEP), backDepthPass(GL_KEEP)
	{}
};

struct RenderTargetBlendOpGL {
	/* default: false */
	bool enabled;
	ColorMask mask;
	GLenum srcCol;
	GLenum destCol;
	GLenum colOp;
	GLenum srcAlpha;
	GLenum destAlpha;
	GLenum alphaOp;

	RenderTargetBlendOpGL() :
		enabled(false), mask(ColorMask::MASK_ALL), srcCol(GL_ONE), destCol(GL_ZERO), colOp(GL_FUNC_ADD),
		srcAlpha(GL_ONE), destAlpha(GL_ZERO), alphaOp(GL_FUNC_ADD) {
	}
};

struct BlendStateGL {
	bool enabled;
	bool alphaToCoverage;
	bool sameBlendOpAllTarget;
	bool sameMaskAllTarget;
	uint8 numRenderTargets;
	RenderTargetBlendOpGL blendOp[(uint32)RenderConstants::MAX_RENDER_TARGETS];

	BlendStateGL() :
		enabled(false), alphaToCoverage(false), numRenderTargets(1), sameBlendOpAllTarget(true), sameMaskAllTarget(true){
	}
};

class PassViewGL: public Pass::View {
	NEX_LOG_HELPER(PassViewGL)
	;

public:

	PassViewGL();
	virtual ~PassViewGL();
	/* Could be async accessed when using multi gpu setup, so have to make sure its thread safe */
	static std::pair<uint16, VertexSemanticListGL*> MapLayout(
			const VertexSemanticGL* semantics, uint32 numSemantics);

	inline const VertexSemanticListGL& GetInputSemantics() const {
		return *inputSemantics;
	}

	inline GLuint GetProgram() {
		return iGlProgram;
	}

	inline uint16 GetInputLayoutID() const {
		return inputLayoutId;
	}

	inline const RasterStateGL& GetRasterState() const {
		return rasterState;
	}

	inline const DepthStencilStateGL& GetDepthStencilState() const {
		return depthStencilState;
	}

	inline const BlendStateGL& GetBlendState() const {
		return blendState;
	}

	virtual void Compile(nextar::RenderContext*, const Pass::CompileParams&);
	virtual void SetTexture(RenderContext* rc, const SamplerParameter& desc,
			const TextureUnit* tu);
	virtual void Destroy(nextar::RenderContext*);

	void SetupStates(const Pass::CompileParams&);
	void SetupRasterState(const RasterState& rs);
	void SetupDepthStencilState(const DepthStencilState& ds);
	void SetupBlendState(const BlendState& bs); 

protected:

	GLuint iGlProgram;

	uint16 inputLayoutId;
	VertexSemanticListGL* inputSemantics;

	DepthStencilStateGL depthStencilState;
	RasterStateGL rasterState;
	BlendStateGL blendState;

	typedef vector<VertexSemanticListGL>::type VertexSemanticListList;

	// todo make thread safe
	static VertexSemanticListList registeredSignatures;

	GpuProgramGL programs[Pass::STAGE_COUNT];
	static GLenum stagesMap[Pass::STAGE_COUNT];
	friend class RenderContextGL;
};

}

#endif //PASSGL_H_