/**
 * Created on: 4th Oct 15
 * @author: obhi
 **/
#include <BaseRenderContext.h>
#include <RenderContext_Base_GL.h>
#include <VertexElementGL.h>
#include <PassViewGL.h>
#include <UniformBufferGL.h>
#include <GpuBufferViewGL.h>
#include <VertexLayoutGL.h>
#include <MultiRenderTargetViewGL.h>
#include <RenderDriverGL.h>

namespace RenderOpenGL {

GLenum RenderContext_Base_GL::s_attachmentMap[] = {
	GL_NONE,
	GL_FRONT_LEFT,
	GL_FRONT_RIGHT,
	GL_BACK_LEFT,
	GL_BACK_RIGHT,
	GL_DEPTH,
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4,
	GL_COLOR_ATTACHMENT5,
	GL_COLOR_ATTACHMENT6,
	GL_COLOR_ATTACHMENT7,
	GL_STENCIL_ATTACHMENT,
};

RenderContext_Base_GL::RenderContext_Base_GL(RenderDriverGL* _driver) :
BaseRenderContext(_driver)
, currentWindow(0)
, currentBoundUniformBuffer(0)
, currentCountOfColorAttachments(0)
, contextFlags(0) {
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_DESTROY_RESOURCES, DestroyResources, this);
	pImpl = CreatePlatformImpl();
}

RenderContext_Base_GL::~RenderContext_Base_GL() {
	NEX_DELETE(GetImpl());
}

void RenderContext_Base_GL::CreateImpl(
	const RenderDriver::ContextCreationParams& ctxParams) {
	SetCreationParams(ctxParams);
	auto it = ctxParams.extraParams.find("BufferBlockSize");
	uint32 blockSize = 1024 * 1024;
	if (it != ctxParams.extraParams.end())
		blockSize = Convert::ToULong((*it).second);

	for (uint32 i = 0; i < 3; ++i)
		_poolIndexBuffer[i].SetBlockSize(blockSize);
	for (uint32 i = 0; i < 3; ++i)
		_poolVertexBuffer[i].SetBlockSize(blockSize);
}

void RenderContext_Base_GL::PostWindowCreation(RenderWindow* gw) {
	/* check if extensions were initialized or initialize extensions */
	if (!IsContextReady()) {
		ReadyContext(gw);
		contextFlags |= CONTEXT_READY;

		if (!AreExtensionsReady()) {
			InitializeExtensions();
			DetermineShaderTarget();
			DetermineConstants();
			contextFlags |= EXTENSIONS_READY;
			SetCurrentWindow(static_cast<RenderWindowImpl*>(gw->GetImpl()));
			currentWindow = static_cast<RenderWindowImpl*>(gw->GetImpl());
			GL_CHECK();
		}
	}
}

void RenderContext_Base_GL::PostWindowDestruction(RenderWindow* gw) {
	/* do nothing */
}

void  RenderContext_Base_GL::PostCloseImpl() {
	GetImpl()->PostCloseImpl();
	BaseRenderContext::PostCloseImpl();
}

void RenderContext_Base_GL::SetVideoModeImpl(const VideoMode& videoMode) {
	GetImpl()->SetVideoModeImpl(videoMode);
}

void RenderContext_Base_GL::SetViewport(const CommitContext& c) {
	float x = c.viewport->GetTopLeftX() * ((float)c.targetDimension.dx + 0.5f);
	float y = c.viewport->GetTopLeftY() * ((float)c.targetDimension.dy + 0.5f);
	glViewport((GLint)x, (GLint)y, (GLint)c.viewDimensions.dx, (GLint)c.viewDimensions.dy);
}

void RenderContext_Base_GL::Draw(StreamData* streamData, CommitContext& ctx) {
	/**
	 * Note on functions/extensions used:
	 * + GlBindBuffer - to bind the VBO
	 * + GlBindVertexArray - to bind the Vertex Layout ala VAO
	 * + glDrawElements - indexed draw when vertexData.start is 0
	 * + GlDrawElementsBaseVertex - indexed draw when vertexData.start is > 0
	 * + glDrawArrays - non indexed draw
	 * + GlDrawElementsInstanced - indexed draw with base = 0
	 * + GlDrawElementsInstancedBaseVertex - indexed draw with base > 0
	 * + GlDrawArraysInstanced - non indexed instanced draw
	 */
	VertexData& vd = streamData->vertices;
	// bind input layout
	NEX_ASSERT(vd.layout);
	VertexLayoutGL* layout = static_cast<VertexLayoutGL*>(GetView(vd.layout));
	VertexBufferBinding& binding = (*vd.binding);
	layout->Enable(binding, static_cast<PassViewGL*>(ctx.pass), this);

	GLint primtype;
	switch (streamData->type) {
	case PT_POINT_LIST:
		primtype = GL_POINTS;
		break;
	case PT_LINE_LIST:
		primtype = GL_LINES;
		break;
	case PT_TRI_LIST:
		primtype = GL_TRIANGLES;
		break;
	case PT_TRI_STRIP:
		primtype = GL_TRIANGLE_STRIP;
		break;
	case PT_TRI_FAN:
		primtype = GL_TRIANGLE_FAN;
		break;
	}

	IndexBuffer* ibuffer = streamData->indices.indices.GetPtr();
	if (ibuffer) {
		GpuBufferViewGL* buffer = static_cast<GpuBufferViewGL*>(GetView(ibuffer));
		GLint indexsize = buffer->GetStride();
		uint32 offset = buffer->GetOffset();
		// hack
		NEX_STATIC_ASSERT(GL_UNSIGNED_INT == GL_UNSIGNED_SHORT + 2);
		GLenum indextype = GL_UNSIGNED_SHORT + (indexsize - 2);
		GlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->GetBufferId());
		GL_CHECK();
		if (vd.start == 0) {
			if (streamData->instanceCount == 1)
				glDrawElements(primtype, streamData->indices.count, indextype,
					reinterpret_cast<const GLvoid*> ((indexsize * (GLint)streamData->indices.start) + offset));
			else
				GlDrawElementsInstanced(primtype, streamData->indices.count,
				indextype,
				reinterpret_cast<const GLvoid*> (indexsize * (GLint)streamData->indices.start + offset),
				streamData->instanceCount);
		} else {
			if (streamData->instanceCount == 1)
				GlDrawElementsBaseVertex(primtype, streamData->indices.count,
				indextype,
				reinterpret_cast<const GLvoid*> (indexsize * (GLint)streamData->indices.start + offset),
				vd.start);
			else
				GlDrawElementsInstancedBaseVertex(primtype,
				streamData->indices.count, indextype,
				reinterpret_cast<const GLvoid*> (indexsize * (GLint)streamData->indices.start + offset),
				streamData->instanceCount, vd.start);
		}
	} else {
		if (streamData->instanceCount == 1) 
			glDrawArrays(primtype, vd.start, vd.count);
		else
			GlDrawArraysInstanced(primtype, vd.start, vd.count,
			streamData->instanceCount);
	}
	GL_CHECK();
	layout->Disable(this);
}

void RenderContext_Base_GL::SetCurrentTarget(RenderTarget* canvas) {
	if (canvas == nullptr) {
		SetCurrentWindow(nullptr);
	} else {
		switch (canvas->GetRenderTargetType()) {
		case RenderTargetType::TEXTURE: {
			RenderTextureViewGL* textureView =
				static_cast<RenderTextureViewGL*>(GetView(
				static_cast<RenderTexture*>(canvas)));
			FrameBufferObjectGL& fbo = textureView->GetFBO();
			if (!fbo.IsValid())
				CreateFBO(textureView);
			BindFBO(fbo);
			contextFlags |= CURRENT_TARGET_FBO;
			currentCountOfColorAttachments = textureView->IsColorTarget();
		}
										break;

		case RenderTargetType::RENDER_BUFFER: {
			RenderBufferViewGL* textureView =
				static_cast<RenderBufferViewGL*>(
				GetView(static_cast<RenderBuffer*>(canvas)));
			FrameBufferObjectGL& fbo = textureView->GetFBO();
			if (!fbo.IsValid())
				CreateFBO(textureView);
			BindFBO(fbo);
			contextFlags |= CURRENT_TARGET_FBO;
			currentCountOfColorAttachments = textureView->IsColorTarget();
		}
											  break;

		case RenderTargetType::BACK_BUFFER:
			if (contextFlags & CURRENT_TARGET_FBO) {
				UnbindFBO(false);
				contextFlags &= ~CURRENT_TARGET_FBO;
			}

			if (currentWindow != canvas) {
				currentWindow = canvas;
				SetCurrentWindow(canvas);
			}
			currentCountOfColorAttachments = 0;
			break;

		case RenderTargetType::MULTI_RENDER_TARGET: {
			MultiRenderTargetViewGL* textureView =
				static_cast<MultiRenderTargetViewGL*>(GetView(
				static_cast<MultiRenderTarget*>(canvas)));
			FrameBufferObjectGL& fbo = textureView->GetFBO();
			NEX_ASSERT(fbo.IsValid());
			BindFBO(fbo);
			// set the individual draw buffers
			if ((currentCountOfColorAttachments = textureView->GetColorAttachmentCount()))
				GlDrawBuffers(currentCountOfColorAttachments, s_attachmentMap + 6);
			contextFlags |= CURRENT_TARGET_FBO;
		}
			break;
		}
	}
}

