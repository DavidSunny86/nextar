/*
 * RenderContextGL.cpp
 *
 *  Created on: 17-Aug-2013
 *      Author: obhi
 */
#include <RenderOpenGL.h>
#include <RenderContextGL.h>
#include <RenderDriverGL.h>
#include <RenderWindow.h>
#include <ShaderAsset.h>
#include <PassGL.h>

namespace RenderOpenGL {

	RenderContextGL::RenderContextGL(RenderDriverGL* _driver) : BaseRenderContext(_driver),
			contextFlags(0) {
	}

	RenderContextGL::~RenderContextGL() {

	}

	RenderWindow* RenderContextGL::CreateRenderWindowImpl() {
		return CreateWindowImpl();
	}

	void RenderContextGL::PostWindowCreation(RenderWindow* gw) {
		/* check if extensions were initialized or initialize extensions */
		if (!IsContextReady()) {
			ReadyContext(gw);
			contextFlags |= CONTEXT_READY;

			if (!AreExtensionsReady()) {
				_ReadyExtensions(contextCreationParams.reqOpenGLVersionMajor,
						contextCreationParams.reqOpenGLVersionMinor);
				contextFlags |= EXTENSIONS_READY;
			}
		}
	}

	void RenderContextGL::PostWindowDestruction(RenderWindow* gw) {
		/* do nothing */
	}

	void RenderContextGL::CreateImpl(const RenderDriver::ContextCreationParams& ctxParams) {
		SetCreationParams(ctxParams);
	}

