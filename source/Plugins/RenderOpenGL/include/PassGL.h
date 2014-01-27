
#ifndef PASSGL_H_
#define PASSGL_H_

#include "Pass.h"
#include "UniformBufferGL.h"

namespace RenderOpenGL {

	struct SamplerState {
		uint8 updateFrequency;
		// uint8 index; /* within shader */
		uint8 samplerCount;
		uint16 autoName;
		GLint location;
		union {
			GLuint sampler;
			GLuint* samplerArr;
		};

		String name;
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

	typedef vector<SamplerState>::type SamplerStateList;

	class PassGL : public Pass {
		NEX_LOG_HELPER(PassGL);

	public:
		/* Could be async accessed when using multi gpu setup, so have to make sure its thread safe */
		static std::pair<uint16, VertexSemanticListGL*> MapLayout(const VertexSemanticGL* semantics, uint32 numSemantics);

		inline const VertexSemanticListGL& GetInputSemantics() const {
			return *inputSemantics;
		}

	protected:
		virtual bool Compile(nextar::RenderContext*);
		virtual void Decompile(nextar::RenderContext*);

		virtual bool UpdateTextureStates(nextar::RenderContext*) = 0;
		virtual bool UpdateBlendStates(nextar::RenderContext*) = 0;
		virtual bool UpdateRasterStates(nextar::RenderContext*) = 0;
		virtual bool UpdateDepthStencilStates(nextar::RenderContext*) = 0;

		/* Material Property Table:
		 * [u1][u2]..[un][s1][s2]..[sm]
		 * Uniforms are mapped to material properties
		 * directly indexing, the material properties
		 * first contain the uniforms then the samplers
		 * in their taible. */
		UniformBufferList uniforms;
		/* The samplers are bound following the uniforms,
		 * the samplers might refer to default textures,
		 * which may be retrieved from the material via
		 * the name of the sampler in case the material
		 * does not contain a valid value.
		 * */
		SamplerStateList samplers;
		GLuint iGlProgram;
		VertexSemanticListGL* inputSemantics;
		
		typedef vector<VertexSemanticListGL>::type VertexSemanticListList;
		static VertexSemanticListList registeredSignatures;
	};

}

#endif //PASSGL_H_