void RenderContext_Base_GL::Clear(const ClearBufferInfo& info, ClearFlags cflags) {
	if (Test(cflags & ClearFlags::CLEAR_COLOR)) {
		if (currentCountOfColorAttachments) {
			for (uint32 i = 0; i < currentCountOfColorAttachments; ++i) {
				ClearFlags cf = (ClearFlags)((uint16)ClearFlags::CLEAR_COLOR_0 << (uint16)i);
				if (Test(cflags & cf))
					GlClearBufferfv(GL_COLOR, i, info.clearColor[i].AsFloatArray());
			}
		} else {
			GlClearBufferfv(GL_COLOR, 0, info.clearColor[0].AsFloatArray());
		}
	}

	if (Test(cflags & ClearFlags::CLEAR_DEPTH) &&
		Test(cflags & ClearFlags::CLEAR_STENCIL)) {
		if (!depthStencilState.depthWrite) {
			depthStencilState.depthWrite = true;
			glDepthMask(GL_TRUE);
		}
					
		GlClearBufferfi(GL_DEPTH_STENCIL, 0, info.clearDepth, info.clearStencil);
	} else if (Test(cflags & ClearFlags::CLEAR_DEPTH)) {
		if (!depthStencilState.depthWrite) {
			depthStencilState.depthWrite = true;
			glDepthMask(GL_TRUE);
		}

		GlClearBufferfv(GL_DEPTH, 0, &info.clearDepth);
	} else if (Test(cflags & ClearFlags::CLEAR_STENCIL)) {
		GLint stencil = info.clearStencil;
		GlClearBufferiv(GL_STENCIL, 0, &stencil);
	}
	GL_CHECK();
}

void RenderContext_Base_GL::Copy(
	RenderTarget* source, FrameBuffer src,
	RenderTarget* dest, FrameBuffer dst) {

	NEX_ASSERT(source->GetDimensions() == dest->GetDimensions());
	Size s = source->GetDimensions();
	Size d = dest->GetDimensions();
	SpecifyTargetState(true, true, source, src);
	SpecifyTargetState(false, true, dest, dst);
	GLbitfield mask = GL_COLOR_BUFFER_BIT;
	switch (src) {
	case FrameBuffer::DEPTH:
		mask = GL_DEPTH_BUFFER_BIT; break;
	case FrameBuffer::STENCIL:
		mask = GL_STENCIL_BUFFER_BIT; break;
	}
	GlBlitFramebuffer(0, 0, s.dx, s.dy, 0, 0, d.dx, d.dy, mask, GL_NEAREST);
	GL_CHECK();

	SpecifyTargetState(false, false, dest, dst);
	SpecifyTargetState(true, false, source, src);
	SetCurrentTarget(GetCurrentTarget());
}

void RenderContext_Base_GL::SpecifyTargetState(
	bool readOrDraw, bool setUnset,
	RenderTarget* canvas, FrameBuffer src) {
	// setup read
	switch (canvas->GetRenderTargetType()) {
	case RenderTargetType::TEXTURE: {
		RenderTextureViewGL* textureView =
			static_cast<RenderTextureViewGL*>(GetView(
			static_cast<RenderTexture*>(canvas)));
		FrameBufferObjectGL& fbo = textureView->GetFBO();
		if (!fbo.IsValid())
			CreateFBO(textureView);
		if (setUnset) {
			if (src >= FrameBuffer::COLOR_0 && src <= FrameBuffer::COLOR_7)
				BindNamedFBO(fbo, readOrDraw, src);
			else
				BindFBO(fbo, readOrDraw);
		} else {
			if (src >= FrameBuffer::COLOR_0 && src <= FrameBuffer::COLOR_7)
				UnbindNamedFBO(readOrDraw);
			else
				UnbindFBO(readOrDraw);
		}
	}
									break;
	case RenderTargetType::RENDER_BUFFER: {
		RenderBufferViewGL* textureView =
			static_cast<RenderBufferViewGL*>(GetView(
			static_cast<RenderBuffer*>(canvas)));
		FrameBufferObjectGL& fbo = textureView->GetFBO();
		if (!fbo.IsValid())
			CreateFBO(textureView);
		if (setUnset) {
			if (src >= FrameBuffer::COLOR_0 && src <= FrameBuffer::COLOR_7)
				BindNamedFBO(fbo, readOrDraw, src);
			else
				BindFBO(fbo, readOrDraw);
		} else {
			if (src >= FrameBuffer::COLOR_0 && src <= FrameBuffer::COLOR_7)
				UnbindNamedFBO(readOrDraw);
			else
				UnbindFBO(readOrDraw);
		}

	}
										  break;
	case RenderTargetType::BACK_BUFFER:
		SetCurrentWindow(canvas);
		if (setUnset) {
			if (readOrDraw)
				SetReadBuffer(GL_BACK);
			else
				SetDrawBuffer(GL_BACK);
		}
		break;
	case RenderTargetType::MULTI_RENDER_TARGET: {
		MultiRenderTargetViewGL* textureView =
			static_cast<MultiRenderTargetViewGL*>(GetView(
			static_cast<MultiRenderTarget*>(canvas)));
		FrameBufferObjectGL& fbo = textureView->GetFBO();
		NEX_ASSERT(fbo.IsValid());
		if (setUnset) {
			if (src >= FrameBuffer::COLOR_0 && src <= FrameBuffer::COLOR_7)
				BindNamedFBO(fbo, readOrDraw, src);
			else
				BindFBO(fbo, readOrDraw);
		} else {
			if (src >= FrameBuffer::COLOR_0 && src <= FrameBuffer::COLOR_7)
				UnbindNamedFBO(readOrDraw);
			else
				UnbindFBO(readOrDraw);
		}
		break;
	}
	}
}

void RenderContext_Base_GL::BindNamedFBO(FrameBufferObjectGL& fbo,
										 bool readBuffer, FrameBuffer fb) {
	NEX_ASSERT(fb >= FrameBuffer::COLOR_0 && fb <= FrameBuffer::COLOR_7);
	if (readBuffer) {
		BindFBO(fbo, true);
		SetReadBuffer(s_attachmentMap[(uint32)fb]);
	} else {
		BindFBO(fbo, false);
		SetDrawBuffer(s_attachmentMap[(uint32)fb]);
	}
	GL_CHECK();
}

void RenderContext_Base_GL::UnbindNamedFBO(
	bool readBuffer) {
	if (readBuffer) {
		UnbindFBO(true);
		SetReadBuffer(GL_BACK);
	} else {
		UnbindFBO(false);
		SetDrawBuffer(GL_BACK);
	}
	GL_CHECK();
}

void RenderContext_Base_GL::AttachToFBO(RenderTextureViewGL* texture, GLenum type) {
	switch (texture->GetType()) {
	case GL_TEXTURE_1D:
		GlFramebufferTexture1D(GL_FRAMEBUFFER, type, GL_TEXTURE_1D,
							   texture->GetTexture(), 0);
		break;
	case GL_TEXTURE_2D:
		GlFramebufferTexture2D(GL_FRAMEBUFFER, type, GL_TEXTURE_2D,
							   texture->GetTexture(), 0);
		break;
	case GL_TEXTURE_3D:
	case GL_TEXTURE_1D_ARRAY:
	case GL_TEXTURE_2D_ARRAY:
	case GL_TEXTURE_CUBE_MAP:
	case GL_TEXTURE_CUBE_MAP_ARRAY:
		// attach all levels
		NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	}
}

void RenderContext_Base_GL::AttachToFBO(RenderBufferViewGL* rt, GLenum attachmentType) {
	GlFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType,
							  GL_RENDERBUFFER, rt->GetRenderBuffer());
}

bool RenderContext_Base_GL::ValidateFBO() {
#define LOG_CASE(x) case x: Error( #x ); return false
	switch (GlCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER)) {
		LOG_CASE(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
		LOG_CASE(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
		LOG_CASE(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER);
		LOG_CASE(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER);
		LOG_CASE(GL_FRAMEBUFFER_UNSUPPORTED);
		LOG_CASE(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
		LOG_CASE(GL_TEXTURE_FIXED_SAMPLE_LOCATIONS);
		LOG_CASE(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS);
	case GL_FRAMEBUFFER_COMPLETE:
		return true;
	}
	return false;
#undef LOG_CASE
}

void RenderContext_Base_GL::EndRender() {
}

void RenderContext_Base_GL::DetermineConstants() {
	GLint uniformBufferAlignSize = 0;
	GLint uniformBufferMinSize = 0;
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignSize);
	glGetIntegerv(GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBufferMinSize);
	uniformBufferPool.SetAlignmentAndMinSize(uniformBufferAlignSize, uniformBufferMinSize);
}

void RenderContext_Base_GL::DetermineShaderTarget() {
	VersionGL ver = GetContextVersion();
	String target;
	switch (ver) {
	case GLV_2_1:
		target = "120\n"; break;
	case GLV_3_0:
		target = "130\n"; break;
	case GLV_3_1:
		target = "140\n"; break;
	case GLV_3_2:
		target = "150\n"; break;
	case GLV_3_3:
		target = "330\n"; break;
	case GLV_4_0:
		target = "400\n"; break;
	case GLV_4_1:
		target = "410\n"; break;
	case GLV_4_2:
		target = "420\n"; break;
	case GLV_4_3:
		target = "430\n"; break;
	case GLV_4_4:
		target = "440\n"; break;
	case GLV_4_5:
		target = "450\n"; break;
	case GLV_1_3:
	case GLV_1_4:
	case GLV_1_5:
	case GLV_2_0:
	default:
		target += "110\n"; break;

	}

	shaderTarget += "#version " + target;
	shaderTarget += "#define GLSL_version " + target;
	shaderTarget += "\n";
}

GLuint RenderContext_Base_GL::CreateShader(GLenum type, const char* preDef,
										   const char* source) {
	const GLchar* sourceBuff[3] = { 0 };
	// determine shader version

	sourceBuff[0] = shaderTarget.c_str();
	sourceBuff[1] = static_cast<const GLchar*>(preDef ? preDef : "");
	sourceBuff[2] = static_cast<const GLchar*>(source);
	GLuint shaderObject = GlCreateShader(type);
	GL_CHECK();
	GlShaderSource(shaderObject, 3, sourceBuff, NULL);
	GL_CHECK();
	GlCompileShader(shaderObject);
	GL_CHECK();
	// lets check the compilation flag
	GLint compileStatus;
	GlGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compileStatus);
	GL_CHECK();
	if (compileStatus == GL_FALSE) {
		/* log compilation error */
		GLint infoLogLength = 1;
		GlGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		GL_CHECK();
		GLchar* infoLog = static_cast<GLchar*>(NEX_ALLOC(infoLogLength + 2,
			MEMCAT_GENERAL));
		GlGetShaderInfoLog(shaderObject, infoLogLength + 1, NULL, infoLog);
		GL_CHECK();
		Error(String("Shader compilation failed: ") + infoLog);
		NEX_FREE(infoLog, MEMCAT_GENERAL);
		GlDeleteShader(shaderObject);
		shaderObject = 0;
	}
	return shaderObject;
}

