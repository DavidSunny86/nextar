/**
 * Created on: 4th Oct 15
 * @author: obhi
 **/
#ifndef BASERenderContext_Base_GL_H_
#define BASERenderContext_Base_GL_H_

#include <BaseRenderContext.h>
#include <BaseExtensionsGL.h>
#include <FrameBufferObjectGL.h>
#include <PassViewGL.h>
#include <TextureViewGL.h>
#include <RenderBufferViewGL.h>
#include <GpuBufferViewGL.h>
#include <RenderTextureViewGL.h>

namespace RenderOpenGL {

/**
* @remarks Contains bare minimum/declared support for opengl
*/
class RenderContext_Base_GL :
	public nextar::BaseRenderContext,
	public CoreFuncTableVersion33 {
	typedef BaseExtensionsGL Extensions;
	typedef CoreFuncTableVersion33 FunctionTable;

	NEX_LOG_HELPER(RenderContext_Base_GL);

public:


	// Implementation
	class PlatformImpl : public AllocGeneral {
	protected:
		RenderContext_Base_GL* baseContext;
	public:
		PlatformImpl(RenderContext_Base_GL* pParent) : baseContext(pParent) {}
		virtual ~PlatformImpl() {}
		virtual void SetCreationParams(const RenderDriver::ContextCreationParams& ctxParams) = 0;
		virtual RenderWindow* CreateWindowImpl() = 0;
		virtual void ReadyContext(RenderWindow*) = 0;
		virtual void SetCurrentWindow(RenderTarget* rt) = 0;
		virtual void SetVideoModeImpl(const VideoMode& videoMode) = 0;
		virtual void PostCloseImpl() = 0;

		inline RenderContext_Base_GL* GetContext() {
			return baseContext;
		}
	};

	RenderContext_Base_GL(RenderDriverGL*);
	~RenderContext_Base_GL();

	virtual PlatformImpl* CreatePlatformImpl();
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
	virtual void PostCloseImpl();

	virtual ContextID RegisterObject(ContextObject::Type type, uint32 hint);
	virtual void UnregisterObject(ContextID);

	/* HL Functions */
	virtual void SwitchPass(CommitContext&, Pass::View*);
	virtual void SetVideoModeImpl(const VideoMode& videoMode);

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
	virtual void AllocateTexture(GLenum target, GLint levels, GLenum format,
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

	inline GLuint CreateVAO();
	inline void DestroyVAO(GLuint vao);
	inline void DestroyBuffer(GLuint vbo);
	inline void DestroyTexture(GLuint texture);
	inline void EnableVertexArrayObject(GLuint vao);
	inline void DisableVertexArrayObject();
	inline void SetVertexBuffer(GpuBufferViewGL* vb);
	inline void BindVertexBuffer(uint32 stream, uint32 offset, GpuBufferViewGL* vb);
	inline void VertexAttribBinding(GLuint, GLuint);
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

	inline GLenum ClientWaitSync(GLsync sync​, GLbitfield flags​, GLuint64 timeout​);
	inline void DeleteSync(GLsync sync);
	inline GLsync FenceSync(GLenum condition​, GLbitfield flags​);

	inline GLuint CreateRenderBuffer(
		const RenderBuffer::CreateParams*  createParams,
		const PixelFormatGl& format);
	inline void DestroyRenderBuffer(GLuint buffer);

	// capture
	void Capture(PixelBox& image, RenderTarget* rt, GLuint *pbo,
				 FrameBuffer frameBuffer);

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
	static void* GetExtension(const char* name);

	void ReportError(GLenum source,
					GLenum type,
					GLuint id,
					GLenum severity,
					GLsizei length,
					const GLchar* message);


	/* Interface */
	virtual VersionGL GetContextVersion() = 0;

	// Utility for Impl
	inline PlatformImpl* GetImpl();
	inline void SetVideoModes(VideoModeList&& m);
	inline void SetContextParams(const RenderDriver::ContextCreationParams& ccp);
	inline const RenderDriver::ContextCreationParams& GetContextParams() const;
	inline RenderDriver* GetDriver();
	inline void SetOriginalVideoMode(uint32 m);
	inline void BindWriteFBO(FrameBufferObjectGL& fbo);
	inline void UnbindWriteFBO();
	inline void BindFBO(FrameBufferObjectGL& fbo, bool readDraw);
	virtual void BindNamedFBO(FrameBufferObjectGL& fbo, bool readDraw, FrameBuffer fb);
	virtual void UnbindNamedFBO(bool readDraw);
	inline void UnbindFBO(bool readDraw);
	inline void CreateFBO(FrameBufferObjectGL& fbo);
	inline void CreateFBO(RenderTextureViewGL*);
	inline void CreateFBO(RenderBufferViewGL*);
	inline void DestroyFBO(FrameBufferObjectGL& fbo);
	void AttachToFBO(RenderTextureViewGL* rt, GLenum attachmentType);
	void AttachToFBO(RenderBufferViewGL* rt, GLenum attachmentType);
	bool ValidateFBO();
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

	inline void SetCreationParams(const RenderDriver::ContextCreationParams& ctxParams);
	inline RenderWindow* CreateWindowImpl();
	inline void ReadyContext(RenderWindow*);
	inline void SetCurrentWindow(RenderTarget* rt);

	virtual void DestroyResources();
	virtual void InitializeExtensions();

	uint32 currentCountOfColorAttachments;

	BaseExtensionsGL extensions;

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

	PlatformImpl* pImpl;
	static GLenum s_attachmentMap[(uint32)FrameBuffer::FBTYPE_COUNT];
};

void RenderContext_Base_GL::BindWriteFBO(FrameBufferObjectGL& fbo) {
	GlBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.frameBufferObject);
}

void RenderContext_Base_GL::UnbindWriteFBO() {
	GlBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void RenderContext_Base_GL::BindFBO(FrameBufferObjectGL& fbo, bool readDraw) {
	GlBindFramebuffer(readDraw ?
				  GL_READ_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER,
										fbo.frameBufferObject);
}

void RenderContext_Base_GL::UnbindFBO(bool readDraw) {
	GlBindFramebuffer(readDraw ?
				  GL_READ_FRAMEBUFFER : GL_DRAW_FRAMEBUFFER, 0);
}

void RenderContext_Base_GL::CreateFBO(FrameBufferObjectGL& fb) {
	if (!fb.frameBufferObject)
		GlGenFramebuffers(1, &fb.frameBufferObject);
}

void RenderContext_Base_GL::CreateFBO(RenderBufferViewGL* view) {
	CreateFBO(view->fbo);
	BindWriteFBO(view->fbo);
	AttachToFBO(view, view->attachmentType + 0);
	NEX_ASSERT(ValidateFBO());
	UnbindWriteFBO();
}

void RenderContext_Base_GL::CreateFBO(RenderTextureViewGL* view) {
	CreateFBO(view->GetFBO());
	BindWriteFBO(view->GetFBO());
	AttachToFBO(view, view->GetAttachment() + 0);
	NEX_ASSERT(ValidateFBO());
	UnbindWriteFBO();
}

inline void RenderContext_Base_GL::DestroyFBO(FrameBufferObjectGL& fbo) {
	GlDeleteFramebuffers(1, &fbo.frameBufferObject);
	GL_CHECK();
}

inline void RenderContext_Base_GL::EnableVertexArrayObject(GLuint vao) {
	GlBindVertexArray(vao);
	GL_CHECK();
}

inline void RenderContext_Base_GL::DisableVertexArrayObject() {
	GlBindVertexArray(0);
	GL_CHECK();
}

inline void RenderContext_Base_GL::SetVertexBuffer(GpuBufferViewGL* vbGL) {
	//	BufferManagerGL* manager =
	//			static_cast<BufferManagerGL*>(BufferManager::InstancePtr());
	//	manager->TouchVertexBuffer(vbGL);
	GlBindBuffer(GL_ARRAY_BUFFER, vbGL->GetBufferId());
	GL_CHECK();
}

inline void RenderContext_Base_GL::EnableVertexAttribute(const GLuint sem,
														 const VertexAttribGL& vegl) {
	if (vegl.integer)
		extensions.GlVertexAttribIFormat(sem, vegl.elementCount, vegl.dttype,
		(std::size_t) (vegl.element.desc.offset));
	else
		extensions.GlVertexAttribFormat(sem, vegl.elementCount, vegl.dttype,
		vegl.normalize,
		(std::size_t) (vegl.element.desc.offset));
	GL_CHECK();
	GlEnableVertexAttribArray(sem);
	GL_CHECK();
}

inline void RenderContext_Base_GL::EnableVertexAttribute(const GLuint sem,
														 uint32 stride,
														 const VertexAttribGL& vegl) {
	if (vegl.integer)
		GlVertexAttribIPointer(sem, vegl.elementCount, vegl.dttype,
		stride,
		(const GLvoid*)(std::size_t) (vegl.element.desc.offset));
	else
		GlVertexAttribPointer(sem, vegl.elementCount, vegl.dttype,
		vegl.normalize, stride,
		(const GLvoid*)(std::size_t) (vegl.element.desc.offset));
	GL_CHECK();
	GlEnableVertexAttribArray(sem);
	GL_CHECK();
}

inline void RenderContext_Base_GL::DisableVertexAttribute(const GLuint sem) {
	GlDisableVertexAttribArray(sem);
	GL_CHECK();
}

inline GLuint RenderContext_Base_GL::CreateVAO() {
	GLuint vao;
	GlGenVertexArrays(1, &vao);
	GL_CHECK();
	return vao;
}

inline void RenderContext_Base_GL::DestroyVAO(GLuint vao) {
	GlDeleteVertexArrays(1, &vao);
	GL_CHECK();
}

inline void RenderContext_Base_GL::DestroyBuffer(GLuint bufferId) {
	GlDeleteBuffers(1, &bufferId);
	GL_CHECK();
}

inline void RenderContext_Base_GL::ReadBuffer(GLenum target, void* dest,
											  size_t offset, size_t size) {
	void* src = GlMapBuffer(target, GL_READ_ONLY);
	GL_CHECK();
	std::memcpy(dest, (uint8*)(src)+offset, size);
	GlUnmapBuffer(target);
	GL_CHECK();
}

inline void RenderContext_Base_GL::WriteBuffer(GLenum target, size_t totalSize,
											   GLenum usage, const void* src, size_t offset, size_t size) {
	if (!size) {
		size = totalSize - offset;
		GlBufferData(target, totalSize, NULL, usage);
	}
	GL_CHECK();
	WriteSubData(target, src, offset, size);
}

inline void RenderContext_Base_GL::WriteSubData(GLenum target, const void* src,
												size_t offset, size_t size) {
	GlBufferSubData(target, offset, size, src);
	GL_CHECK();
}

inline void RenderContext_Base_GL::Bind(GLenum target, GLuint bufferId) {
	GlBindBuffer(target, bufferId);
	GL_CHECK();
}

inline void* RenderContext_Base_GL::Map(GLenum target, GLenum access) {
	void* ret = GlMapBuffer(target, access);
	GL_CHECK();
	return ret;
}

inline void* RenderContext_Base_GL::MapRange(GLenum target, GLintptr offset,
											 GLsizeiptr length, GLbitfield access) {
	void* ret = GlMapBufferRange(target, offset, length, access);
	GL_CHECK();
	return ret;
}

inline void RenderContext_Base_GL::Unmap(GLenum target) {
	GlUnmapBuffer(target);
	GlBindBuffer(target, 0);
}

inline void RenderContext_Base_GL::SetTexture(uint32 texIdx, GLuint samplerObject,
											  TextureViewGL* texture) {
	GlActiveTexture(GL_TEXTURE0 + texIdx);
	GL_CHECK();
	glBindTexture(texture->GetType(), texture->GetTexture());
	GL_CHECK();
	GlBindSampler(texIdx, samplerObject);
	GL_CHECK();
}

inline GLuint RenderContext_Base_GL::CreateTexture() {
	GLuint texture;
	glGenTextures(1, &texture);
	GL_CHECK();
	return texture;
}

inline void RenderContext_Base_GL::DestroyTexture(GLuint tex) {
	glDeleteTextures(1, &tex);
	GL_CHECK();
}

inline void RenderContext_Base_GL::ActivateTexture(GLenum target, GLuint texture) {
	GL_CHECK();
	glBindTexture(target, texture);
	GL_CHECK();
}

inline Size RenderContext_Base_GL::GetTextureParams(GLenum target) {
	GLint width = -1;
	GLint height = -1;
	glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, &width);
	GL_CHECK();
	glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &height);
	GL_CHECK();

