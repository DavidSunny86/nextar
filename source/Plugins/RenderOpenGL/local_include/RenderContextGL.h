#ifndef RENDERCONTEXTGL_H_
#define RENDERCONTEXTGL_H_

#include <RenderOpenGL.h>
#include <ExtensionsGL.h>
#include <GpuProgramGL.h>
#include <UniformBufferGL.h>
#include <VertexElementGL.h>
#include <RenderTarget.h>
#include <PassViewGL.h>
#include <GpuBufferViewGL.h>
#include <TextureViewGL.h>

namespace RenderOpenGL {

class GpuBufferViewGL;
class RenderContextGL: public nextar::BaseRenderContext, public ExtensionsGL {
	NEX_LOG_HELPER(RenderContextGL)
	;
	// Constants to fetch
	// @GL_MAX_COLOR_ATTACHMENTS

public:

	//typedef vector<GpuObject*>::type GpuObjectTable;

	RenderContextGL(RenderDriverGL*);
	virtual ~RenderContextGL();

	/* Implementation */
	virtual void CreateImpl(
			const RenderDriver::ContextCreationParams& ctxParams);
	virtual RenderWindow* CreateRenderWindowImpl();
	virtual void PostWindowCreation(RenderWindow* windowGl);
	virtual void PostWindowDestruction(RenderWindow* windowGl);
	virtual void Draw(StreamData*, CommitContext&);
	virtual void SetCurrentTarget(RenderTarget*);
	virtual void Clear(const ClearBufferInfo&);
	virtual void CloseImpl();
	virtual void Copy(RenderTarget* src, FrameBuffer srcFb, RenderTarget* dest, FrameBuffer destFb);
	virtual void EndRender();

	virtual ContextID RegisterObject(ContextObject::Type type, uint32 hint);
	virtual void UnregisterObject(ContextID);

	/* HL Functions */
	virtual void SwitchPass(CommitContext&, Pass::View*);

	/* GPU Object */
	/*inline GpuObject* GetObject(uint32 index) {
	 NEX_ASSERT (index < gpuObjects.size());
	 return	gpuObjects[index];
	 //return 0;
	 }*/

	/* GPU Object */
	/* Compile a single shader */
	GLuint CreateShader(GLenum shaderType, const char* preDefs,
			const char* source);
	void DestroyShader(GLuint);

	/* Compile and link to create a program */
	GLuint CreateProgram(GLuint shaders[]);
	void DestroyProgram(GLuint);

	/* Read porgram attribs etc, returns number of semantics read */
	uint32 ReadProgramSemantics(GLuint program,
			VertexSemanticGL* inputSemantics);
	/* Read uniform data */
	void ReadUniforms(PassViewGL*, uint32 passIndex, GLuint program,
			const Pass::VarToAutoParamMap& remapParams,
			ParamEntryTable*);
	/* Read sampler information */
	void ReadSamplers(PassViewGL*, uint32 passIndex, GLuint program,
		const Pass::VarToAutoParamMap& remapParams,
			ParamEntryTable*, const Pass::TextureDescMap& texMap);

	GLuint CreateBuffer(size_t size, GLenum usage, GLenum type);
	GLuint CreateTexture();
	void ActivateTexture(GLenum target, GLuint texture);
	void AllocateTexture(GLenum target, GLint levels, GLenum format,
			GLsizei width, GLsizei height, GLsizei depth);
	void AllocateTextureLevel(GLenum target, GLint level);
	void AllocateTextureLevel(GLenum target, GLint level,
			const PixelFormatGl& format, GLsizei width, GLsizei height,
			GLsizei depth, const uint8* data, size_t size);
	void WriteTextureLevel(GLenum target, GLint level,
			const PixelFormatGl& format, GLsizei width, GLsizei height,
			GLsizei depth, const uint8* data, size_t size);
	void SpecifyTargetState(bool readOrDraw, bool setUnset, RenderTarget* target, FrameBuffer index);

	void CopyBuffer(GLuint src, GLuint dest, size_t size);

	void SetRasterState(const RasterStateGL&);
	void SetDepthStencilState(const DepthStencilStateGL&);
	void SetBlendState(const BlendStateGL&);