void RenderContext_Base_GL::DestroyShader(GLuint sh) {
	GlDeleteShader(sh);
	GL_CHECK();
}

GLuint RenderContext_Base_GL::CreateProgram(GLuint shaders[]) {
	/* create the program object */
	GLuint program = GlCreateProgram();
	for (nextar::uint32 i = 0; i < Pass::STAGE_COUNT; ++i) {
		if (shaders[i]) {
			GlAttachShader(program, shaders[i]);
			GL_CHECK();
		}
	}
	/* link the program and let it allocate the attribute
	 * indices. These default allocations will be used to
	 * create the vertex signature. This will enable the program
	 * to determine:
	 * 1. What are the active attributes?
	 * 2. How they are bound (i.e. index numbers)?
	 */
	GlLinkProgram(program);
	GL_CHECK();
	GLint linkstatus = 0;
	bool relink = false;
	bool validation = false;
	GlGetProgramiv(program, GL_LINK_STATUS, &linkstatus);
	GL_CHECK();
	if (linkstatus == GL_TRUE) {
		GLint validstatus = 0;
		GlValidateProgram(program);
		GL_CHECK();
		GlGetProgramiv(program, GL_VALIDATE_STATUS, &validstatus);
		GL_CHECK();
		if (validstatus == GL_TRUE) {
			validation = true;
		}
	}

	if (!validation) {
		GLint infoLogLen = 0;
		GLchar* infoLog;
		// debug out the error messages
		GlGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
		if (infoLogLen > 0) {
			infoLog = static_cast<GLchar*>(NEX_ALLOC(
				sizeof(GLchar) * infoLogLen, MEMCAT_GENERAL));
			GlGetProgramInfoLog(program, infoLogLen, &infoLogLen, infoLog);
			GL_CHECK();
			Error(String("Shader linking failed: ") + infoLog);
			NEX_FREE(infoLog, MEMCAT_GENERAL);
			GlDeleteProgram(program);
			GL_CHECK();
			program = 0;
		}
	}

	return program;

}

void RenderContext_Base_GL::DestroyProgram(GLuint program) {
	GlDeleteProgram(program);
	GL_CHECK();
}

uint32 RenderContext_Base_GL::ReadProgramSemantics(GLuint program,
												   VertexSemanticGL* inpComp) {

	GLint numAttribs = 0;
	// outputSignature = 0;

	char attribName[64];
	uint32 inpCount = 0;
	uint32 outCount = 0;

	GlGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numAttribs);
	String signature;
	for (GLint i = 0; i < numAttribs; ++i) {
		GLuint attribLoc = 0;
		GLint attribSize = 0;
		GLenum attribType = 0;
		GlGetActiveAttrib(program, i, 128, NULL, &attribSize, &attribType,
						  attribName);
		// map this to a matching semantic
		// if no match is found, we have a problem
		SemanticDef semantic = VertexSemantic::MapSemantic(attribName);
		if (semantic.semantic >= COMP_RESERVED_COUNT) {
			Warn(String("Unmatched input semantic: ") + attribName);
			continue;
		}
		attribLoc = GlGetAttribLocation(program, attribName);
		/* We do not think about output formats now */
		if (semantic.inputSemantic) {
			NEX_ASSERT(inpCount < VertexElement::MAX_VERTEX_ELEMENT);
			inpComp[inpCount].index = attribLoc;
			inpComp[inpCount].semantic.type = GetSemanticType(attribType);
			inpComp[inpCount].semantic.instanceData = semantic.instanceData;
			inpComp[inpCount].semantic.semanticIndex = semantic.semanticIndex;
			inpComp[inpCount].semantic.semantic = semantic.semantic;
			inpCount++;
		} else {
			/** todo Add support for output semantic */
		}

	}

	if (inpCount <= 0) {
		Warn("No input semantics found to map into the shader!");
		return 0;
	}

	/* sort based on semantic + index to keep signature consistents */
	//std::sort( inpComp, inpComp + inpCount );
	return (uint32)inpCount;
}

void RenderContext_Base_GL::ReadUniforms(PassViewGL* pass, uint32 passIndex,
										 GLuint program,
										 const Pass::VarToAutoParamMap& remapParams,
										 ParamEntryTable* paramTable) {
	GLint numBlocks = 0;
	GlGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
	GL_CHECK();
	char name[128];
	ParameterGroupList& ubList = pass->sharedParameters;
	ubList.reserve(numBlocks);
	for (GLint i = 0; i < numBlocks; ++i) {
		GLint size = 0;
		GLint numParams = 0;
		GlGetActiveUniformBlockName(program, i, 128, NULL, name);
		GL_CHECK();
		GlGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_DATA_SIZE,
								  &size);
		GL_CHECK();
		GlGetActiveUniformBlockiv(program, i,
								  GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numParams);
		GL_CHECK();
		//NEX_ASSERT(numParams < MAX_PARAMS);
		if (numParams == 0) {
			continue;
		}
		//bool shareUb = true;
		UniformBufferGL* ubPtr = uniformBufferPool.Acquire(this, name, size);
		if (!ubPtr) {
			Warn(String("Could not allocate uniform buffer: ") + name);
			continue;
		}
			
		if (ubPtr->ref.numOfRef == 1) {
			// populate
			InitializeUniformBuffer(*ubPtr, pass, passIndex, name, i, program,
										numParams, size, remapParams);
		}

		PrepareParamTable(*ubPtr, passIndex, paramTable);

		ubList.push_back({ (uint32)i, ubPtr });
	}
	// sort ublist by context
	std::sort(ubList.begin(), ubList.end(), [](const ParameterGroupData& p1, const ParameterGroupData& p2){
		return (p1->context < p2->context) != 0;
	});
}

void RenderContext_Base_GL::PrepareParamTable(const UniformBufferGL& u, uint32 passIndex, ParamEntryTable* table) {
	// parameter parsing is not important if the
	// the auto param is well defined and understood
	// by the engine
	if (!u.parameter) {
		return;
	}
		
	uint32 startParamIndex = (uint32)table->size();
	table->reserve(startParamIndex + u.numParams);

	const UniformGL* en = static_cast<const UniformGL*>(u.parameter) + u.numParams;
	for (const UniformGL* uniform = static_cast<const UniformGL*>(u.parameter); uniform != en; ++uniform) {
		if (uniform->autoName == AutoParamName::AUTO_CUSTOM_CONSTANT) {
			auto& uform = *uniform;
			ParamEntry pe;
			pe.arrayCount = uform.arrayCount;
			pe.autoName = uform.autoName;
			pe.maxSize = uform.size;
			pe.name = &uform.name;
			pe.type = uform.type;
			pe.passIndex = passIndex;
			pe.context = u.context;
			table->push_back(pe);
		}
	}

}