	return Size((uint16)width, (uint16)height);
}

inline void RenderContext_Base_GL::GetTextureData(GLenum target, GLenum format, GLenum type, GLvoid* data) {
	glGetTexImage(target, 0, format, type, data);
	GL_CHECK();
}

inline void RenderContext_Base_GL::SetReadBuffer(GLenum b) {
	glReadBuffer(b);
	GL_CHECK();
}

inline void RenderContext_Base_GL::SetDrawBuffer(GLenum b) {
	glDrawBuffer(b);
	GL_CHECK();
}

inline GLenum RenderContext_Base_GL::ClientWaitSync(GLsync sync​, GLbitfield flags​,
												  GLuint64 timeout​) {
	GLenum result = GlClientWaitSync(sync​, flags​, timeout​);
	GL_CHECK();
	return result;
}

inline void RenderContext_Base_GL::DeleteSync(GLsync sync) {
	GlDeleteSync(sync);
	GL_CHECK();
}

inline GLsync RenderContext_Base_GL::FenceSync(GLenum condition​,
											   GLbitfield flags​) {
	GLsync sync = GlFenceSync(condition​, flags​);
	GL_CHECK();
	return sync;
}

inline GLuint RenderContext_Base_GL::CreateRenderBuffer(
	const RenderBuffer::CreateParams* createParams,
	const PixelFormatGl& format) {
	GLuint renderBuffer;
	GlGenRenderbuffers(1, &renderBuffer);
	GL_CHECK();
	GlBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	GL_CHECK();
	GlRenderbufferStorageMultisample(GL_RENDERBUFFER,
									 createParams->samples, format.internalFormat,
									 createParams->width, createParams->height);
	GL_CHECK();
	GlBindRenderbuffer(GL_RENDERBUFFER, 0);
	GL_CHECK();
	return renderBuffer;
}

