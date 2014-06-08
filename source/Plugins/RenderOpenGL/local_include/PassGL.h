
#ifndef PASSGL_H_
#define PASSGL_H_

#include <Pass.h>
#include <UniformBufferGL.h>

namespace RenderOpenGL {

	struct SamplerState {
		TextureSamplerParamDesc desc;
		uint32 index;
		GLint location;
		GLuint sampler;
	};

	struct VertexSemanticGL {
		VertexSemantic semantic;
		GLuint index;

		friend bool operator ==(const VertexSemanticGL& v1,
				const VertexSemanticGL& v2) {
			// types may vary depending upon normalization is used
			return (v1 == v2 && v1.index == v2.index);
		}
	};

	/** Stored in passes, streams can only be bound
	 * to matching vertex signatures.
	 */
	typedef vector<VertexSemanticGL>::type VertexSemanticListGL;

	class PassGL : public Pass {
		NEX_LOG_HELPER(PassGL);

	public:

		PassGL();
		virtual ~PassGL();
		/* Could be async accessed when using multi gpu setup, so have to make sure its thread safe */
		static std::pair<uint16, VertexSemanticListGL*> MapLayout(const VertexSemanticGL* semantics, uint32 numSemantics);

		inline const VertexSemanticListGL& GetInputSemantics() const {
			return *inputSemantics;
		}

		inline GLuint GetProgram() {
			return iGlProgram;
		}
				
		virtual void SetTexture(RenderContext* rc, const SamplerParameter& desc, const TextureUnit* tu) = 0;

	protected:
		virtual bool Compile(nextar::RenderContext*);
		virtual void Decompile(nextar::RenderContext*);

		virtual bool UpdateTextureStates(nextar::RenderContext*) = 0;
		virtual bool UpdateBlendStates(nextar::RenderContext*) = 0;
		virtual bool UpdateRasterStates(nextar::RenderContext*) = 0;
		virtual bool UpdateDepthStencilStates(nextar::RenderContext*) = 0;

		GLuint iGlProgram;
		VertexSemanticListGL* inputSemantics;
		
		typedef vector<VertexSemanticListGL>::type VertexSemanticListList;
		static VertexSemanticListList registeredSignatures;

		friend class RenderContextGL;
	};

}

#endif //PASSGL_H_