void RenderContext_Base_GL::InitializeUniformBuffer(
	UniformBufferGL& u, PassViewGL* pass,
	uint32 passIndex, const char* name, GLint blockIndex, GLuint prog,
	GLuint numParams, uint32 size, const Pass::VarToAutoParamMap& remapParams
	) {

	NEX_ASSERT(size > 0);
	uint16 numUnmappedParams = 0;
	u.size = size;
	u.numParams = numParams;
	u.lastUpdateId = -1;
	u.arrayCount = 1;
	u.type = ParamDataType::PDT_STRUCT;
	u.parameter = nullptr;
	u.processor = nullptr;

	bool parseIndividualParams = true;
	const AutoParam* autoParam = nullptr;
	// look for AutoParams
	auto it = remapParams.find(name);

	if (it != remapParams.end())
		autoParam = pass->MapParam((*it).second);
	if (autoParam && autoParam->type == ParamDataType::PDT_STRUCT) {
		u.autoName = autoParam->autoName;
		u.context = autoParam->context;
		u.processor = autoParam->processor;
		u.type = ParamDataType::PDT_STRUCT;
		if (u.processor)
			parseIndividualParams = false;
	} else {
		u.context = ParameterContext::CTX_UNKNOWN;
		u.autoName = AutoParamName::AUTO_CUSTOM_CONSTANT;
		u.type = ParamDataType::PDT_UNKNOWN;
	}

	u.size = size;
		
	if (!parseIndividualParams)
		return;

	void* tempBuffer = NEX_ALLOC(sizeof(GLint) * numParams * 7 + 128,
								 MEMCAT_GENERAL);
	GLint *indices = static_cast<GLint*>(tempBuffer)+numParams * 0;
	GLint *type = static_cast<GLint*>(tempBuffer)+numParams * 1;
	GLint *offset = static_cast<GLint*>(tempBuffer)+numParams * 2;
	GLint *arraynum = static_cast<GLint*>(tempBuffer)+numParams * 3;
	GLint *rowMaj = static_cast<GLint*>(tempBuffer)+numParams * 4;
	GLint *matStride = static_cast<GLint*>(tempBuffer)+numParams * 5;
	GLint *arrayStride = static_cast<GLint*>(tempBuffer)+numParams * 6;
	char *uniName = reinterpret_cast<char*>(static_cast<GLint*>(tempBuffer)
											+numParams * 7);

	const GLuint* uindices = (GLuint*)indices;
	//GLuint blockIndex = GlGetUniformBlockIndex(prog, (const GLchar*)name.c_str());
	GlGetActiveUniformBlockiv(prog, blockIndex,
							  GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices);
	GL_CHECK();
	GlGetActiveUniformsiv(prog, numParams, uindices, GL_UNIFORM_TYPE, type);
	GL_CHECK();
	GlGetActiveUniformsiv(prog, numParams, uindices, GL_UNIFORM_OFFSET, offset);
	GL_CHECK();
	GlGetActiveUniformsiv(prog, numParams, uindices, GL_UNIFORM_SIZE, arraynum);
	GL_CHECK();
	GlGetActiveUniformsiv(prog, numParams, uindices, GL_UNIFORM_IS_ROW_MAJOR,
						  rowMaj);
	GL_CHECK();
	GlGetActiveUniformsiv(prog, numParams, uindices, GL_UNIFORM_MATRIX_STRIDE,
						  matStride);
	GL_CHECK();
	GlGetActiveUniformsiv(prog, numParams, uindices, GL_UNIFORM_ARRAY_STRIDE,
						  arrayStride);
	GL_CHECK();

	// First we determine the context of the auto-param.
	// 1. If there are no auto-params we guess the param context from hints in the buffer name.
	// 2. If auto-params are retrieved we take the first auto-param context. We check the
	//    consistency of context for other auto-param found. If any discrepency is observed
	//    the buffer is rejected.
	// We can safely say that a struct processor is applicable for the buffer under these conditions:
	// 1. If the context is other than VIEW and FRAME
	// 2. If no auto-params are found.
	// We can ignore storing the individual param data in a buffer if a struct processor is applicable.
	// We should parse the params anyway, if the params are required for paramTable for lookup.
	uint32 autoParamCount = 0;
	ParameterContext chosen = u.context;
	UniformGL* uniforms = NEX_NEW(UniformGL[numParams]);
	uint32 startParamIndex = -1;
	
	for (GLint i = 0; i < (GLint)numParams; ++i) {
		UniformGL& uform = uniforms[i];
		GlGetActiveUniformName(prog, indices[i], 128, 0, uniName);
		GL_CHECK();
		// put the string construction outside the loop? we do a move for custom params however.
		String paramName = uniName;
		size_t np = paramName.find_first_of('.');
		if (np != String::npos) {
			paramName = paramName.substr(np + 1);
		}
		auto it = remapParams.find(paramName);
		const AutoParam* paramDef = nullptr;
		if (it != remapParams.end())
			paramDef = pass->MapParam((*it).second);

		if (paramDef) {
			if (chosen == ParameterContext::CTX_UNKNOWN)
				chosen = paramDef->context;
			else if (chosen != paramDef->context
					 && paramDef->context != ParameterContext::CTX_UNKNOWN) {
				Warn(
					String("Parameters from different contexts cannot be mixed for buffer: ")
					+ name + "with parameter: " + paramName);
				// discard??
			}
			uform.autoName = paramDef->autoName;
			uform.processor = paramDef->processor;
			autoParamCount++;
		} else {
			uform.name = std::move(paramName);
			uform.autoName = AutoParamName::AUTO_CUSTOM_CONSTANT;
			uform.processor = Pass::GetConstantProcessor();
		}
		uform.isRowMajMatrix = rowMaj[i] ? true : false;
		uform.typeGl = type[i];
		uform.matrixStride = matStride[i];
		uform.arrayStride = arrayStride[i];
		uform.bufferOffset = offset[i];
		uform.arrayCount = uint16(arraynum[i]);
		uform.size = GetShaderParamSize(uform.typeGl) * uform.arrayCount;
		uform.type = GetShaderParamType(uform.typeGl);

	}

	if (chosen == ParameterContext::CTX_UNKNOWN)
		chosen = GuessContextByName(name);
	if (chosen == ParameterContext::CTX_UNKNOWN) {
		Warn(
			String("Buffer context could not be determined, assigning material context: ")
			+ name);
		chosen = ParameterContext::CTX_MATERIAL;
	}
	u.context = chosen;
	u.parameter = uniforms;
	// the context was just determined
	if (!autoParamCount && chosen != ParameterContext::CTX_VIEW
		&& chosen != ParameterContext::CTX_FRAME) {
		// lets look for the struct processor
		if (!u.processor)
			u.processor = Pass::GetStructProcessor();
		u.type = ParamDataType::PDT_STRUCT;
	} else {
		// sort the resulting uniforms such that auto params come first
		// followed by unmapped params. Unmapped params are sorted by buffer offset.
		std::sort(uniforms, uniforms + numParams,
				  [](const UniformGL& first, const UniformGL& second) {
			if (first.autoName == second.autoName) {
				NEX_ASSERT(first.autoName == AutoParamName::AUTO_CUSTOM_CONSTANT);
				return first.bufferOffset < first.bufferOffset;
			}
			return (first.autoName <
					second.autoName) != 0;
		});
	}

	NEX_FREE(tempBuffer, MEMCAT_GENERAL);
}

GLuint RenderContext_Base_GL::CreateSamplerFromParams(const TextureUnitParams& params) {
	GLuint sampler;
	GlGenSamplers(1, &sampler);
	GL_CHECK();
	if (sampler) {
		GlSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER,
							GetGlMinFilter(params.minFilter));
		GL_CHECK();
		GlSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER,
							GetGlMagFilter(params.magFilter));
		GL_CHECK();
		GlSamplerParameteri(sampler, GL_TEXTURE_WRAP_S,
							GetGlAddressMode(params.uAddress));
		GL_CHECK();
		GlSamplerParameteri(sampler, GL_TEXTURE_WRAP_T,
							GetGlAddressMode(params.vAddress));
		GL_CHECK();
		GlSamplerParameteri(sampler, GL_TEXTURE_WRAP_R,
							GetGlAddressMode(params.wAddress));
		GL_CHECK();
	}
	if (params.comparisonFunc != TEXCOMP_NONE) {
		GlSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC,
							GetGlCompareFunc(params.comparisonFunc));
		GL_CHECK();
		GlSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE,
							GL_COMPARE_REF_TO_TEXTURE);
		GL_CHECK();
	} else {
		GlSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		GL_CHECK();
	}
	GlSamplerParameterf(sampler, GL_TEXTURE_LOD_BIAS,
						params.lodBias);
	GL_CHECK();
	GlSamplerParameterf(sampler, GL_TEXTURE_MIN_LOD, params.minLod-1000);
	GL_CHECK();
	GlSamplerParameterf(sampler, GL_TEXTURE_MAX_LOD, params.maxLod);
	GL_CHECK();
	GlSamplerParameteri(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT,
						params.maxAnisotropy);
	GL_CHECK();
	GlSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR,
						 params.borderColor.AsFloatArray());
	GL_CHECK();
	return sampler;
}

void RenderContext_Base_GL::ReadSamplers(PassViewGL* pass, uint32 passIndex,
										 GLuint program,
										 const Pass::VarToAutoParamMap& remapParams,
										 ParamEntryTable* paramTable,
										 const Pass::TextureDescMap& texMap) {
	/** todo Massive work left for sampler arrays */
	GLint numUni = 0;
	uint32 numSamplers = 0;
	char name[128];
	size_t extra = 0;
	GlGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUni);
	vector<GLuint>::type samplerList;

	for (auto& s : texMap) {
		samplerList.push_back(CreateSamplerFromParams(s.texUnitParams));
	}

	for (GLuint i = 0; i < (GLuint)numUni; ++i) {
		GLint type;
		GlGetActiveUniformsiv(program, 1, &i, GL_UNIFORM_TYPE, &type);
		if (IsSamplerType(type)) {
			numSamplers++;
		}
	}

	if (!numSamplers) {
		pass->samplers = nullptr;
		pass->numSamplerCount = 0;
		return;
	}

	GL_CHECK();
	// assume all are samplers, otherwise we got a problem
	SamplerState* samplers = NEX_NEW(SamplerState[numSamplers]);
	uint32 mapped = 0;
	uint32 startParamIndex = -1;
	if (paramTable) {
		startParamIndex = (uint32)paramTable->size();
		paramTable->reserve(numSamplers + startParamIndex);
	}
	GlUseProgram(program);
	for (GLuint i = 0; i < (GLuint)numUni; ++i) {

		GLint type;
		GlGetActiveUniformsiv(program, 1, &i, GL_UNIFORM_TYPE, &type);
		GL_CHECK();

		if (IsSamplerType(type)) {

			GlGetActiveUniformName(program, i, 128, 0, name);
			GL_CHECK();

			GLint loc = GlGetUniformLocation(program, name);
			GL_CHECK();

			String unitName = name;

			SamplerState& ss = samplers[mapped];
			ss.index = mapped++;
			ss.arrayCount = 1;
			uint32 tu = Pass::MapSamplerParams(unitName,
				texMap, ss.context);
			if (tu >= samplerList.size()) {
				Error(
					String(
					"Overflowing/Unspecified texture unit index for ")
					+ name);
				continue;
			}
			const AutoParam* paramDef = nullptr;
			auto it = remapParams.find(name);
			if (it != remapParams.end())
				paramDef = pass->MapParam((*it).second);
			ss.sampler = samplerList[tu];
			ss.location = loc;
			if (paramDef == nullptr) {
				ss.autoName = AutoParamName::AUTO_CUSTOM_CONSTANT;
				if (ss.context == ParameterContext::CTX_UNKNOWN)
					ss.context = GuessContextByName(unitName,
					ParameterContext::CTX_MATERIAL);
				ss.processor = Pass::GetTextureProcessor();
				ss.name = std::move(unitName);
			} else {
				ss.autoName = paramDef->autoName;
				ss.context = paramDef->context;
				ss.processor = paramDef->processor;
			}
			ss.size = sizeof(TextureUnit);
			// todo Should be a view
			if (paramTable && !paramDef) {
				ParamEntry pe;
				pe.arrayCount = 1;
				pe.autoName = ss.autoName;
				pe.context = ss.context;
				pe.maxSize = ss.size;
				pe.name = &ss.name;
				pe.passIndex = passIndex;
				pe.type = ParamDataType::PDT_TEXTURE;
				paramTable->push_back(pe);
			}
			// @optimize Reuse samplers. Samplers are currently bound to unit names
			// It is possible to bind multiple units to sampler sampler in GLSL.
			// This support is necessary in here.
			// Bind the sampler variable to sampler index
			GlUniform1i(ss.location, ss.index);
			GL_CHECK();
		}
	}
	if (mapped != numSamplers)
		Warn("Not all samplers were mapped!");
	pass->samplers = samplers;
	pass->numSamplerCount = mapped;
	std::sort(samplers, samplers + mapped,
			  [](const SamplerState& s1, const SamplerState& s2) {
		return s1.autoName == s2.autoName ? s1.index < s2.index :
			(s1.autoName < s2.autoName);
	});
	GlUseProgram(0);
}