	inline void DestroyVAO(GLuint vao);
	inline void DestroyBuffer(GLuint vbo);
	inline void DestroyTexture(GLuint texture);
	inline void EnableVertexArrayObject(GLuint vao);
	inline void DisableVertexArrayObject();
	inline void SetVertexBuffer(GpuBufferViewGL* vb);
	inline void EnableVertexAttribute(const GLuint location,
			uint32 stride,
			const VertexAttribGL& vegl);
	inline void EnableVertexAttribute(const GLuint location,
				const VertexAttribGL& vegl);
	inline void DisableVertexAttribute(const GLuint location);
	inline void ReadBuffer(GLenum target, void* dest, size_t offset,
			size_t size);
	inline void WriteBuffer(GLenum target, size_t totalSize, GLenum usage,
			const void* src, size_t offset, size_t size);
	inline void WriteSubData(GLenum target, const void* src, size_t offset,
			size_t size);
	inline void Bind(GLenum target, GLuint bufferId);
	inline void* Map(GLenum target, GLenum access);
	inline void Unmap(GLenum target);
	inline void* MapRange(GLenum target, GLintptr offset, GLsizeiptr length,
			GLbitfield access);
	inline void SetTexture(uint32 texIdx, GLuint samplerObject,
			TextureViewGL* tu);
	inline Size GetTextureParams(GLenum target);
	inline void GetTextureData(GLenum target, GLenum format, GLenum type, GLvoid* data);

	inline void SetReadBuffer(GLenum b);
	inline void SetDrawBuffer(GLenum b);

	// capture
	void Capture(PixelBox& image, RenderTarget* rt, GLuint *pbo,
			FrameBuffer frameBuffer);
		
	void ReportError(GLenum source,
					GLenum type,
					GLuint id,
					GLenum severity,
					GLsizei length,
					const GLchar* message);

	static ParamDataType GetShaderParamType(GLuint type);
	static uint16 GetShaderParamSize(GLuint type);
	static VertexComponentType GetSemanticType(GLenum);
	static bool IsSamplerType(GLint type);
	static GLint GetGlAddressMode(TextureAddressMode t);
	static GLenum GetGlCompareFunc(TextureComparisonMode type);
	static GLenum GetGlCompareFunc(DepthStencilCompare c);
	static GLenum GetGlStencilOp(StencilOp s);
	static GLint GetGlMagFilter(TextureMagFilter t);
	static GLint GetGlMinFilter(TextureMinFilter t);
	static GLenum GetGlTextureType(TextureBase::TextureType type);
	static GLenum GetGlBlendDataSource(BlendDataSource type);
	static GLenum GetGlBlendEquation(BlendOp op);
	// @optimize Return from a table a const reference rather than the object
	// and store that in texture view.
	static PixelFormatGl GetGlPixelFormat(PixelFormat imageFormat,
			PixelFormat textureFormat);
	static ParameterContext GuessContextByName(const String& name,
			ParameterContext defaultContext = ParameterContext::CTX_UNKNOWN);
	static void DestroyResources(void* pThis);

protected:


	void DetermineShaderTarget();
	GLuint CreateSamplerFromParams(const TextureUnitParams& params);

	UniformBufferGL* CreateUniformBuffer(PassViewGL* pass, uint32 passIndex,
			const String& name, GLint blockIndex, GLuint prog, GLuint numParams,
			uint32 size, const Pass::VarToAutoParamMap& remapParams,
			ParamEntryTable* paramTable);

	enum {
		CONTEXT_READY = 1 << 0,
		EXTENSIONS_READY = 1 << 1,
		CURRENT_TARGET_FBO = 1 << 2,
	};

	bool IsContextReady() {
		return contextFlags & CONTEXT_READY;
	}

	bool AreExtensionsReady() {
		return contextFlags & EXTENSIONS_READY;
	}

	// Implementation
	virtual void SetCreationParams(
			const RenderDriver::ContextCreationParams& ctxParams) = 0;
	virtual RenderWindow* CreateWindowImpl() = 0;
	virtual void ReadyContext(RenderWindow*) = 0;
	virtual void SetCurrentWindow(RenderTarget* rt) = 0;

	uint32 currentCountOfColorAttachments;

	uint32 contextFlags;
	String shaderTarget;
	//GpuObjectTable gpuObjects;
	/* uniform buffer table */
	typedef unordered_map<String, UniformBufferGL>::type UniformBufferMap;

