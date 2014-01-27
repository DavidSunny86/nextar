/*
 * RenderContextGL.cpp
 *
 *  Created on: 17-Aug-2013
 *      Author: obhi
 */
#include "RenderOpenGL.h"
#include "RenderContextGL.h"
#include "RenderDriverGL.h"
#include "RenderWindow.h"
#include "Shader.h"

namespace RenderOpenGL {

	RenderContextGL::RenderContextGL(RenderDriverGL* _driver) : BaseRenderContext(_driver),
			contextFlags(0) {
	}

	RenderContextGL::~RenderContextGL() {

	}

	RenderWindowPtr RenderContextGL::CreateRenderWindowImpl() {
		RenderWindow* gw = CreateWindowImpl();
		return Assign(gw);
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
	            /** @todo Add support for output semantic */
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

	void RenderContextGL::ReadUniforms(UniformBufferList& ubList, GLuint program) {
		GLint numUblocks = 0;
		GlGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numUblocks);
		char name[128];
		ubList.reserve(numUblocks);
		for (GLint i = 0; i < numUblocks; ++i) {
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
			UniformBufferGL* ubPtr = 0;
			UniformBufferMap::iterator it = uniformBufferMap.find(uniName);
			if (it != uniformBufferMap.end()) {
				ubPtr = &(*it).second;
				if ( !(ubPtr && ubPtr->GetParamCount() == numParams &&
						ubPtr->GetSize() == size) ) {
					Warn(String("Uniform buffer cannot be registered"
					            " (name already registered with different contents): ") + uniName);
					ubPtr = 0;
					// we can surely register it as unshared ub
					continue;
				}
			}

			if (!ubPtr) {
				// create a new uniform buffer
				//if (shareUb) {
					// insert into map
				ubPtr = &uniformBufferMap[uniName];
				CreateUniformBuffer(ubPtr, i, program, numParams, size);
				//}
			}
			ubList.push_back(ubPtr);
			GlUniformBlockBinding(program, i, i);
		}

	}

	void RenderContextGL::CreateUniformBuffer(UniformBufferGL* u,
			GLint blockIndex, GLuint prog, GLuint numParams, size_t size) {

	    NEX_ASSERT(size > 0);
	    uint16 numUnmappedParams = 0;

	    u->ubName = GL_INVALID_VALUE;
	    GlGenBuffers(1, &u->ubName);
	    GL_CHECK();
	    if (u->ubName == GL_INVALID_VALUE) {
	        Error("Failed to generate buffer name");
	        NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
	    }

	    uint8 mask = 0;
	    UniformList uniforms;
	    uniforms.reserve(numParams);

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

	    UniformGL uform;
	    for (GLint i = 0; i < (GLint)numParams; ++i) {
	        GlGetActiveUniformName(prog, indices[i], 128, 0, uniName);
	        GL_CHECK();
	        uform.isRowMajMatrix = rowMaj[i] ? true : false;
	        uform.typeGl = uint8(type[i]);
	        uform.matrixStride = matStride[i];
	        uform.arrayStride = arrayStride[i];
	        uform.arrayCount = uint16(arraynum[i]);
	        Shader::ParamDef& paramDef = Shader::MapParamName(uform.name);
	        uform.autoName = paramDef.autoName;
	        if (uform.autoName == (uint16)AutoParamName::AUTO_CUSTOM)
	        	uform.name = uniName;
	        uform.type = GetShaderParamType(uform.typeGl);
	        uform.updateFrequency = paramDef.updateFrequency;
	        uform.offset =  offset[i];
	        if (uform.autoName == (uint16)AutoParamName::AUTO_CUSTOM)
	        	numUnmappedParams++;
	        else if(paramDef.type != uform.type ) {
	        	Warn("Auto parameter differs by type, please set appropriate type: " + uform.name);
	        	continue;
	        }
	        mask |= uform.updateFrequency;
	        uniforms.push_back(uform);
	    }

	    u->numUnmappedParams = numUnmappedParams;
	    u->uniforms.swap(uniforms);
	    u->mask = mask;
	    u->size = size;
	    NEX_FREE(tempBuffer, MEMCAT_GENERAL);
	    GlBindBuffer( GL_UNIFORM_BUFFER, u->ubName );
	    GL_CHECK();
	    GlBufferData( GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW );
	    GL_CHECK();
	    GlBindBuffer( GL_UNIFORM_BUFFER, 0 );
	    GL_CHECK();
	}

