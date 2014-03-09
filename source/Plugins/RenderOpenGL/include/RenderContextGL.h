
#ifndef RENDERCONTEXTGL_H_
#define RENDERCONTEXTGL_H_

#include <RenderOpenGL.h>
#include <ExtensionsGL.h>
#include <GpuProgramGL.h>
#include <UniformBufferGL.h>
#include <VertexElementGL.h>
#include <VertexBufferGL.h>
#include <IndexBufferGL.h>
#include <PassGL.h>

namespace RenderOpenGL {

	class RenderContextGL : public nextar::BaseRenderContext,
	public ExtensionsGL {
		NEX_LOG_HELPER(RenderContextGL);
	public:

		//typedef vector<GpuObject*>::type GpuObjectTable;

		RenderContextGL(RenderDriverGL*);
		virtual ~RenderContextGL();

		/* Implementation */
		virtual void CreateImpl(const RenderDriver::ContextCreationParams& ctxParams);
		virtual RenderWindowPtr CreateRenderWindowImpl();
		virtual void PostWindowCreation(RenderWindow* windowGl);
		virtual void PostWindowDestruction(RenderWindow* windowGl);

		/* HL Functions */

		/* GPU Object */
		/*inline GpuObject* GetObject(uint32 index) {
			NEX_ASSERT (index < gpuObjects.size());
			return	gpuObjects[index];
			//return 0;
		}*/

		/* GPU Object */
		/* Compile a single shader */
		GLuint CreateShader(GLenum shaderType,
				const char* preDefs, const char* source
				);
		void DestroyShader(GLuint);

		/* Compile and link to create a program */
		GLuint CreateProgram(GLuint shaders[]);
		void DestroyProgram(GLuint);

		/* Read porgram attribs etc, returns number of semantics read */
		uint32 ReadProgramSemantics(GLuint program,
				VertexSemanticGL* inputSemantics);
		/* Read uniform data */
		void ReadUniforms(UniformBufferList&, GLuint program);
		/* Read sampler information */
		void ReadSamplers(SamplerStateList& samplers, uint32& numUnmapped, const PassGL* shader, GLuint program);

		GLuint CreateVAO(VertexBufferBinding& binding, VertexAttribListGL& attribs, const VertexSemanticListGL& semantics);
		GLuint CreateVertexBuffer(size_t size, GLenum usage);
		GLuint CreateIndexBuffer(size_t size, GLenum usage);

		void CopyBuffer(GLuint src, GLuint dest, size_t size);

		inline void DestroyVAO(GLuint vao);
		inline void DestroyVertexBuffer(GLuint vbo);
		inline void DestroyIndexBuffer(GLuint ibo);
		inline void EnableVertexArrayObject(GLuint vao);
		inline void DisableVertexArrayObject();
		inline void SetVertexBuffer(VertexBufferPtr& vb);
		inline void EnableVertexAttribute(const GLuint location, const VertexAttribGL& vegl);
		inline void DisableVertexAttribute(const GLuint location);
		inline void ReadBuffer(GLenum target, GLuint bufferId, void* dest, size_t offset, size_t size);
		inline void WriteBuffer(GLenum target, GLuint bufferId, size_t totalSize, GLenum usage, 
			const void* src, size_t offset, size_t size);
		
				
		virtual void Draw(StreamData*, CommitContext&);
	protected:

		static uint16 GetShaderParamType(GLuint type);
		static uint16 GetShaderParamSize(GLuint type);
		static uint16 GetSemanticType(GLenum);
		static bool IsSamplerType(GLint type);
		static GLint GetGlAddressMode(TextureAddressModeType t);
		static GLenum GetGlCompareFunc(TextureComparisonModeType type);
		static GLint GetGlMagFilter(TextureMinFilterType t);
		static GLint GetGlMinFilter(TextureMinFilterType t);
		
		UniformBufferGL CreateUniformBuffer(GLint blockIndex, GLuint prog, GLuint numParams, size_t size);

		enum {
			CONTEXT_READY = 1 << 0,
			EXTENSIONS_READY = 1 << 1,
		};

		bool IsContextReady() {
			return contextFlags & CONTEXT_READY;
		}

		bool AreExtensionsReady() {
			return contextFlags & EXTENSIONS_READY;
		}

		virtual void SetCreationParams(const RenderDriver::ContextCreationParams& ctxParams) = 0;
		virtual RenderWindow* CreateWindowImpl() = 0;
		virtual void ReadyContext(RenderWindow*) = 0;

		uint32 contextFlags;
		//GpuObjectTable gpuObjects;
		/* uniform buffer table */
		typedef unordered_map<StringRef, UniformBufferGL>::type UniformBufferMap;

		UniformBufferMap uniformBufferMap;
		RenderDriver::ContextCreationParams contextCreationParams;
	};