inline void RenderContext_Base_GL::DestroyRenderBuffer(GLuint buffer) {
	GlDeleteRenderbuffers(1, &buffer);
	GL_CHECK();
}

inline void RenderContext_Base_GL::BindVertexBuffer(uint32 stream,
													uint32 offset,
													GpuBufferViewGL* buffer) {
	NEX_ASSERT(extensions.GlBindVertexBuffer);
	extensions.GlBindVertexBuffer(stream, buffer->GetBufferId(), offset,
								  buffer->GetStride());
	GL_CHECK();
}

inline void RenderContext_Base_GL::VertexAttribBinding(GLuint index, GLuint stream) {
	NEX_ASSERT(extensions.GlVertexAttribBinding);
	extensions.GlVertexAttribBinding(index, stream);
	GL_CHECK();
}

inline void RenderContext_Base_GL::SetCreationParams(const RenderDriver::ContextCreationParams& ctxParams) {
	pImpl->SetCreationParams(ctxParams);
}

inline RenderWindow* RenderContext_Base_GL::CreateWindowImpl() {
	return pImpl->CreateWindowImpl();
}

inline void RenderContext_Base_GL::ReadyContext(RenderWindow* wind) {
	pImpl->ReadyContext(wind);
}

inline void RenderContext_Base_GL::SetCurrentWindow(RenderTarget* rt) {
	pImpl->SetCurrentWindow(rt);
}

inline void RenderContext_Base_GL::SetVideoModes(VideoModeList&& m) {
	videoModes = std::move(m);
}

inline void RenderContext_Base_GL::SetContextParams(const RenderDriver::ContextCreationParams& ccp) {
	contextCreationParams = ccp;
}

inline const RenderDriver::ContextCreationParams& RenderContext_Base_GL::GetContextParams() const {
	return contextCreationParams;
}

inline RenderDriver* RenderContext_Base_GL::GetDriver() {
	return driver;
}

inline RenderContext_Base_GL::PlatformImpl* RenderContext_Base_GL::GetImpl() {
	return pImpl;
}

inline void RenderContext_Base_GL::SetOriginalVideoMode(uint32 m) {
	currentVideoMode = originalVideoMode = m;
}

}


#endif // BASERenderContext_Base_GL_H_
