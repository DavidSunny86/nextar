
#ifndef RENDERCONTEXTGL_H_
#define RENDERCONTEXTGL_H_

#include <RenderOpenGL.h>
#include <ExtensionsGL.h>
#include <GpuProgramGL.h>
#include <UniformBufferGL.h>
#include <VertexElementGL.h>
#include <VertexBufferGL.h>
#include <IndexBufferGL.h>
#include <RenderTarget.h>
#include <PassViewGL.h>
#include <BufferManagerGL.h>

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
		virtual RenderWindow* CreateRenderWindowImpl();
		virtual void PostWindowCreation(RenderWindow* windowGl);
		virtual void PostWindowDestruction(RenderWindow* windowGl);
		virtual void Draw(StreamData*, CommitContext&);
		virtual void SetCurrentTarget(RenderTarget*);
		virtual void Clear(Color& c, float depth, uint16 stencil, ClearFlags flags);
		virtual void SetActivePass(Pass* pass);

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
		void ReadUniforms(PassViewGL*, uint32 passIndex, GLuint program, ParamEntryTable*);
		/* Read sampler information */
		void ReadSamplers(PassViewGL*, uint32 passIndex, GLuint program, ParamEntryTable*, const Pass::TextureDescMap& texMap);

		GLuint CreateVAO(VertexBufferBinding& binding, VertexAttribListGL& attribs, const VertexSemanticListGL& semantics);
		GLuint CreateVertexBuffer(size_t size, GLenum usage);
		GLuint CreateIndexBuffer(size_t size, GLenum usage);
		GLuint CreateTexture();
		void ActivateTexture(GLenum target, GLuint texture);
		void AllocateTexture(GLenum target, GLint levels, GLenum format, GLsizei width, GLsizei height, GLsizei depth);
		void AllocateTextureLevel(GLenum target, GLint level, );
		void AllocateTextureLevel(GLenum target, GLint level, const PixelFormatGl& format, GLsizei width, GLsizei height, GLsizei depth, const uint8* data, size_t size);
		void WriteTextureLevel(GLenum target, GLint level, const PixelFormatGl& format, GLsizei width, GLsizei height, GLsizei depth, const uint8* data, size_t size);

		void CopyBuffer(GLuint src, GLuint dest, size_t size);

		inline void DestroyVAO(GLuint vao);
		inline void DestroyVertexBuffer(GLuint vbo);
		inline void DestroyIndexBuffer(GLuint ibo);
		inline void DestroyTexture(GLuint texture);
		inline void EnableVertexArrayObject(GLuint vao);
		inline void DisableVertexArrayObject();
		inline void SetVertexBuffer(VertexBufferPtr& vb);
		inline void EnableVertexAttribute(const GLuint location, const VertexAttribGL& vegl);
		inline void DisableVertexAttribute(const GLuint location);
		inline void ReadBuffer(GLenum target, void* dest, size_t offset, size_t size);
		inline void WriteBuffer(GLenum target, size_t totalSize, GLenum usage,
			const void* src, size_t offset, size_t size);
		inline void WriteSubData(GLenum target, const void* src, size_t offset, size_t size);
		inline void Bind(GLenum target, GLuint bufferId);
		inline void* Map(GLenum target, GLenum access);
		inline void Unmap(GLenum target);
		inline void* MapRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
		inline void SetTexture(uint32 texIdx, GLuint samplerObject, TextureViewGL* tu);

		// capture
		void Capture(PixelBox& image, RenderTarget* rt, GLuint *pbo, FrameBuffer frameBuffer);


		static uint16 GetShaderParamType(GLuint type);
		static uint16 GetShaderParamSize(GLuint type);
		static uint16 GetSemanticType(GLenum);
		static bool IsSamplerType(GLint type);
		static GLint GetGlAddressMode(TextureAddressModeType t);
		static GLenum GetGlCompareFunc(TextureComparisonModeType type);
		static GLint GetGlMagFilter(TextureMinFilterType t);
		static GLint GetGlMinFilter(TextureMinFilterType t);
		static GLenum GetGlTextureType(TextureBase::TextureType type);
		// @optimize Return from a table a const reference rather than the object
		// and store that in texture view.
		static PixelFormatGl GetGlPixelFormat(PixelFormat imageFormat, PixelFormat textureFormat);
		static ParameterContext GuessContextByName(const String& name,
				ParameterContext defaultContext = ParameterContext::CTX_UNKNOWN);

	protected:

		UniformBufferGL* CreateUniformBuffer(
				PassViewGL* pass,
				uint32 passIndex,
				const String& name,
				GLint blockIndex,
				GLuint prog,
				GLuint numParams,
				size_t size,
				ParamEntryTable* paramTable);

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

		// Implementation
		virtual void SetCreationParams(const RenderDriver::ContextCreationParams& ctxParams) = 0;
		virtual RenderWindow* CreateWindowImpl() = 0;
		virtual void ReadyContext(RenderWindow*) = 0;
		virtual void SetCurrentWindow(RenderTarget* rt) = 0;

		uint32 contextFlags;
		//GpuObjectTable gpuObjects;
		/* uniform buffer table */
		typedef unordered_map<String, UniformBufferGL>::type UniformBufferMap;

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
				vegl.dttype, vegl.element.desc.stride,
				(const GLvoid*) (std::size_t) (vegl.element.desc.offset));
		else
			GlVertexAttribPointer(sem, vegl.elementCount,
				vegl.dttype, vegl.normalize, vegl.element.desc.stride,
				(const GLvoid*) (std::size_t) (vegl.element.desc.offset));
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

	inline void RenderContextGL::ReadBuffer(GLenum target, void* dest, size_t offset, size_t size) {
		void* src = GlMapBuffer(target, GL_READ_ONLY);
		GL_CHECK();
		std::memcpy(dest, (uint8*) (src) + offset, size);
		GlUnmapBuffer(target);
		GL_CHECK();
	}

	inline void RenderContextGL::WriteBuffer(GLenum target, size_t totalSize, GLenum usage,
		const void* src, size_t offset, size_t size) {
		if (!size) {
			size = totalSize - offset;
			GlBufferData(target, totalSize, NULL, usage);
		}
		GL_CHECK();
		WriteSubData(target, src, offset, size);
	}

	inline void RenderContextGL::WriteSubData(GLenum target, const void* src, size_t offset, size_t size) {
		GlBufferSubData(target, offset, size, src);
		GL_CHECK();
	}

	inline void RenderContextGL::Bind(GLenum target, GLuint bufferId) {
		GlBindBuffer(target, bufferId);
		GL_CHECK();
	}

	inline void* RenderContextGL::Map(GLenum target, GLenum access) {
		void* ret = GlMapBuffer(target, access);
		GL_CHECK();
		return ret;
	}

	inline void* RenderContextGL::MapRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access) {
		void* ret = GlMapBufferRange(target, offset, length, access);
		GL_CHECK();
		return ret;
	}

	inline void RenderContextGL::Unmap(GLenum target) {
		GlUnmapBuffer(target);
		GlBindBuffer(target, 0);
	}

	inline void RenderContextGL::SetTexture(
			uint32 texIdx, GLuint samplerObject, TextureViewGL* texture) {
		GlActiveTexture(GL_TEXTURE0 + texIdx);
		GL_CHECK();
		glBindTexture(texture->GetType(), texture->GetTexture());
		GL_CHECK();
		GlBindSampler(texIdx, samplerObject);
		GL_CHECK();
	}

	inline GLuint RenderContextGL::CreateTexture() {
		GLuint texture;
		glGenTextures(1, &texture);
		GL_CHECK();
		return texture;
	}

	inline void RenderContextGL::DestroyTexture(GLuint tex) {
		glDeleteTextures(1, &tex);
		GL_CHECK();
	}

	inline void RenderContextGL::ActivateTexture(GLenum target, GLuint texture) {
		glBindTexture(target, texture);
		GL_CHECK();
	}

}

#endif //RENDERCONTEXTGL_H_
