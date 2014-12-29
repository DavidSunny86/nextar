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

	virtual void Compile(nextar::RenderContext*, const Pass::CompileParams&);
	virtual void SetTexture(RenderContext* rc, const SamplerParameter& desc,
			const TextureUnit* tu);
	virtual void Destroy(nextar::RenderContext*);

protected:

	GLuint iGlProgram;

	uint16 inputLayoutId;
	VertexSemanticListGL* inputSemantics;

	typedef vector<VertexSemanticListGL>::type VertexSemanticListList;

	// todo make thread safe
	static VertexSemanticListList registeredSignatures;

	GpuProgramGL programs[Pass::STAGE_COUNT];
	static GLenum stagesMap[Pass::STAGE_COUNT];
	friend class RenderContextGL;
};

}

#endif //PASSGL_H_