	void RenderContextGL::ReadSamplers(SamplerStateList& samplers, const PassGL* shader, GLuint program) {
		/** @todo Massive work left for sampler arrays */
	    GLint numUni = 0;
	    char name[128];
        size_t extra = 0;
	    GlGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUni);
	    samplers.reserve(numUni);

        for (GLuint i = 0; i < (GLuint) numUni; ++i) {

            GlGetActiveUniformName(program, i, 128, 0, name);
	        GLint loc = GlGetUniformLocation(program, name);
	        String unitName = name;
            GLint type;

            GlGetActiveUniformsiv(program, 1, &i, GL_UNIFORM_TYPE, &type);

            GL_CHECK();
            if (IsSamplerType(type)) {

            	SamplerState ss;
            	/**
            	 * @todo Add sampler array support please
            	 * */
            	ss.samplerCount = 1;
                //uint16 index = shader->GetTextureUnitIndex(unitName);
                //if ((int16)index < 0) {
                //    Error(String("Overflowing/Unspecified texture unit index for ") + name);
                //    continue;
                //}
            	const TextureUnitParams& tu = *shader->GetTextureUnit(unitName);
            	if (&tu == nullptr) {
				    Error(String("Overflowing/Unspecified texture unit index for ") + name);
				    continue;
				}
                Shader::ParamDef& paramDef = Shader::MapParamName(unitName);
                ss.location = loc;
                /**
                 * The index of the texture unit within the shader may not be stored
                 * as we have already created the sampler object from the data provided.
                 * Update on a state parameter might require that the sampler object
                 * be recreated. Now, we choose not to store this index on to the shader
                 * texture table here, as we can search via name to obtain the same. To
                 * make it more efficient, we may eventually store the texture units as
                 * in a map (marked as @todo) in the shader. This index will not be the
                 * location the texture will be bound to, or the sampler will be bound to.
                 * The sampler will be bound to the index in which it gets stored in the
                 * sampler table. This will be efficient and will result in no gaps in
                 * between consecutive texture units as all the texture units parsed
                 * from the shader might not get used eventually.
                 * */
                // ss.index = (uint8)index;
                ss.autoName = paramDef.autoName;
                if (ss.autoName == (uint16)AutoParamName::AUTO_CUSTOM)
                	ss.name = unitName;
                ss.updateFrequency = paramDef.updateFrequency;

                GlGenSamplers(1, &ss.sampler);
                GL_CHECK();
                if (ss.sampler) {
                	GlSamplerParameteri(ss.sampler, GL_TEXTURE_MIN_FILTER, GetGlMinFilter(tu.minFilter));
                	GL_CHECK();
                	GlSamplerParameteri(ss.sampler, GL_TEXTURE_MAG_FILTER, GetGlMagFilter(tu.magFilter));
                	GL_CHECK();
                	GlSamplerParameteri(ss.sampler, GL_TEXTURE_WRAP_S, GetGlAddressMode(tu.uAddress));
                	GL_CHECK();
                	GlSamplerParameteri(ss.sampler, GL_TEXTURE_WRAP_T, GetGlAddressMode(tu.vAddress));
                	GL_CHECK();
                	GlSamplerParameteri(ss.sampler, GL_TEXTURE_WRAP_R, GetGlAddressMode(tu.wAddress));
                	GL_CHECK();
                }
                if (tu.comparisonFunc) {
                	GlSamplerParameteri(ss.sampler, GL_TEXTURE_COMPARE_FUNC, GetGlCompareFunc(tu.comparisonFunc));
                	GL_CHECK();
                	GlSamplerParameteri(ss.sampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                	GL_CHECK();
                } else {
                	GlSamplerParameteri(ss.sampler, GL_TEXTURE_COMPARE_MODE, GL_NONE);
                	GL_CHECK();
                }
                GlSamplerParameterf(ss.sampler, GL_TEXTURE_LOD_BIAS, tu.lodBias);
				GL_CHECK();
				GlSamplerParameterf(ss.sampler, GL_TEXTURE_MIN_LOD, tu.minLod);
				GL_CHECK();
				GlSamplerParameterf(ss.sampler, GL_TEXTURE_MAX_LOD, tu.maxLod);
                GL_CHECK();
                GlSamplerParameteri(ss.sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, tu.maxAnisotropy);
				GL_CHECK();
				GlSamplerParameterfv(ss.sampler, GL_TEXTURE_BORDER_COLOR, tu.borderColor.AsFloatArray());
				GL_CHECK();
				samplers.push_back(ss);
            }
	    }
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
		}
		return (uint16)ParamDataType::PDT_UNKNOWN;
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

}