void RenderContext_Base_GL::Capture(PixelBox& image, RenderTarget* rt, GLuint *pbo,
									FrameBuffer frameBuffer) {
	/* todo Expand this function for frame buffer,
	 * depth map capture etc. */
	Size size = rt->GetDimensions();
	// assuming color buffer capture
	size_t dataSize = size.dx * size.dy * 4;

	if (!image.Data()) {
		image.Data() = static_cast<uint8*>((NEX_ALLOC(dataSize, MEMCAT_GENERAL)));
		image.deleteData = true;
	}

	bool asyncCapture =
		RenderManager::Instance().GetRenderSettings().asyncCapture;

	glReadBuffer(FrameBufferObjectGL::attachment[(uint32)frameBuffer]);
	GL_CHECK();
	if (asyncCapture) {
		uint32 next = RenderConstants::MAX_FRAME_PRE_CAPTURE - 1;
		if (!pbo[0]) {
			// need to create pbo, and this is our first go
			next = 0;
			GlGenBuffers(RenderConstants::MAX_FRAME_PRE_CAPTURE, pbo);
			GL_CHECK();
			for (uint32 i = 0; i < RenderConstants::MAX_FRAME_PRE_CAPTURE;
				 ++i) {
				GlBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[0]);
				GL_CHECK();
				GlBufferData(GL_PIXEL_PACK_BUFFER, dataSize, 0, GL_STREAM_READ);
				GL_CHECK();
			}
			GlBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		}

		GlBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[next]);
		GL_CHECK();
		glReadPixels(0, 0, size.dx, size.dy, GL_BGRA, GL_UNSIGNED_BYTE, 0);
		GL_CHECK();
		// map and read
		GLuint cur = pbo[0];
		for (uint32 i = 0; i < RenderConstants::MAX_FRAME_PRE_CAPTURE - 1; ++i)
			pbo[i] = pbo[i + 1];
		pbo[RenderConstants::MAX_FRAME_PRE_CAPTURE - 1] = cur;
		GlBindBuffer(GL_PIXEL_PACK_BUFFER, cur);
		GL_CHECK();
		void* data = GlMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		GL_CHECK();
		std::memcpy(image.Data(), data, dataSize);
		GlUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		GL_CHECK();
		GlBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		GL_CHECK();
	} else {
		glReadPixels(0, 0, size.dx, size.dy, GL_BGRA, GL_UNSIGNED_BYTE, image.Data());
		GL_CHECK();
	}

	image.left = 0;
	image.right = size.dx;
	image.top = 0;
	image.bottom = size.dy;
	image.front = 0;
	image.back = 1;
	// todo
	image.format = PixelFormat::BGRA8;
	image.CalculatePitches();
	glReadBuffer(GL_BACK);
}

// @optimize Map all buffers??
void RenderContext_Base_GL::SwitchPass(CommitContext& context, Pass::View* passView) {
	PassViewGL* passViewGl = static_cast<PassViewGL*>(passView);

	SetRasterState(passViewGl->GetRasterState());
	SetDepthStencilState(passViewGl->GetDepthStencilState());
	SetBlendState(passViewGl->GetBlendState());

	GLuint program = passViewGl->GetProgram();
	GlUseProgram(program);
	GL_CHECK();
	ParameterGroupList& paramList = passViewGl->GetSharedParameters();
	for (uint32 i = 0; i < paramList.size(); ++i) {
		UniformBufferGL* ubPtr = static_cast<UniformBufferGL*>(paramList[i].group);
		GlBindBufferRange(GL_UNIFORM_BUFFER, i, ubPtr->ref.ubNameGl, ubPtr->ref.offset, ubPtr->ref.size);
		GL_CHECK();
		GlUniformBlockBinding(program, paramList[i].data, i);
		GL_CHECK();
	}
}

GLuint RenderContext_Base_GL::CreateBuffer(size_t size, GLenum usage, GLenum type) {
	GLuint bufferId = 0;
	GlGenBuffers(1, &bufferId);
	if (!bufferId) {
		Error("Failed to create buffer");
		NEX_THROW_GracefulError(EXCEPT_OUT_OF_RESOURCES);
	}
	GlBindBuffer(type, bufferId);
	GlBufferData(type, size, NULL, usage);
	GlBindBuffer(type, 0);
	return bufferId;
}