	UniformBufferMap uniformBufferMap;
	RenderDriver::ContextCreationParams contextCreationParams;
	RenderTarget* currentWindow;

	BlendStateGL blendState;
	DepthStencilStateGL depthStencilState;
	RasterStateGL rasterState;
};

inline void RenderContextGL::EnableVertexArrayObject(GLuint vao) {
	GlBindVertexArray(vao);
	GL_CHECK();
}

inline void RenderContextGL::DisableVertexArrayObject() {
	GlBindVertexArray(0);
	GL_CHECK();
}

inline void RenderContextGL::SetVertexBuffer(GpuBufferViewGL* vbGL) {
//	BufferManagerGL* manager =
//			static_cast<BufferManagerGL*>(BufferManager::InstancePtr());
//	manager->TouchVertexBuffer(vbGL);
	GlBindBuffer(GL_ARRAY_BUFFER, vbGL->GetBufferId());
	GL_CHECK();
}

inline void RenderContextGL::EnableVertexAttribute(const GLuint sem,
		const VertexAttribGL& vegl) {
	if (vegl.integer)
		GlVertexAttribIFormat(sem, vegl.elementCount, vegl.dttype,
				(std::size_t) (vegl.element.desc.offset));
	else
		GlVertexAttribFormat(sem, vegl.elementCount, vegl.dttype,
				vegl.normalize,
				(std::size_t) (vegl.element.desc.offset));
	GL_CHECK();
	GlEnableVertexAttribArray(sem);
	GL_CHECK();
}

inline void RenderContextGL::EnableVertexAttribute(const GLuint sem,
		uint32 stride,
		const VertexAttribGL& vegl) {
	if (vegl.integer)
		GlVertexAttribIPointer(sem, vegl.elementCount, vegl.dttype,
				stride,
				(const GLvoid*) (std::size_t) (vegl.element.desc.offset));
	else
		GlVertexAttribPointer(sem, vegl.elementCount, vegl.dttype,
				vegl.normalize, stride,
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

inline void RenderContextGL::DestroyBuffer(GLuint bufferId) {
	GlDeleteBuffers(1, &bufferId);
}

inline void RenderContextGL::ReadBuffer(GLenum target, void* dest,
		size_t offset, size_t size) {
	void* src = GlMapBuffer(target, GL_READ_ONLY);
	GL_CHECK();
	std::memcpy(dest, (uint8*) (src) + offset, size);
	GlUnmapBuffer(target);
	GL_CHECK();
}

inline void RenderContextGL::WriteBuffer(GLenum target, size_t totalSize,
		GLenum usage, const void* src, size_t offset, size_t size) {
	if (!size) {
		size = totalSize - offset;
		GlBufferData(target, totalSize, NULL, usage);
	}
	GL_CHECK();
	WriteSubData(target, src, offset, size);
}

inline void RenderContextGL::WriteSubData(GLenum target, const void* src,
		size_t offset, size_t size) {
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

inline void* RenderContextGL::MapRange(GLenum target, GLintptr offset,
		GLsizeiptr length, GLbitfield access) {
	void* ret = GlMapBufferRange(target, offset, length, access);
	GL_CHECK();
	return ret;
}

inline void RenderContextGL::Unmap(GLenum target) {
	GlUnmapBuffer(target);
	GlBindBuffer(target, 0);
}

inline void RenderContextGL::SetTexture(uint32 texIdx, GLuint samplerObject,
		TextureViewGL* texture) {
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
	GL_CHECK();
	glBindTexture(target, texture);
	GL_CHECK();
}

inline Size RenderContextGL::GetTextureParams(GLenum target) {
	GLint width = -1;
	GLint height = -1;
	glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, &width);
	GL_CHECK();
	glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &height);
	GL_CHECK();

	return Size((uint16)width, (uint16)height);
}

inline void RenderContextGL::GetTextureData(GLenum target, GLenum format, GLenum type, GLvoid* data) {
	glGetTexImage(target, 0, format, type, data);
	GL_CHECK();
}

inline void RenderContextGL::SetReadBuffer(GLenum b) {
	glReadBuffer(b);
	GL_CHECK();
}

inline void RenderContextGL::SetDrawBuffer(GLenum b) {
	glDrawBuffer(b);
	GL_CHECK();
}

}


#endif //RENDERCONTEXTGL_H_