	inline void RenderContextGL::EnableVertexArrayObject(GLuint vao) {
		GlBindVertexArray(vao);
		GL_CHECK();
	}

	inline void RenderContextGL::DisableVertexArrayObject() {
		GlBindVertexArray(0);
		GL_CHECK();
	}

	inline void RenderContextGL::SetVertexBuffer(VertexBufferPtr& vb) {
		VertexBufferGL* vbGL = static_cast<VertexBufferGL*>(vb.GetPtr());
		BufferManagerGL* manager = static_cast<BufferManagerGL*>(BufferManager::InstancePtr());
		manager->TouchVertexBuffer(vbGL);
		GlBindBuffer(GL_ARRAY_BUFFER,
			vbGL->GetBufferId());
		GL_CHECK();
	}

	inline void RenderContextGL::EnableVertexAttribute(const GLuint sem, const VertexAttribGL& vegl) {
		if (vegl.integer)
			GlVertexAttribIPointer(sem, vegl.elementCount,
				vegl.dttype, vegl.element.desc.stride, (const GLvoid*) (vegl.element.desc.offset));
		else
			GlVertexAttribPointer(sem, vegl.elementCount,
				vegl.dttype, vegl.normalize, vegl.element.desc.stride, (const GLvoid*) (vegl.element.desc.offset));
		GL_CHECK();
		GlEnableVertexAttribArray(sem);
		GL_CHECK();
	}

	inline void RenderContextGL::DisableVertexAttribute(const GLuint sem) {
		GlDisableVertexAttribArray(sem);
		GL_CHECK();
	}

	inline void RenderContextGL::DestroyVAO(GLuint vao) {
		GlDeleteVertexArrays(1, &vao);
	}

	inline void RenderContextGL::DestroyVertexBuffer(GLuint bufferId) {
		GlDeleteBuffers(1, &bufferId);
	}

	inline void RenderContextGL::DestroyIndexBuffer(GLuint bufferId) {
		GlDeleteBuffers(1, &bufferId);
	}

	inline void RenderContextGL::ReadBuffer(GLenum target, GLuint bufferId, void* dest, size_t offset, size_t size) {
		GlBindBuffer(target, bufferId);
		GL_CHECK();
		void* src = GlMapBuffer(target, GL_READ_ONLY);
		GL_CHECK();
		std::memcpy(dest, (uint8*) (src) + offset, size);
		GlUnmapBuffer(target);
		GL_CHECK();
	}

	inline void RenderContextGL::WriteBuffer(GLenum target, GLuint bufferId, size_t totalSize, GLenum usage, 
		const void* src, size_t offset, size_t size) {
		GlBindBuffer(target, bufferId);
		if (!size) {
			size = totalSize - offset;
			GlBufferData(target, totalSize, NULL, usage);
		}
		GL_CHECK();
		GlBufferSubData(target, offset, size, src);
		GL_CHECK();
	}

}

#endif //RENDERCONTEXTGL_H_