void RenderContext_Base_GL::CopyBuffer(GLuint src, GLuint dest, size_t size) {
	GlBindBuffer(GL_COPY_READ_BUFFER, src);
	GlBindBuffer(GL_COPY_WRITE_BUFFER, dest);
	GlCopyBufferSubData(GL_COPY_READ_BUFFER,
						GL_COPY_WRITE_BUFFER, 0, 0, size);
	GL_CHECK();
	GlBindBuffer(GL_COPY_READ_BUFFER, 0);
	GlBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void RenderContext_Base_GL::AllocateTexture(GLenum target, GLint levels,
											GLenum format, GLsizei width, GLsizei height, GLsizei depth) {
	if (!extensions.GLE_ARB_texture_storage)
		return;
	switch (target) {
	case GL_TEXTURE_1D:
		extensions.GlTexStorage1D(target, levels, format, width);
		break;
	case GL_TEXTURE_2D:
	case GL_TEXTURE_RECTANGLE:
	case GL_TEXTURE_CUBE_MAP:
	case GL_TEXTURE_1D_ARRAY:
		extensions.GlTexStorage2D(target, levels, format, width, height);
		break;
	case GL_TEXTURE_3D:
	case GL_TEXTURE_2D_ARRAY:
	case GL_TEXTURE_CUBE_MAP_ARRAY:
		extensions.GlTexStorage3D(target, levels, format, width, height, depth);
		break;
	}
	GL_CHECK();
}

void RenderContext_Base_GL::AllocateTextureLevel(GLenum target, GLint level,
												 const PixelFormatGl& format, GLsizei width, GLsizei height,
												 GLsizei depth, const uint8* data, size_t size) {
	switch (target) {
	case GL_TEXTURE_1D:
		if (format.isCompressed)
			GlCompressedTexImage1D(target, level, format.internalFormat, width,
			0, (GLsizei)size, data);
		else
			glTexImage1D(target, level, format.internalFormat, width, 0,
			format.sourceFormat, format.dataType, data);
		break;
	case GL_TEXTURE_2D:
	case GL_TEXTURE_RECTANGLE:
	case GL_TEXTURE_CUBE_MAP:
	case GL_TEXTURE_1D_ARRAY:
		if (format.isCompressed)
			GlCompressedTexImage2D(target, level, format.internalFormat, width,
			height, 0, (GLsizei)size, data);
		else
			glTexImage2D(target, level, format.internalFormat, width, height, 0,
			format.sourceFormat, format.dataType, data);
		break;
	case GL_TEXTURE_3D:
	case GL_TEXTURE_2D_ARRAY:
	case GL_TEXTURE_CUBE_MAP_ARRAY:
		if (format.isCompressed)
			GlCompressedTexImage3D(target, level, format.internalFormat, width,
			height, depth, 0, (GLsizei)size, data);
		else
			GlTexImage3D(target, level, format.internalFormat, width, height,
			depth, 0, format.sourceFormat, format.dataType, data);
		break;
	}
	GL_CHECK();
}

void RenderContext_Base_GL::WriteTextureLevel(GLenum target, GLint level,
											  const PixelFormatGl& format, GLsizei width, GLsizei height,
											  GLsizei depth, const uint8* data, size_t size) {
	// todo Not sure if it will work for compressed formats
	switch (target) {
	case GL_TEXTURE_1D:
		glTexSubImage1D(target, level, 0, width, format.sourceFormat,
						format.dataType, data);
		break;
	case GL_TEXTURE_2D:
	case GL_TEXTURE_RECTANGLE:
	case GL_TEXTURE_CUBE_MAP:
	case GL_TEXTURE_1D_ARRAY:
		glTexSubImage2D(target, level, 0, 0, width, height, format.sourceFormat,
						format.dataType, data);
		break;
	case GL_TEXTURE_3D:
	case GL_TEXTURE_2D_ARRAY:
	case GL_TEXTURE_CUBE_MAP_ARRAY:
		GlTexSubImage3D(target, level, 0, 0, 0, width, height, depth,
						format.sourceFormat, format.dataType, data);
		break;
	}
	GL_CHECK();

}

void RenderContext_Base_GL::GetTextureData(GLenum target, GLenum format, GLenum type, GLvoid* data) {
	GLenum retformat = format;
	switch (format) {
	case GL_RG16:
		retformat = GL_RG; break;
	case GL_RGBA8:
		retformat = GL_RGBA; break;
	case GL_R8:
		retformat = GL_RED; break;
		// todo
	}
	glGetTexImage(target, 0, retformat, type, data);
	GL_CHECK();
}

bool RenderContext_Base_GL::IsSamplerType(GLint type) {
	switch (type) {
	case GL_SAMPLER_1D: //sampler1D
	case GL_SAMPLER_2D: //sampler2D
	case GL_SAMPLER_3D: //sampler3D
	case GL_SAMPLER_CUBE: //samplerCube
	case GL_SAMPLER_1D_SHADOW: //sampler1DShadow
	case GL_SAMPLER_2D_SHADOW: //sampler2DShadow
	case GL_SAMPLER_1D_ARRAY: //sampler1DArray
	case GL_SAMPLER_2D_ARRAY: //sampler2DArray
	case GL_SAMPLER_1D_ARRAY_SHADOW: //sampler1DArrayShadow
	case GL_SAMPLER_2D_ARRAY_SHADOW: //sampler2DArrayShadow
	case GL_SAMPLER_2D_MULTISAMPLE: //sampler2DMS
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: //sampler2DMSArray
	case GL_SAMPLER_CUBE_SHADOW: //samplerCubeShadow
	case GL_SAMPLER_BUFFER: //samplerBuffer
	case GL_SAMPLER_2D_RECT: //sampler2DRect
	case GL_SAMPLER_2D_RECT_SHADOW: //sampler2DRectShadow
	case GL_INT_SAMPLER_1D: //isampler1D
	case GL_INT_SAMPLER_2D: //isampler2D
	case GL_INT_SAMPLER_3D: //isampler3D
	case GL_INT_SAMPLER_CUBE: //isamplerCube
	case GL_INT_SAMPLER_1D_ARRAY: //isampler1DArray
	case GL_INT_SAMPLER_2D_ARRAY: //isampler2DArray
	case GL_INT_SAMPLER_2D_MULTISAMPLE: //isampler2DMS
	case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: //isampler2DMSArray
	case GL_INT_SAMPLER_BUFFER: //isamplerBuffer
	case GL_INT_SAMPLER_2D_RECT: //isampler2DRect
	case GL_UNSIGNED_INT_SAMPLER_1D: //usampler1D
	case GL_UNSIGNED_INT_SAMPLER_2D: //usampler2D
	case GL_UNSIGNED_INT_SAMPLER_3D: //usampler3D
	case GL_UNSIGNED_INT_SAMPLER_CUBE: //usamplerCube
	case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: //usampler2DArray
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: //usampler2DArray
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: //usampler2DMS
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: //usampler2DMSArray
	case GL_UNSIGNED_INT_SAMPLER_BUFFER: //usamplerBuffer
	case GL_UNSIGNED_INT_SAMPLER_2D_RECT: //usampler2DRect
		return true;
	}
	return false;
}

GLint RenderContext_Base_GL::GetGlMinFilter(TextureMinFilter t) {
	switch (t) {
	case TF_MIN_NEAREST:
		return GL_NEAREST;
	case TF_MIN_LINEAR:
		return GL_LINEAR;
	case TF_MIN_NEAREST_MIPMAP_NEAREST:
		return GL_NEAREST_MIPMAP_NEAREST;
	case TF_MIN_LINEAR_MIPMAP_NEAREST:
		return GL_LINEAR_MIPMAP_NEAREST;
	case TF_MIN_NEAREST_MIPMAP_LINEAR:
		return GL_NEAREST_MIPMAP_LINEAR;
	case TF_MIN_LINEAR_MIPMAP_LINEAR:
		return GL_LINEAR_MIPMAP_LINEAR;
	}
	return GL_LINEAR;
}

GLint RenderContext_Base_GL::GetGlMagFilter(TextureMagFilter t) {
	switch (t) {
	case TF_MAG_NEAREST:
		return GL_NEAREST;
	case TF_MAG_LINEAR:
		return GL_LINEAR;
	}
	return GL_LINEAR;
}

GLint RenderContext_Base_GL::GetGlAddressMode(TextureAddressMode t) {
	switch (t) {
	case TAM_BORDER:
		return GL_CLAMP_TO_BORDER;
	case TAM_CLAMP:
		return GL_CLAMP_TO_EDGE;
	case TAM_MIRROR:
		return GL_MIRRORED_REPEAT;
	case TAM_WRAP:
		return GL_REPEAT;
	}
	return GL_REPEAT;
}

GLenum RenderContext_Base_GL::GetGlStencilOp(StencilOp type) {
	switch (type) {
	case STENCILOP_ZERO:
		return GL_ZERO;
	case STENCILOP_REPLACE:
		return GL_REPLACE;
	case STENCILOP_INCR_SAT:
		return GL_INCR_WRAP;
	case STENCILOP_DECR_SAT:
		return GL_DECR_WRAP;
	case STENCILOP_INVERT:
		return GL_INVERT;
	case STENCILOP_INCR:
		return GL_INCR;
	case STENCILOP_DECR:
		return GL_DECR;
	}
	return GL_KEEP;
}

GLenum RenderContext_Base_GL::GetGlCompareFunc(TextureComparisonMode type) {
	switch (type) {
	case TEXCOMP_NONE:
	case TEXCOMP_NEVER:
		return GL_NEVER;
	case TEXCOMP_LESS:
		return GL_LESS;
	case TEXCOMP_EQUAL:
		return GL_EQUAL;
	case TEXCOMP_LESS_EQUAL:
		return GL_LEQUAL;
	case TEXCOMP_GREATER:
		return GL_GREATER;
	case TEXCOMP_NOT_EQUAL:
		return GL_NOTEQUAL;
	case TEXCOMP_GREATER_EQUAL:
		return GL_GEQUAL;
	}
	return 0;
}

GLenum RenderContext_Base_GL::GetGlCompareFunc(DepthStencilCompare type) {
	switch (type) {
	case DSCOMP_NEVER:
		return GL_NEVER;
	case DSCOMP_LESS:
		return GL_LESS;
	case DSCOMP_EQUAL:
		return GL_EQUAL;
	case DSCOMP_DEFAULT:
	case DSCOMP_LESS_EQUAL:
		return GL_LEQUAL;
	case DSCOMP_GREATER:
		return GL_GREATER;
	case DSCOMP_NOT_EQUAL:
		return GL_NOTEQUAL;
	case DSCOMP_GREATER_EQUAL:
		return GL_GEQUAL;
	}
	return GL_ALWAYS;
}

VertexComponentType RenderContext_Base_GL::GetSemanticType(GLenum e) {
	switch (e) {
	case GL_FLOAT:
		return COMP_TYPE_FLOAT1;
	case GL_FLOAT_VEC2:
		return COMP_TYPE_FLOAT2;
	case GL_FLOAT_VEC3:
		return COMP_TYPE_FLOAT3;
	case GL_FLOAT_VEC4:
		return COMP_TYPE_FLOAT4;
	case GL_UNSIGNED_INT:
		return COMP_TYPE_UNSIGNED_INT;
	default:
		NEX_ASSERT(0);
	}
	return COMP_TYPE_INVALID;
}

uint16 RenderContext_Base_GL::GetShaderParamSize(GLuint type) {
	switch (type) {
	case GL_BOOL:
		return (uint16)1;
	case GL_UNSIGNED_INT:
		return (uint16) sizeof(uint32);
	case GL_INT:
		return (uint16) sizeof(int32);
	case GL_FLOAT:
		return (uint16) sizeof(float);
	case GL_FLOAT_VEC2:
		return (uint16) sizeof(float) * 2;
	case GL_FLOAT_VEC3:
		return (uint16) sizeof(float) * 3;
	case GL_FLOAT_VEC4:
		return (uint16) sizeof(float) * 4;
	case GL_INT_VEC2:
		return (uint16) sizeof(int32) * 2;
	case GL_INT_VEC3:
		return (uint16) sizeof(int32) * 3;
	case GL_INT_VEC4:
		return (uint16) sizeof(int32) * 4;
	case GL_FLOAT_MAT3x4:
		return (uint16) sizeof(float) * 12;
	case GL_FLOAT_MAT4:
		return (uint16) sizeof(float) * 16;
	}
	return (uint16)0;
}

ParamDataType RenderContext_Base_GL::GetShaderParamType(GLuint type) {
	switch (type) {
	case GL_BOOL:
		return  ParamDataType::PDT_BOOL;
	case GL_UNSIGNED_INT:
		return  ParamDataType::PDT_UINT;
	case GL_INT:
		return  ParamDataType::PDT_INT;
	case GL_FLOAT:
		return  ParamDataType::PDT_FLOAT;
	case GL_FLOAT_VEC2:
		return  ParamDataType::PDT_VEC2;
	case GL_FLOAT_VEC3:
		return  ParamDataType::PDT_VEC3;
	case GL_FLOAT_VEC4:
		return  ParamDataType::PDT_VEC4;
	case GL_INT_VEC2:
		return  ParamDataType::PDT_IVEC2;
	case GL_INT_VEC3:
		return  ParamDataType::PDT_IVEC3;
	case GL_INT_VEC4:
		return  ParamDataType::PDT_IVEC4;
	case GL_FLOAT_MAT3x4:
		return  ParamDataType::PDT_MAT3x4;
	case GL_FLOAT_MAT4:
		return  ParamDataType::PDT_MAT4x4;
	}
	return  ParamDataType::PDT_UNKNOWN;
}

GLenum RenderContext_Base_GL::GetGlTextureType(TextureBase::TextureType type) {
	switch (type) {
	case TextureBase::TEXTURE_1D:
		return GL_TEXTURE_1D;
	case TextureBase::TEXTURE_1D_ARRAY:
		return GL_TEXTURE_1D_ARRAY;
	case TextureBase::TEXTURE_2D:
		return GL_TEXTURE_2D;
	case TextureBase::TEXTURE_2D_ARRAY:
		return GL_TEXTURE_2D_ARRAY;
	case TextureBase::TEXTURE_3D:
		return GL_TEXTURE_3D;
	case TextureBase::TEXTURE_CUBE_MAP:
		return GL_TEXTURE_CUBE_MAP;
	case TextureBase::TEXTURE_CUBE_MAP_ARRAY:
		return GL_TEXTURE_CUBE_MAP_ARRAY;
	}
	return 0;
}

GLenum RenderContext_Base_GL::GetGlBlendDataSource(BlendDataSource type) {
	switch (type) {
	case BDS_ZERO: return GL_ZERO;
	case BDS_SRC_ALPHA: return GL_SRC_ALPHA;
	case BDS_INV_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
	case BDS_SRC_COLOR: return GL_SRC_COLOR;
	case BDS_INV_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
	case BDS_DST_ALPHA: return GL_DST_ALPHA;
	case BDS_INV_DST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
	case BDS_DST_COLOR: return GL_DST_COLOR;
	case BDS_INV_DST_COLOR: return GL_ONE_MINUS_DST_COLOR;
	}
	return GL_ONE;
}

GLenum RenderContext_Base_GL::GetGlBlendEquation(BlendOp op) {
	switch (op) {
		// for GlBlendEquation or GlBlendEquationSeparate
	case BOP_SUB: return GL_FUNC_SUBTRACT;
	case BOP_INV_SUB: return GL_FUNC_REVERSE_SUBTRACT;
	case BOP_MIN: return GL_MIN;
	case BOP_MAX: return GL_MAX;
	}
	return GL_FUNC_ADD;
}

PixelFormatGl RenderContext_Base_GL::GetGlPixelFormat(PixelFormat imageFormat,
													  PixelFormat textureFormat) {
	NEX_ASSERT(PixelUtils::IsValidTextureFormat(textureFormat));
	PixelFormatGl pft;
	// todo Check for floating point components
	pft.isCompressed = false;
	pft.isDepthSencil = false;
	pft.internalFormat = GL_NONE;
	pft.sourceFormat = GL_NONE;
	pft.attachmentType = GL_COLOR_ATTACHMENT0;
	pft.textureFormat = textureFormat;
	switch (textureFormat) {
	case PixelFormat::R8:
		NEX_ASSERT(imageFormat == PixelFormat::R8);
		pft.internalFormat = GL_R8;
		pft.sourceFormat = GL_R8;
		pft.dataType = GL_UNSIGNED_BYTE;
		pft.pixelSize = 1;
		break;
	case PixelFormat::BGRA8:
		pft.internalFormat = GL_RGBA8;
		pft.sourceFormat = GL_BGRA;
#if (NEX_ENDIANNESS == NEX_ENDIAN_LITTLE)
		pft.dataType = GL_UNSIGNED_INT_8_8_8_8_REV;
#else
		pft.dataType = GL_UNSIGNED_INT_8_8_8_8;
#endif
		pft.pixelSize = 4;
		break;
	case PixelFormat::RGBA8:
		NEX_ASSERT(
			imageFormat == PixelFormat::RGBA8
			|| imageFormat == PixelFormat::BGRA8);
		pft.internalFormat = GL_RGBA8;
		pft.sourceFormat =
			imageFormat == PixelFormat::RGBA8 ? GL_RGBA : GL_BGRA;
#if (NEX_ENDIANNESS == NEX_ENDIAN_LITTLE)
		pft.dataType = GL_UNSIGNED_INT_8_8_8_8_REV;
#else
		pft.dataType = GL_UNSIGNED_INT_8_8_8_8;
#endif
		pft.pixelSize = 4;
		break;
	case PixelFormat::D16:
		NEX_ASSERT(imageFormat == PixelFormat::D16);
		pft.internalFormat = GL_DEPTH_COMPONENT16;
		pft.sourceFormat = GL_DEPTH_COMPONENT;
		pft.dataType = GL_FLOAT;
		pft.attachmentType = GL_DEPTH_ATTACHMENT;
		pft.isDepthSencil = true;
		pft.pixelSize = 2;
		break;
	case PixelFormat::D24:
		NEX_ASSERT(imageFormat == PixelFormat::D24);
		pft.internalFormat = GL_DEPTH_COMPONENT24;
		pft.sourceFormat = GL_DEPTH_COMPONENT;
		pft.dataType = GL_FLOAT;
		pft.attachmentType = GL_DEPTH_ATTACHMENT;
		pft.isDepthSencil = true;
		pft.pixelSize = 3;
		break;
	case PixelFormat::D24S8:
		NEX_ASSERT(imageFormat == PixelFormat::D24S8);
		pft.internalFormat = GL_DEPTH24_STENCIL8;
		pft.sourceFormat = GL_DEPTH_STENCIL;
		pft.dataType = GL_FLOAT; // irrelevant
		pft.attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
		pft.isDepthSencil = true;
		pft.pixelSize = 4;
		break;
	case PixelFormat::D32:
		NEX_ASSERT(imageFormat == PixelFormat::D32);
		pft.internalFormat = GL_DEPTH_COMPONENT32;
		pft.sourceFormat = GL_DEPTH_COMPONENT;
		pft.dataType = GL_FLOAT;
		pft.attachmentType = GL_DEPTH_ATTACHMENT;
		pft.isDepthSencil = true;
		pft.pixelSize = 4;
		break;
	case PixelFormat::RGBA16F:
		NEX_ASSERT(imageFormat == PixelFormat::RGBA16F);
		pft.internalFormat = GL_RGBA16F;
		pft.sourceFormat = GL_RGBA;
		pft.dataType = GL_FLOAT;
		pft.pixelSize = 8;
		break;
	case PixelFormat::RG16:
		NEX_ASSERT(imageFormat == PixelFormat::RG16);
		pft.internalFormat = GL_RG16;
		pft.sourceFormat = GL_RG16;
		pft.dataType = GL_UNSIGNED_SHORT;
		pft.pixelSize = 4;
		break;
	case PixelFormat::RG16_SNORM:
		NEX_ASSERT(imageFormat == PixelFormat::RG16_SNORM);
		pft.internalFormat = GL_RG16_SNORM;
		pft.sourceFormat = GL_RG16_SNORM;
		pft.dataType = GL_SHORT;
		pft.pixelSize = 4;
		break;
	}
	return pft;
}

ParameterContext RenderContext_Base_GL::GuessContextByName(const String& name,
														   ParameterContext defaultContex) {
	String lowerName = name;
	StringUtils::ToLower(lowerName);
	size_t p = lowerName.find("__");
	if (p != String::npos && p + 2 < lowerName.length()) {
		ParameterContext c = ShaderParameter::GetContextFromKey(lowerName[p + 2]);
		if (c >= 0 && c <= ParameterContext::CTX_COUNT)
			return c;
	}

	return defaultContex;
}

ContextID RenderContext_Base_GL::RegisterObject(ContextObject::Type type, uint32 hint) {
	const uint32 dynamicLayoutHint = (VertexLayout::HAS_TRANSIENT_BUFFERS |
									  VertexLayout::LOOSE_BUFFER_BINDING | VertexLayout::SHARED_VERTEX_LAYOUT);
	ContextObject::View* view = nullptr;
	switch (type) {
	case ContextObject::TYPE_TEXTURE:
		view = (NEX_NEW(TextureViewGL()));
		break;
	case ContextObject::TYPE_RENDER_TEXTURE:
		view = (NEX_NEW(RenderTextureViewGL()));
		break;
	case ContextObject::TYPE_MULTI_RENDER_TARGET:
		view = (NEX_NEW(MultiRenderTargetViewGL()));
		break;
	case ContextObject::TYPE_RENDER_BUFFER:
		view = (NEX_NEW(RenderBufferViewGL()));
		break;
	case ContextObject::TYPE_INDEX_BUFFER:
		view = (NEX_NEW(GpuBufferViewGL(GL_ELEMENT_ARRAY_BUFFER)));
		break;
	case ContextObject::TYPE_VERTEX_BUFFER:
		if (hint == (uint32)GpuBuffer::REGULARLY_RELEASED ||
			hint == (uint32)GpuBuffer::IMMEDIATELY_RELEASED)
			//@ todo Use transient buffer pool to reduce footprint
			view = (NEX_NEW(GpuTransientBufferViewGL(GL_ARRAY_BUFFER)));
		else
			view = (NEX_NEW(GpuBufferViewGL(GL_ARRAY_BUFFER)));
		break;
	case ContextObject::TYPE_VERTEX_LAYOUT:
		if (hint & dynamicLayoutHint) {
			if (extensions.GLE_ARB_vertex_attrib_binding)
				view = (NEX_NEW(VertexLayoutFlexibleGL()));
			else
				view = (NEX_NEW(VertexLayoutDynamicGL()));
		} else
			view = (NEX_NEW(VertexLayoutStaticGL()));
		break;
	case ContextObject::TYPE_PASS:
		view = NEX_NEW(PassViewGL());
		break;
	}
	view->Create(this);
	return reinterpret_cast<ContextID>(view);
}

void RenderContext_Base_GL::UnregisterObject(ContextID id) {
	if (id) {
		ContextObject::View* view = reinterpret_cast<ContextObject::View*>(id);
		view->Destroy(this);
		NEX_DELETE(view);
	}
}

void RenderContext_Base_GL::SetRasterState(const RasterStateGL& state) {
	if (state.frontIsCCW ^ rasterState.frontIsCCW) {
		static GLenum face[] = {
			GL_CW,
			GL_CCW
		};

		rasterState.frontIsCCW = state.frontIsCCW;
		glFrontFace(face[state.frontIsCCW]);
		GL_CHECK();
	}

	if (state.cullMode != rasterState.cullMode) {

		if (!state.cullMode) {
			glDisable(GL_CULL_FACE);
		} else {

			if (!rasterState.cullMode) {
				glEnable(GL_CULL_FACE);
			}
			glCullFace(state.cullMode);
			GL_CHECK();
		}
		rasterState.cullMode = state.cullMode;
	}

	if (state.depthClip != rasterState.depthClip && extensions.GLE_ARB_depth_clamp) {
		if (state.depthClip)
			glEnable(GL_DEPTH_CLAMP);
		else
			glDisable(GL_DEPTH_CLAMP);
		rasterState.depthClip = state.depthClip;
	}
	GL_CHECK();

	if (state.useScissors != rasterState.useScissors) {
		if (state.useScissors)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
		rasterState.useScissors = state.useScissors;
	}
	GL_CHECK();

	if (state.usingMultisample != rasterState.usingMultisample) {
		if (state.usingMultisample)
			glEnable(GL_MULTISAMPLE);
		else
			glDisable(GL_MULTISAMPLE);
		rasterState.usingMultisample = state.usingMultisample;
	}
	GL_CHECK();

	if (state.usingLineAA != rasterState.usingLineAA) {
		if (state.usingLineAA)
			glEnable(GL_LINE_SMOOTH);
		else
			glDisable(GL_LINE_SMOOTH);
		rasterState.usingLineAA = state.usingLineAA;
	}
	GL_CHECK();

	if (state.fillMode != rasterState.fillMode) {
		glPolygonMode(GL_FRONT_AND_BACK, state.fillMode);
		rasterState.fillMode = state.fillMode;
	}
	GL_CHECK();

	if (state.constantDepthBias != 0 || state.slopeScaledDepthBias != 0) {

		if (!rasterState.depthBiasEnabled) {
			glEnable(GL_POLYGON_OFFSET_FILL);
			GL_CHECK();
			glEnable(GL_POLYGON_OFFSET_POINT);
			GL_CHECK();
			glEnable(GL_POLYGON_OFFSET_LINE);
			GL_CHECK();
			rasterState.depthBiasEnabled = true;
		}

		if (state.depthBiasClamp && extensions.GlPolygonOffsetClampEXT) {
			extensions.GlPolygonOffsetClampEXT(-state.slopeScaledDepthBias, -state.constantDepthBias, state.depthBiasClamp);
		} else
			glPolygonOffset(-state.slopeScaledDepthBias, -state.constantDepthBias);
		GL_CHECK();
	} else if (rasterState.depthBiasEnabled) {
		glDisable(GL_POLYGON_OFFSET_FILL);
		GL_CHECK();
		glDisable(GL_POLYGON_OFFSET_POINT);
		GL_CHECK();
		glDisable(GL_POLYGON_OFFSET_LINE);
		GL_CHECK();
		rasterState.depthBiasEnabled = false;
	}
}

void RenderContext_Base_GL::SetDepthStencilState(const DepthStencilStateGL& state) {
	if (depthStencilState.depthTest != state.depthTest) {
		if (state.depthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		depthStencilState.depthTest = state.depthTest;
	}
	GL_CHECK();

	if (depthStencilState.depthWrite != state.depthWrite && depthStencilState.depthTest) {
		if (state.depthWrite)
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
		depthStencilState.depthWrite = state.depthWrite;
	}
	GL_CHECK();

	if (depthStencilState.depthTest && depthStencilState.depthCompare != state.depthCompare) {
		glDepthFunc(state.depthCompare);
		depthStencilState.depthCompare = state.depthCompare;
	}
	GL_CHECK();

	if (depthStencilState.stencilTest != state.stencilTest) {
		if (state.stencilTest) {
			glEnable(GL_STENCIL_TEST);

		} else {
			glDisable(GL_STENCIL_TEST);
		}

		depthStencilState.stencilTest = state.stencilTest;
	}
	GL_CHECK();

	if (depthStencilState.stencilTest) {
		if (state.stencilSeparateOp) {

			GlStencilMaskSeparate(GL_FRONT, state.stencilFrontMask);
			GL_CHECK();
			GlStencilFuncSeparate(GL_FRONT, state.stencilFrontFunc, state.stencilFrontRef,
								  state.stencilFrontMask);
			GL_CHECK();
			GlStencilOpSeparate(GL_FRONT, state.frontStencilFail,
								state.frontStencilPass, state.frontDepthPass);
			GL_CHECK();
			GlStencilMaskSeparate(GL_BACK, state.stencilBackMask);
			GL_CHECK();
			GlStencilFuncSeparate(GL_BACK, state.stencilBackFunc, state.stencilBackRef,
								  state.stencilBackMask);
			GL_CHECK();
			GlStencilOpSeparate(GL_BACK, state.backStencilFail,
								state.backStencilPass, state.backDepthPass);
		} else {

			glStencilMask(state.stencilFrontMask);
			GL_CHECK();
			glStencilFunc(state.stencilFrontFunc, state.stencilFrontRef,
						  state.stencilFrontMask);
			GL_CHECK();
			glStencilOp(state.frontStencilFail,
						state.frontStencilPass, state.frontDepthPass);
		}
	}
	GL_CHECK();
}

void RenderContext_Base_GL::SetBlendState(const BlendStateGL& state) {
	if ((blendState.enabled ^ state.enabled)) {
		if (state.enabled) {
			glEnable(GL_BLEND);
			blendState.enabled = true;
		} else {
			blendState.enabled = false;
			glDisable(GL_BLEND);
		}
	}
	if (blendState.enabled) {
		if (state.sameBlendOpAllTarget) {
			GlBlendFuncSeparate(state.blendOp[0].srcCol, state.blendOp[0].destCol,
								state.blendOp[0].srcAlpha, state.blendOp[0].destAlpha);
			GlBlendEquationSeparate(state.blendOp[0].colOp, state.blendOp[0].alphaOp);

		} else {
			for (uint8 i = 0; i < state.numRenderTargets; ++i) {

				GlBlendFuncSeparatei(i, state.blendOp[i].srcCol, state.blendOp[i].destCol,
									 state.blendOp[i].srcAlpha, state.blendOp[i].destAlpha);
				GlBlendEquationSeparate(state.blendOp[i].colOp, state.blendOp[i].alphaOp);
			}
			if (extensions.GLE_EXT_draw_buffers2) {
				for (uint8 i = 0; i < state.numRenderTargets; ++i) {
					if (state.blendOp[i].enabled)
						extensions.GlEnableIndexedEXT(GL_BLEND, i);
					else
						extensions.GlDisableIndexedEXT(GL_BLEND, i);
				}
			}
		}

		if (state.sameMaskAllTarget || !GlColorMaski) {
			ColorMask m = state.blendOp[0].mask;
			glColorMask((GLboolean)(m & ColorMask::MASK_RED),
						(GLboolean)(m & ColorMask::MASK_GREEN),
						(GLboolean)(m & ColorMask::MASK_BLUE),
						(GLboolean)(m & ColorMask::MASK_ALPHA));
		} else {
			for (uint8 i = 0; i < state.numRenderTargets; ++i) {
				ColorMask m = state.blendOp[i].mask;
				GlColorMaski(i,
							 (GLboolean)(m & ColorMask::MASK_RED),
							 (GLboolean)(m & ColorMask::MASK_GREEN),
							 (GLboolean)(m & ColorMask::MASK_BLUE),
							 (GLboolean)(m & ColorMask::MASK_ALPHA));
			}
		}
	}
}

void RenderContext_Base_GL::InitializeExtensions() {
	extensions.InitializeFunctionPointers();
	FunctionTable::InitializeFunctionPointers();
}

void RenderContext_Base_GL::CloseImpl() {
}

void RenderContext_Base_GL::DestroyResources(void* pThis) {
	reinterpret_cast<RenderContext_Base_GL*>(pThis)->DestroyResources();
}

void RenderContext_Base_GL::DestroyResources() {
	uniformBufferPool.Destroy(this);
}

RenderWindow* RenderContext_Base_GL::CreateRenderWindowImpl() {
	return GetImpl()->CreateWindowImpl();
}

void RenderContext_Base_GL::ReportError(GLenum source,
										GLenum type,
										GLuint id,
										GLenum severity,
										GLsizei length,
										const GLchar* message) {
	OutStringStream stream;
	stream << "OpenGL Error: source=";
	switch (type) {
	case GL_DEBUG_SOURCE_API:
		stream << "GL"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		stream << "Window system"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		stream << "Shader compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		stream << "Third party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:
		stream << "Application"; break;
	case GL_DEBUG_SOURCE_OTHER:
	default:
		stream << "Other"; break;
	}

	stream << ", severity=";
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		stream << "High, "; break;
	case GL_DEBUG_SEVERITY_LOW:
		stream << "Low, "; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		stream << "Info, "; break;
	case GL_DEBUG_SEVERITY_MEDIUM:
	default:
		stream << "Medium, "; break;
	}

	String msg((const char*)message, length);
	stream << msg;
	Error(stream.str());
}

VertexSemanticDataGL RenderContext_Base_GL::GetInputSemanticsDataFromID(VertexSemanticID id) {
	VertexSemanticDataGL data;
	VertexSemanticListElement& vsle = registeredSignatures[id.indexIntoRegisteredList];
	data.rawArray = &vertexSemanticBlob[vsle.indexIntoBlobList];
	data.count = vsle.count;
	return data;
}

VertexSemanticID RenderContext_Base_GL::MapLayout(
	const VertexSemanticGL* semantics, uint32 numSemantics) {
	// @urgent Make thread safe
	uint16 index = 0;
	for (auto& it : registeredSignatures) {
		if (it.count == numSemantics) {
			VertexSemanticGL* l = &vertexSemanticBlob[it.indexIntoBlobList];
			if (std::equal(l, l + it.count, semantics))
				return VertexSemanticID(index);
		}
		index++;
	}
	
	registeredSignatures.push_back({ (uint16)vertexSemanticBlob.size(), (uint16)numSemantics });
	vertexSemanticBlob.insert(vertexSemanticBlob.end(), semantics, semantics + numSemantics);

	return VertexSemanticID(index);
}

}