	GLuint RenderContextGL::CreateShader(GLenum type, const char* preDef, const char* source) {
		const GLchar* sourceBuff[2] = {0};
		sourceBuff[0] = static_cast<const GLchar*>(preDef ? preDef : "");
		sourceBuff[1] = static_cast<const GLchar*>(source);
		GLuint shaderObject = GlCreateShader(type);
		GL_CHECK();
		GlShaderSource(shaderObject, 2, sourceBuff, NULL);
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
			GlGetShaderiv(shaderObject, GL_COMPILE_STATUS, &infoLogLength);
			GL_CHECK();
			GLchar* infoLog = static_cast<GLchar*>(NEX_ALLOC(infoLogLength+2, MEMCAT_GENERAL));
			GlGetShaderInfoLog(shaderObject, infoLogLength+1, NULL, infoLog);
			GL_CHECK();
			Error(String("Shader compilation failed: ") + infoLog);
			NEX_FREE(infoLog, MEMCAT_GENERAL);
			GlDeleteShader(shaderObject);
			shaderObject = 0;
		}
		return shaderObject;
	}

	void RenderContextGL::DestroyShader(GLuint sh) {
		GlDeleteShader(sh);
		GL_CHECK();
	}

	GLuint RenderContextGL::CreateProgram(GLuint shaders[]) {
		/* create the program object */
		GLuint program = GlCreateProgram();
		for(nextar::uint32 i = 0; i < Pass::NUM_STAGES; ++i) {
			if(shaders[i]) {
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
				infoLog = static_cast<GLchar*> (NEX_ALLOC(sizeof (GLchar) * infoLogLen, MEMCAT_GENERAL));
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

	void RenderContextGL::DestroyProgram(GLuint program) {
		GlDeleteProgram(program);
		GL_CHECK();
	}

	uint32 RenderContextGL::ReadProgramSemantics(GLuint program,
			VertexSemanticGL* inpComp
			) {

		GLint numAttribs = 0;
		// outputSignature = 0;
		
		char attribName[64];
		size_t inpCount = 0;
		size_t outCount = 0;

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
				Warn("Unmatched input semantic.");
				return 0;
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
		return inpCount;
	}

	void RenderContextGL::ReadUniforms(PassGL* pass, GLuint program) {
		GLint numBlocks = 0;
		GlGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
		char name[128];

		if(numBlocks > RenderConstants::MAX_CBUFFER_PER_PASS) {
			Error("Pass has too many constant buffers!.");
			NEX_THROW_GracefulError(EXCEPT_COMPILATION_FAILED);
		}

		Pass::ConstantBufferList ubList;

		for (GLint i = 0; i < numBlocks; ++i) {
			GLint size = 0;
			GLint numParams = 0;
			GlGetActiveUniformBlockName(program,
					i, 128, NULL, name);
			GlGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
			GlGetActiveUniformBlockiv(program, i,
					GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,
					&numParams);
			//NEX_ASSERT(numParams < MAX_PARAMS);
			if (numParams == 0) {
				continue;
			}
			String uniName = name;
			//bool shareUb = true;
			UniformBufferGL* ubPtr = nullptr;
			ConstantBufferPtr cbPtr;
			UniformBufferMap::iterator it = uniformBufferMap.find(uniName);
			if (it != uniformBufferMap.end()) {
				cbPtr = (*it).second;
				ubPtr = static_cast<UniformBufferGL*>(cbPtr.GetPtr());
				if ( !(ubPtr && ubPtr->GetParamCount() == numParams &&
						ubPtr->GetSize() == size) ) {
					Warn(String("Uniform buffer cannot be registered"
								" (name already registered with different contents): ") + uniName);
					ubPtr = 0;
					// we can surely register it as unshared ub
					continue;
				}
			}

			if (!cbPtr) {
				// create a new uniform buffer
				ubPtr = CreateUniformBuffer(pass, i, program, numParams, size);
				ubPtr->name = uniName;
				cbPtr = nextar::Bind<ConstantBuffer>(ubPtr);
				uniformBufferMap.emplace(std::cref(ubPtr->name),
						cbPtr).first;
				ubPtr->SetBinding((GLuint)uniformBufferMap.size());
			}

			ubList[i] = cbPtr;
			// sort ublist by name
			GlBindBufferRange(GL_UNIFORM_BUFFER, ubPtr->GetBinding(), ubPtr->ubNameGl, 0, ubPtr->size);
			GlUniformBlockBinding(program, i, ubPtr->GetBinding());
		}
		std::sort(ubList.data(), ubList.data() + numBlocks,
				[](const UniformBufferGL* first, const UniformBufferGL* second) {
			if(first->GetFrequency() == second->GetFrequency())
				return first->GetName() < second->GetName();
			return (first->GetFrequency() < second->GetFrequency()) != 0;
		});

		pass->numConstBuffers = numBlocks;
		pass->sharedParameters.swap(ubList);
	}

	UniformBufferGL* RenderContextGL::CreateUniformBuffer(PassGL* pass, GLint blockIndex, GLuint prog,
			GLuint numParams, size_t size) {

		NEX_ASSERT(size > 0);
		uint16 numUnmappedParams = 0;

		UniformBufferGL* pUb = NEX_NEW(UniformBufferGL);
		UniformBufferGL& u = *pUb;
		u.ubNameGl = GL_INVALID_VALUE;
		GlGenBuffers(1, &u.ubNameGl);
		GL_CHECK();
		if (u.ubNameGl == GL_INVALID_VALUE) {
			Error("Failed to generate buffer name");
			NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
		}

		UpdateFrequency mask = (UpdateFrequency)0;
		UniformGL* uniforms = NEX_ALLOC_ARRAY_T(UniformGL, MEMCAT_GENERAL, numParams);
		void* tempBuffer = NEX_ALLOC(sizeof(GLint)*numParams*7 + 128, MEMCAT_GENERAL);
		GLint *indices = static_cast<GLint*>(tempBuffer) + numParams*0;
		GLint *type = static_cast<GLint*>(tempBuffer) + numParams*1;
		GLint *offset = static_cast<GLint*>(tempBuffer) + numParams*2;
		GLint *arraynum = static_cast<GLint*>(tempBuffer) + numParams*3;
		GLint *rowMaj = static_cast<GLint*>(tempBuffer) + numParams*4;
		GLint *matStride = static_cast<GLint*>(tempBuffer) + numParams*5;
		GLint *arrayStride = static_cast<GLint*>(tempBuffer) + numParams*6;
		char *uniName = reinterpret_cast<char*>(static_cast<GLint*>(tempBuffer) + numParams*7);

		const GLuint* uindices = (GLuint*)indices;
		//GLuint blockIndex = GlGetUniformBlockIndex(prog, (const GLchar*)name.c_str());
		GlGetActiveUniformBlockiv(prog, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices);
		GL_CHECK();
		GlGetActiveUniformsiv(prog, numParams, uindices, GL_UNIFORM_TYPE, type);
		GL_CHECK();
		GlGetActiveUniformsiv(prog, numParams, uindices, GL_UNIFORM_OFFSET, offset);
		GL_CHECK();
		GlGetActiveUniformsiv(prog, numParams, uindices, GL_UNIFORM_SIZE, arraynum);
		GL_CHECK();
		GlGetActiveUniformsiv(prog, numParams, uindices, GL_UNIFORM_IS_ROW_MAJOR, rowMaj);
		GL_CHECK();
		GlGetActiveUniformsiv(prog, numParams, uindices, GL_UNIFORM_MATRIX_STRIDE, matStride);
		GL_CHECK();
		GlGetActiveUniformsiv(prog, numParams, uindices, GL_UNIFORM_ARRAY_STRIDE, arrayStride);
		GL_CHECK();


		uint32 numCustomParams = 0;
		for (GLint i = 0; i < (GLint)numParams; ++i) {
			UniformGL& uform = uniforms[i];
			GlGetActiveUniformName(prog, indices[i], 128, 0, uniName);
			String name = uniName;
			GL_CHECK();
			uform.isRowMajMatrix = rowMaj[i] ? true : false;
			uform.typeGl = uint8(type[i]);
			uform.matrixStride = matStride[i];
			uform.arrayStride = arrayStride[i];
			uform.constBufferDesc.cbOffset = offset[i];
			uform.constBufferDesc.paramDesc.arrayCount = uint16(arraynum[i]);
			uform.constBufferDesc.paramDesc.size =
					GetShaderParamSize(uform.typeGl) * uform.constBufferDesc.paramDesc.arrayCount;
			uform.constBufferDesc.paramDesc.type = GetShaderParamType(uform.typeGl);

			// todo Make all possible shader paramter auto param/custom param
			// and register them to this table, thus a valid pointer is always
			// retrieved. For custom param, the processor will be set to custom
			// processor which knows how to retrieve from the parameter buffer.
			// Later on when the shader script is properly made, the script will
			// read the parameters from the script and automatically add them
			// to the mapped list for retrieval here.
			const AutoParam* paramDef = pass->MapParam(name);
			// if this is not an auto param, we
			// use a custom processor
			if (paramDef == nullptr) {
				numCustomParams++;
				// todo There are a lot of assumptions here. We assume, all parameters
				// are material parameters. They could well be any other type custom
				// parameter (read Object).
				uform.constBufferDesc.paramDesc.autoName = AutoParamName::AUTO_CUSTOM_CONSTANT;
				uform.constBufferDesc.paramDesc.name = std::move(name);
				uform.constBufferDesc.paramDesc.frequency = UpdateFrequency::PER_MATERIAL;
				uform.constBufferDesc.paramDesc.processor = Pass::customConstantProcessorMaterial;
			} else {
				uform.constBufferDesc.paramDesc.autoName = paramDef->autoName;
				uform.constBufferDesc.paramDesc.frequency = paramDef->frequency;
				uform.constBufferDesc.paramDesc.processor = paramDef->processor;
				NEX_ASSERT(uform.constBufferDesc.paramDesc.type == paramDef->type);
			}

			mask |= uform.constBufferDesc.paramDesc.frequency;
		}

		// sort the resulting uniforms such that auto params come first
		// followed by unmapped params. Unmapped params are sorted by name.
		std::sort(uniforms, uniforms + numParams,
				[] (const UniformGL& first, const UniformGL& second) {
					if (first.constBufferDesc.paramDesc.autoName == second.constBufferDesc.paramDesc.autoName) {
						NEX_ASSERT(first.constBufferDesc.paramDesc.autoName == AutoParamName::AUTO_CUSTOM_CONSTANT);
						return first.constBufferDesc.cbOffset < first.constBufferDesc.cbOffset;
					}
					return (first.constBufferDesc.paramDesc.autoName <
							second.constBufferDesc.paramDesc.autoName) != 0;
			}
		);
		if (numCustomParams == numParams) {
			u.SetFlags(ConstantBuffer::CUSTOM_STRUCT);
			u.processor = Pass::customStructProcessorMaterial;
		}
		u.paramCount = numParams;
		u.paramDesc = &uniforms->constBufferDesc;
		u.frequency = mask;
		u.size = size;
		NEX_FREE(tempBuffer, MEMCAT_GENERAL);
		GlBindBuffer( GL_UNIFORM_BUFFER, u.ubNameGl );
		GL_CHECK();
		GlBufferData( GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW );
		GL_CHECK();
		GlBindBuffer( GL_UNIFORM_BUFFER, 0 );
		GL_CHECK();

		return pUb;
	}

	void RenderContextGL::ReadSamplers(PassGL* pass, GLuint program) {
		/** todo Massive work left for sampler arrays */
		GLint numUni = 0;
		char name[128];
		size_t extra = 0;
		GlGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUni);
		// assume all are samplers, otherwise we got a problem
		SamplerState* samplers = NEX_ALLOC_ARRAY_T(SamplerState, MEMCAT_GRAPHICS, numUni);
		uint32 mapped = 0;
		for (GLuint i = 0; i < (GLuint) numUni; ++i) {

			GlGetActiveUniformName(program, i, 128, 0, name);
			GLint loc = GlGetUniformLocation(program, name);
			String unitName = name;
			GLint type;

			GlGetActiveUniformsiv(program, 1, &i, GL_UNIFORM_TYPE, &type);

			GL_CHECK();
			if (IsSamplerType(type)) {

				SamplerState& ss = samplers[mapped];
				ss.index = mapped++;
				ss.desc.paramDesc.arrayCount = 1;
				const Pass::SamplerDesc& tu = *pass->MapSamplerParams(unitName);
				if (&tu == nullptr) {
					Error(String("Overflowing/Unspecified texture unit index for ") + name);
					continue;
				}
				const AutoParam* paramDef = pass->MapParam(unitName);
				ss.location = loc;

				if (paramDef == nullptr) {
					ss.desc.paramDesc.autoName = AutoParamName::AUTO_CUSTOM_CONSTANT;
					ss.desc.paramDesc.name = std::move(unitName);
					ss.desc.paramDesc.frequency = UpdateFrequency::PER_MATERIAL;
					ss.desc.paramDesc.processor = Pass::customTextureProcessorMaterial;
				} else {
					ss.desc.paramDesc.autoName = paramDef->autoName;
					ss.desc.paramDesc.frequency = paramDef->frequency;
					ss.desc.paramDesc.processor = paramDef->processor;
				}
				ss.desc.paramDesc.size = sizeof(SamplerState);
				ss.desc.defaultTexture.texture = tu.defaultTexture;
				const TextureUnitParams& params = tu.texUnitParams;

				GlGenSamplers(1, &ss.sampler);
				GL_CHECK();
				if (ss.sampler) {
					GlSamplerParameteri(ss.sampler, GL_TEXTURE_MIN_FILTER, GetGlMinFilter(params.minFilter));
					GL_CHECK();
					GlSamplerParameteri(ss.sampler, GL_TEXTURE_MAG_FILTER, GetGlMagFilter(params.magFilter));
					GL_CHECK();
					GlSamplerParameteri(ss.sampler, GL_TEXTURE_WRAP_S, GetGlAddressMode(params.uAddress));
					GL_CHECK();
					GlSamplerParameteri(ss.sampler, GL_TEXTURE_WRAP_T, GetGlAddressMode(params.vAddress));
					GL_CHECK();
					GlSamplerParameteri(ss.sampler, GL_TEXTURE_WRAP_R, GetGlAddressMode(params.wAddress));
					GL_CHECK();
				}
				if (params.comparisonFunc) {
					GlSamplerParameteri(ss.sampler, GL_TEXTURE_COMPARE_FUNC, GetGlCompareFunc(params.comparisonFunc));
					GL_CHECK();
					GlSamplerParameteri(ss.sampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
					GL_CHECK();
				} else {
					GlSamplerParameteri(ss.sampler, GL_TEXTURE_COMPARE_MODE, GL_NONE);
					GL_CHECK();
				}
				GlSamplerParameterf(ss.sampler, GL_TEXTURE_LOD_BIAS, params.lodBias);
				GL_CHECK();
				GlSamplerParameterf(ss.sampler, GL_TEXTURE_MIN_LOD, params.minLod);
				GL_CHECK();
				GlSamplerParameterf(ss.sampler, GL_TEXTURE_MAX_LOD, params.maxLod);
				GL_CHECK();
				GlSamplerParameteri(ss.sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, params.maxAnisotropy);
				GL_CHECK();
				GlSamplerParameterfv(ss.sampler, GL_TEXTURE_BORDER_COLOR, params.borderColor.AsFloatArray());
				GL_CHECK();
			}
			std::sort(samplers, samplers + mapped, [](const SamplerState& s1, const SamplerState& s2) {
				return s1.desc.paramDesc.autoName == s2.desc.paramDesc.autoName ? s1.desc.paramDesc.name < s2.desc.paramDesc.name :
						((s1.desc.paramDesc.autoName < s2.desc.paramDesc.autoName) != 0);
			});
		}
	}

	void RenderContextGL::Capture(PixelBox& image,
			RenderTarget* rt,
			GLuint *pbo,
			FrameBuffer frameBuffer) {
		/* todo Expand this function for frame buffer,
		 * depth map capture etc. */
		GLenum readTargets[] = {
				GL_FRONT_LEFT,
				GL_FRONT_RIGHT,
				GL_BACK_LEFT,
				GL_BACK_RIGHT
		};

		Size size = rt->GetDimensions();
		// assuming color buffer capture
		size_t dataSize = size.dx*size.dy*4;

		if (!image.Data()) {
			image.Data() = (NEX_ALLOC(dataSize, MEMCAT_GENERAL));
			image.deleteData = true;
		}

		bool asyncCapture =
				RenderManager::Instance().GetRenderSettings().asyncCapture;

		glReadBuffer(readTargets[frameBuffer]);
		if (asyncCapture) {
			uint32 next = RenderConstants::MAX_FRAME_PRE_CAPTURE-1;
			if (!pbo[0]) {
				// need to create pbo, and this is our first go
				next = 0;
				GlGenBuffers(RenderConstants::MAX_FRAME_PRE_CAPTURE, pbo);
				GL_CHECK();
				for(uint32 i = 0; i < RenderConstants::MAX_FRAME_PRE_CAPTURE; ++i) {
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
			for (uint32 i = 0; i < RenderConstants::MAX_FRAME_PRE_CAPTURE-1; ++i)
				pbo[i] = pbo[i+1];
			pbo[RenderConstants::MAX_FRAME_PRE_CAPTURE-1] = cur;
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
		
	void RenderContextGL::Draw(StreamData* streamData, CommitContext& ctx) {
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
		VertexLayoutGL* layout = static_cast<VertexLayoutGL*> (
			vd.layout);
		VertexBufferBinding& binding = (*vd.binding);
		layout->Enable(binding, static_cast<PassGL*>(ctx.pass), this);

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
		}

		if (streamData->useIndices) {
			IndexBufferGL* buff = static_cast<IndexBufferGL*> (
				streamData->indices.indices.GetPtr());

			GLenum indextype;
			GLint indexsize;
			if (buff->GetIndexType() == IndexBuffer::TYPE_16BIT) {
				indexsize = 2;
				indextype = GL_UNSIGNED_SHORT;
			} else {
				indexsize = 4;
				indextype = GL_UNSIGNED_INT;
			}

			GlBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
				buff->GetBufferId());
			GL_CHECK();
			if (vd.start == 0) {
				if (streamData->instanceCount == 1)
					glDrawElements(primtype, streamData->indices.count, indextype,
					(const GLvoid*) (indexsize * streamData->indices.start));
				else
					GlDrawElementsInstanced(primtype,
					streamData->indices.count, indextype,
					(const GLvoid*) (indexsize * streamData->indices.start),
					streamData->instanceCount);
			} else {
				if (streamData->instanceCount == 1)
					GlDrawElementsBaseVertex(primtype,
					streamData->indices.count, indextype,
					(const GLvoid*) (indexsize * streamData->indices.start),
					vd.start);
				else
					GlDrawElementsInstancedBaseVertex(primtype,
					streamData->indices.count, indextype,
					(const GLvoid*) (indexsize * streamData->indices.start),
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
		if (streamData->isGsdataValid) {
			GlBindVertexArray(0);
		}
	}
	
	void RenderContextGL::SetCurrentTarget(RenderTarget* canvas) {
		if (canvas == nullptr)
			SetCurrentWindow(nullptr);
		else {
			switch (canvas->GetRenderTargetType()) {
			case RenderTargetType::RENDER_TEXTURE:
			case RenderTargetType::RENDER_WINDOW:
				SetCurrentWindow(canvas);
				break;
			case RenderTargetType::MULTI_RENDER_TARGET:
			}
		}
	}

	void RenderContextGL::Clear(Color& c, float depth, uint16 stencil, ClearFlags flags) {
		GLbitfield mask = 0;
		if (Test(flags & ClearFlags::CLEAR_COLOR)) {
			mask |= GL_COLOR_BUFFER_BIT;
			glClearColor(c.red, c.green, c.blue, c.alpha);
		}
		if (Test(flags & ClearFlags::CLEAR_DEPTH)) {
			mask |= GL_DEPTH_BUFFER_BIT;
			glClearDepth(depth);
		}
		if (Test(flags & ClearFlags::CLEAR_STENCIL)) {
			mask |= GL_STENCIL_BUFFER_BIT;
			glClearStencil(stencil);
		}
		glClear(mask);
		GL_CHECK();
	}

	void RenderContextGL::SetActivePass(Pass* pass) {
		PassGL* passGl = static_cast<PassGL*>(pass);
		GlUseProgram(passGl->GetProgram());
		GL_CHECK();
	}

	GLuint RenderContextGL::CreateVAO(VertexBufferBinding& binding,
			VertexAttribListGL& attributes,
			const VertexSemanticListGL& semantics) {

		uint16 outElements[RenderConstants::MAX_VERTEX_ELEMENT];
		uint32 numMapped = BufferManager::MapSignatureToSemantics(
				&semantics[0].semantic, sizeof(VertexSemanticGL), semantics.size(),
				&attributes[0].element, sizeof(VertexAttribGL), attributes.size(),
				outElements);

		if (numMapped) {
			uint16 stream = -1;
			GLuint vao;
			GlGenVertexArrays(1, &vao);
			GL_CHECK();
			GlBindVertexArray(vao);
			GL_CHECK();
			for(uint32 j = 0; j < numMapped; ++j) {
				NEX_ASSERT(semantics[j].index < (uint16)-1);
				VertexAttribGL& vegl = attributes[outElements[j]];
				if( vegl.element.streamIndex != stream ) {
					stream = vegl.element.streamIndex;
					SetVertexBuffer(binding.GetBuffer(stream));
				}
				EnableVertexAttribute(semantics[j].index, vegl);

			}
			return vao;
		}
		return 0;
	}

	GLuint RenderContextGL::CreateVertexBuffer(size_t size, GLenum usage) {
		GLuint bufferId = 0;
		GlGenBuffers(1, &bufferId);
		if (!bufferId) {
			Error("Failed to create index buffer");
			NEX_THROW_GracefulError(EXCEPT_OUT_OF_RESOURCES);
		}
		GlBindBuffer(GL_ARRAY_BUFFER, bufferId);
		GlBufferData(GL_ARRAY_BUFFER, size, NULL, usage);
		GlBindBuffer(GL_ARRAY_BUFFER, 0);
		return bufferId;
	}

	GLuint RenderContextGL::CreateIndexBuffer(size_t size, GLenum usage) {
		GLuint bufferId = 0;
		GlGenBuffers(1, &bufferId);
		if( !bufferId ) {
			Error("Failed to create index buffer");
			NEX_THROW_GracefulError(EXCEPT_OUT_OF_RESOURCES);
		}
		GlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
		GlBufferData(GL_ELEMENT_ARRAY_BUFFER, size, NULL, usage);
		GlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return bufferId;
	}
		
	void RenderContextGL::CopyBuffer(GLuint src, GLuint dest, size_t size) {
		GlBindBuffer(GL_COPY_READ_BUFFER, src);
		GlBindBuffer(GL_COPY_WRITE_BUFFER, dest);
		GlCopyBufferSubData(GL_COPY_READ_BUFFER,
			GL_COPY_WRITE_BUFFER,
			0, 0, size);
		GL_CHECK();
		GlBindBuffer(GL_COPY_READ_BUFFER, 0);
		GlBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	}

	void RenderContextGL::AllocateTexture(GLenum target, GLint levels,
			GLenum format, GLsizei width, GLsizei height, GLsizei depth) {
		switch(target) {
		case GL_TEXTURE_1D:
			GlTexStorage1D(target, levels, format, width);
			break;
		case GL_TEXTURE_2D:
		case GL_TEXTURE_RECTANGLE:
		case GL_TEXTURE_CUBE_MAP:
		case GL_TEXTURE_1D_ARRAY:
			GlTexStorage2D(target, levels, format, width, height);
			break;
		case GL_TEXTURE_3D:
		case GL_TEXTURE_2D_ARRAY:
		case GL_TEXTURE_CUBE_MAP_ARRAY:
			GlTexStorage3D(target, levels, format, width, height, depth);
			break;
		}
		GL_CHECK();
	}

	void RenderContextGL::AllocateTextureLevel(GLenum target, GLint level,
			const PixelFormatGl&  format,	GLsizei width,
			GLsizei height, GLsizei depth, const uint8* data, size_t size) {
		switch(target) {
		case GL_TEXTURE_1D:
			if (format.isCompressed)
				glCompressedTexImage1D(target, level, format.internalFormat, width, 0, size, data);
			else
				glTexImage1D(target, level, format.internalFormat, width, 0, format.sourceFormat, format.dataType, data);
			break;
		case GL_TEXTURE_2D:
		case GL_TEXTURE_RECTANGLE:
		case GL_TEXTURE_CUBE_MAP:
		case GL_TEXTURE_1D_ARRAY:
			if (format.isCompressed)
				glCompressedTexImage2D(target, level, format.internalFormat, width, height, 0, size, data);
			else
				glTexImage2D(target, level, format.internalFormat, width, height, 0, format.sourceFormat, format.dataType, data);
			break;
		case GL_TEXTURE_3D:
		case GL_TEXTURE_2D_ARRAY:
		case GL_TEXTURE_CUBE_MAP_ARRAY:
			if (format.isCompressed)
				glCompressedTexImage3D(target, level, format.internalFormat, width, height, depth, 0, size, data);
			else
				glTexImage3D(target, level, format.internalFormat, width, height, depth, 0, format.sourceFormat, format.dataType, data);
			break;
		}
		GL_CHECK();
	}

	void RenderContextGL::WriteTextureLevel(GLenum target, GLint level,
			const PixelFormatGl&  format, GLsizei width,
			GLsizei height, GLsizei depth, const uint8* data, size_t size) {
		// todo Not sure if it will work for compressed formats
		switch(target) {
		case GL_TEXTURE_1D:
				glTexSubImage1D(target, level, 0, width, format.sourceFormat, format.dataType, data);
			break;
		case GL_TEXTURE_2D:
		case GL_TEXTURE_RECTANGLE:
		case GL_TEXTURE_CUBE_MAP:
		case GL_TEXTURE_1D_ARRAY:
				glTexSubImage2D(target, level, 0, 0, width, height, format.sourceFormat, format.dataType, data);
			break;
		case GL_TEXTURE_3D:
		case GL_TEXTURE_2D_ARRAY:
		case GL_TEXTURE_CUBE_MAP_ARRAY:
				glTexSubImage3D(target, level, 0, 0, 0, width, height, depth, format.sourceFormat, format.dataType, data);
			break;
		}
		GL_CHECK();

	}

	bool RenderContextGL::IsSamplerType(GLint type) {
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

	GLint RenderContextGL::GetGlMinFilter(TextureMinFilterType t) {
		switch(t) {
			case TF_MIN_NEAREST: return GL_NEAREST;
			case TF_MIN_LINEAR: return GL_LINEAR;
			case TF_MIN_NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
			case TF_MIN_LINEAR_MIPMAP_NEAREST: return GL_LINEAR_MIPMAP_NEAREST;
			case TF_MIN_NEAREST_MIPMAP_LINEAR: return GL_NEAREST_MIPMAP_LINEAR;
			case TF_MIN_LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
		}
		return GL_LINEAR;
	}

	GLint RenderContextGL::GetGlMagFilter(TextureMinFilterType t) {
		switch(t) {
			case TF_MAG_NEAREST: return GL_NEAREST;
			case TF_MAG_LINEAR: return GL_LINEAR;
		}
		return GL_LINEAR;
	}

	GLint RenderContextGL::GetGlAddressMode(TextureAddressModeType t) {
		switch(t) {
			case TAM_BORDER: return GL_CLAMP_TO_BORDER;
			case TAM_CLAMP: return GL_CLAMP_TO_EDGE;
			case TAM_MIRROR: return GL_MIRRORED_REPEAT;
			case TAM_WRAP: return GL_REPEAT;
		}
		return GL_REPEAT;
	}

	GLenum RenderContextGL::GetGlCompareFunc(TextureComparisonModeType type) {
		switch(type) {
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

	uint16 RenderContextGL::GetSemanticType(GLenum e) {
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
		return -1;
	}

	uint16 RenderContextGL::GetShaderParamSize(GLuint type) {
		switch(type) {
			case GL_BOOL: return (uint16)1;
			case GL_UNSIGNED_INT: return (uint16)sizeof(uint32);
			case GL_INT: return (uint16)sizeof(int32);
			case GL_FLOAT: return (uint16)sizeof(float);
			case GL_FLOAT_VEC2: return (uint16)sizeof(float)*2;
			case GL_FLOAT_VEC3: return (uint16)sizeof(float)*3;
			case GL_FLOAT_VEC4: return (uint16)sizeof(float)*4;
			case GL_INT_VEC2: return (uint16)sizeof(int32)*2;
			case GL_INT_VEC3: return (uint16)sizeof(int32)*3;
			case GL_INT_VEC4: return (uint16)sizeof(int32)*4;
			case GL_FLOAT_MAT3x4: return (uint16)sizeof(float)*12;
			case GL_FLOAT_MAT4: return (uint16)sizeof(float)*16;
		}
		return (uint16)0;
	}

	uint16 RenderContextGL::GetShaderParamType(GLuint type) {
		switch(type) {
			case GL_BOOL: return (uint16)ParamDataType::PDT_BOOL;
			case GL_UNSIGNED_INT: return (uint16)ParamDataType::PDT_UINT;
			case GL_INT: return (uint16)ParamDataType::PDT_INT;
			case GL_FLOAT: return (uint16)ParamDataType::PDT_FLOAT;
			case GL_FLOAT_VEC2: return (uint16)ParamDataType::PDT_VEC2;
			case GL_FLOAT_VEC3: return (uint16)ParamDataType::PDT_VEC3;
			case GL_FLOAT_VEC4: return (uint16)ParamDataType::PDT_VEC4;
			case GL_INT_VEC2: return (uint16)ParamDataType::PDT_IVEC2;
			case GL_INT_VEC3: return (uint16)ParamDataType::PDT_IVEC3;
			case GL_INT_VEC4: return (uint16)ParamDataType::PDT_IVEC4;
			case GL_FLOAT_MAT3x4: return (uint16)ParamDataType::PDT_MAT3x4;
			case GL_FLOAT_MAT4: return (uint16)ParamDataType::PDT_MAT4x4;
		}
		return (uint16)ParamDataType::PDT_UNKNOWN;
	}

	GLenum RenderContextGL::GetGlTextureType(TextureBase::TextureType type) {
		switch(type) {
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

	RenderContextGL::PixelFormatGl RenderContextGL::GetGlPixelFormat(PixelFormat imageFormat, PixelFormat textureFormat) {
		NEX_ASSERT(PixelUtils::IsValidTextureFormat(textureFormat));
		PixelFormatGl pft;
		// todo Check for floating point components
		pft.isCompressed = false;
		pft.internalFormat = GL_NONE;
		pft.sourceFormat = GL_NONE;
		switch(textureFormat) {
		case PixelFormat::R8:
			NEX_ASSERT(imageFormat == PixelFormat::R8);
			pft.internalFormat = GL_R8;
			pft.sourceFormat = GL_R8;
			pft.dataType = GL_UNSIGNED_BYTE;
			break;
		case PixelFormat::BGRA8:
			pft.internalFormat = GL_RGBA8;
			pft.sourceFormat = GL_BGRA;
#if (NEX_ENDIANNESS == NEX_ENDIAN_LITTLE)
			pft.dataType = GL_UNSIGNED_INT_8_8_8_8_REV;
#else
			pft.dataType = GL_UNSIGNED_INT_8_8_8_8;
#endif
			break;
		case PixelFormat::RGBA8:
			NEX_ASSERT(imageFormat == PixelFormat::RGBA8 || imageFormat == PixelFormat::BGRA8);
			pft.internalFormat = GL_RGBA8;
			pft.sourceFormat = imageFormat == PixelFormat::RGBA8 ? GL_RGBA : GL_BGRA;
#if (NEX_ENDIANNESS == NEX_ENDIAN_LITTLE)
			pft.dataType = GL_UNSIGNED_INT_8_8_8_8_REV;
#else
			pft.dataType = GL_UNSIGNED_INT_8_8_8_8;
#endif
			break;
		case PixelFormat::D16:
			NEX_ASSERT(imageFormat == PixelFormat::D16);
			pft.internalFormat = GL_DEPTH_COMPONENT16;
			pft.sourceFormat = GL_DEPTH_COMPONENT;
			pft.dataType = GL_FLOAT;
			break;
		case PixelFormat::D24:
			NEX_ASSERT(imageFormat == PixelFormat::D24);
			pft.internalFormat = GL_DEPTH_COMPONENT24;
			pft.sourceFormat = GL_DEPTH_COMPONENT;
			pft.dataType = GL_FLOAT;
			break;
		case PixelFormat::D24S8:
			NEX_ASSERT(imageFormat == PixelFormat::D24S8);
			pft.internalFormat = GL_DEPTH24_STENCIL8;
			pft.sourceFormat = GL_DEPTH_STENCIL;
			pft.dataType = GL_FLOAT; // irrelevant
			break;
		case PixelFormat::D32:
			NEX_ASSERT(imageFormat == PixelFormat::D32);
			pft.internalFormat = GL_DEPTH_COMPONENT32;
			pft.sourceFormat = GL_DEPTH_COMPONENT;
			pft.dataType = GL_FLOAT;
			break;
		case PixelFormat::RGBA16F:
			NEX_ASSERT(imageFormat == PixelFormat::RGBA16F);
			pft.internalFormat = GL_RGBA16F;
			pft.sourceFormat = GL_RGBA;
			pft.dataType = GL_FLOAT;
		}
		return pft;
	}
}


