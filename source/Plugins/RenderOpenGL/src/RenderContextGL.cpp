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
#include <PassViewGL.h>
#include <FrameBufferObjectGL.h>
#include <MultiRenderTargetViewGL.h>
#include <VertexLayoutGL.h>
#include <RenderTexture.h>
#include <RenderTextureViewGL.h>
#include <RenderBuffer.h>
#include <RenderBufferViewGL.h>

namespace RenderOpenGL {

RenderContextGL::RenderContextGL(RenderDriverGL* _driver) :
		BaseRenderContext(_driver), contextFlags(0) {
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_DESTROY_RESOURCES, DestroyResources, this);
}

RenderContextGL::~RenderContextGL() {

}

void RenderContextGL::DestroyResources(void* pThis) {
	RenderContextGL* rc = reinterpret_cast<RenderContextGL*>(pThis);
	auto& ubMap = rc->uniformBufferMap;
	for (auto& e : ubMap) {
		e.second.Destroy(rc);
	}
}

RenderWindow* RenderContextGL::CreateRenderWindowImpl() {
	return CreateWindowImpl();
}

void RenderContextGL::CloseImpl() {
	uniformBufferMap.clear();
}

void RenderContextGL::PostWindowCreation(RenderWindow* gw) {
	/* check if extensions were initialized or initialize extensions */
	if (!IsContextReady()) {
		ReadyContext(gw);
		contextFlags |= CONTEXT_READY;

		if (!AreExtensionsReady()) {
			_ReadyExtensions(contextCreationParams.reqOpenGLVersionMajor,
					contextCreationParams.reqOpenGLVersionMinor);
			GL_CHECK();
			contextFlags |= EXTENSIONS_READY;
		}
	}
}

void RenderContextGL::PostWindowDestruction(RenderWindow* gw) {
	/* do nothing */
}

void RenderContextGL::CreateImpl(
		const RenderDriver::ContextCreationParams& ctxParams) {
	SetCreationParams(ctxParams);
	GL_CHECK();
}

GLuint RenderContextGL::CreateShader(GLenum type, const char* preDef,
		const char* source) {
	const GLchar* sourceBuff[2] = { 0 };
	sourceBuff[0] = static_cast<const GLchar*>(preDef ? preDef : "");
	sourceBuff[1] = static_cast<const GLchar*>(source);
	GL_CHECK();
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

void RenderContextGL::DestroyShader(GLuint sh) {
	GlDeleteShader(sh);
	GL_CHECK();
}

GLuint RenderContextGL::CreateProgram(GLuint shaders[]) {
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

void RenderContextGL::DestroyProgram(GLuint program) {
	GlDeleteProgram(program);
	GL_CHECK();
}

uint32 RenderContextGL::ReadProgramSemantics(GLuint program,
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

void RenderContextGL::ReadUniforms(PassViewGL* pass, uint32 passIndex,
	GLuint program,
	Pass::VarToAutoParamMap& remapParams,
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
		String uniName = name;
		//bool shareUb = true;
		UniformBufferGL* ubPtr = nullptr;
		UniformBufferMap::iterator it = uniformBufferMap.find(uniName);
		if (it != uniformBufferMap.end()) {
			ubPtr = &(*it).second;
			if (!(ubPtr && ubPtr->numParams == numParams && ubPtr->size == size)) {
				Warn(
					String(
					"Uniform buffer cannot be registered"
					" (name already registered with different contents): ")
					+ uniName);
				ubPtr = 0;
				// we can surely register it as unshared ub
				continue;
			}
		}

		if (!ubPtr) {
			// create a new uniform buffer
			ubPtr = CreateUniformBuffer(pass, passIndex, uniName, i, program,
				numParams, size, remapParams, paramTable);
			GL_CHECK();
			//ubPtr->SetBinding((GLuint) uniformBufferMap.size()-1);
		}

		ubList.push_back(ubPtr);
		//GlBindBufferBase(GL_UNIFORM_BUFFER, ubPtr->GetBinding(),
		//		ubPtr->ubNameGl);
		//GL_CHECK();
		//GlUniformBlockBinding(program, i, ubPtr->GetBinding());
		//GL_CHECK();
	}
	// sort ublist by context
	std::sort(ubList.begin(), ubList.end(), [](const ParameterGroup* p1, const ParameterGroup* p2){
		return (p1->context < p2->context) != 0;
	});
}

UniformBufferGL* RenderContextGL::CreateUniformBuffer(PassViewGL* pass,
		uint32 passIndex, const String& name, GLint blockIndex, GLuint prog,
		GLuint numParams, uint32 size, Pass::VarToAutoParamMap& remapParams,
		ParamEntryTable* table) {

	NEX_ASSERT(size > 0);
	uint16 numUnmappedParams = 0;
	UniformBufferGL& u = uniformBufferMap[name];
	u.ubNameGl = GL_INVALID_VALUE;
	u.size = size;
	u.numParams = numParams;
	u.lastUpdateId = -1;
	u.arrayCount = 1;
	u.type = ParamDataType::PDT_STRUCT;
	u.parameter = nullptr;
	u.processor = nullptr;

	GlGenBuffers(1, &u.ubNameGl);
	GL_CHECK();
	if (u.ubNameGl == GL_INVALID_VALUE) {
		Error("Failed to generate buffer name");
		NEX_THROW_GracefulError(EXCEPT_INVALID_CALL);
	}

	bool storeIndividualParams = table != nullptr;
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
		// parameter parsing is not important if the
		// the auto param is well defined and understood
		// by the engine
		if (u.processor) {
			parseIndividualParams = false;
			storeIndividualParams = false;
		}
	} else {
		u.context = ParameterContext::CTX_UNKNOWN;
		u.autoName = AutoParamName::AUTO_CUSTOM_CONSTANT;
	}

	u.size = size;
	GlBindBuffer(GL_UNIFORM_BUFFER, u.ubNameGl);
	GL_CHECK();
	GlBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
	GL_CHECK();
	GlBindBuffer(GL_UNIFORM_BUFFER, 0);
	GL_CHECK();

	if (!storeIndividualParams && !parseIndividualParams)
		return &u;

	void* tempBuffer = NEX_ALLOC(sizeof(GLint) * numParams * 7 + 128,
			MEMCAT_GENERAL);
	GLint *indices = static_cast<GLint*>(tempBuffer) + numParams * 0;
	GLint *type = static_cast<GLint*>(tempBuffer) + numParams * 1;
	GLint *offset = static_cast<GLint*>(tempBuffer) + numParams * 2;
	GLint *arraynum = static_cast<GLint*>(tempBuffer) + numParams * 3;
	GLint *rowMaj = static_cast<GLint*>(tempBuffer) + numParams * 4;
	GLint *matStride = static_cast<GLint*>(tempBuffer) + numParams * 5;
	GLint *arrayStride = static_cast<GLint*>(tempBuffer) + numParams * 6;
	char *uniName = reinterpret_cast<char*>(static_cast<GLint*>(tempBuffer)
			+ numParams * 7);

	const GLuint* uindices = (GLuint*) indices;
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
	if (storeIndividualParams) {
		startParamIndex = (uint32)table->size();
		table->reserve(startParamIndex + numParams);
	}
	for (GLint i = 0; i < (GLint) numParams; ++i) {
		UniformGL& uform = uniforms[i];
		GlGetActiveUniformName(prog, indices[i], 128, 0, uniName);
		GL_CHECK();
		String paramName = uniName;
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
						"Parameters from different contexts cannot be mixed for buffer: "
								+ name + "with parameter: " + paramName);
				// discard??
			}
			uform.autoName = paramDef->autoName;
			uform.processor = paramDef->processor;
			autoParamCount++;
		} else {
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

		if (storeIndividualParams && !paramDef) {
			ParamEntry pe;
			pe.arrayCount = uform.arrayCount;
			pe.autoName = uform.autoName;
			pe.maxSize = uform.size;
			pe.name = std::move(paramName);
			pe.type = uform.type;
			pe.passIndex = passIndex;
			table->push_back(pe);
		}
	}

	if (chosen == ParameterContext::CTX_UNKNOWN)
		chosen = GuessContextByName(name);
	if (chosen == ParameterContext::CTX_UNKNOWN) {
		Warn(
				"Buffer context could not be determined, assigning material context: "
						+ name);
		chosen = ParameterContext::CTX_MATERIAL;
	}
	u.context = chosen;
	// the context was just determined
	if (storeIndividualParams) {
		for (; startParamIndex < table->size(); ++startParamIndex) {
			table->at(startParamIndex).context = chosen;
		}
	}

	if ((!autoParamCount && chosen != ParameterContext::CTX_VIEW
			&& chosen != ParameterContext::CTX_FRAME)
			|| !parseIndividualParams) {
		// lets look for the struct processor
		if (!u.processor)
			u.processor = Pass::GetStructProcessor();
		NEX_DELETE_ARRAY(uniforms);
		u.parameter = nullptr;
	} else {
		u.parameter = uniforms;
		// sort the resulting uniforms such that auto params come first
		// followed by unmapped params. Unmapped params are sorted by name.
		std::sort(uniforms, uniforms + numParams,
				[] (const UniformGL& first, const UniformGL& second) {
					if (first.autoName == second.autoName) {
						NEX_ASSERT(first.autoName == AutoParamName::AUTO_CUSTOM_CONSTANT);
						return first.bufferOffset < first.bufferOffset;
					}
					return (first.autoName <
							second.autoName) != 0;
				});
	}
	
	NEX_FREE(tempBuffer, MEMCAT_GENERAL);

	return &u;
}

GLuint RenderContextGL::CreateSamplerFromParams(const TextureUnitParams& params) {
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
	GlSamplerParameterf(sampler, GL_TEXTURE_MIN_LOD, params.minLod);
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

void RenderContextGL::ReadSamplers(PassViewGL* pass, uint32 passIndex,
		GLuint program,
		Pass::VarToAutoParamMap& remapParams,
		ParamEntryTable* paramTable,
		const Pass::TextureDescMap& texMap) {
	/** todo Massive work left for sampler arrays */
	GLint numUni = 0;
	uint32 numSamplers = 0;
	char name[128];
	size_t extra = 0;
	GlGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUni);
	vector<GLuint>::type samplerList;

	for(auto& s : texMap) {
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
		paramTable->reserve(numUni + startParamIndex);
	}
	GlUseProgram(program);
	for (GLuint i = 0; i < (GLuint) numUni; ++i) {

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
					texMap);
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
				if (texMap[tu].texUnitParams.context == ParameterContext::CTX_UNKNOWN)
					ss.context = GuessContextByName(unitName,
						ParameterContext::CTX_MATERIAL);
				else
					ss.context = texMap[tu].texUnitParams.context;
				ss.processor = Pass::GetTextureProcessor();
			} else {
				ss.autoName = paramDef->autoName;
				ss.context = paramDef->context;
				ss.processor = paramDef->processor;
			}
			ss.size = sizeof(TextureUnit);
			// todo Should be a view
			ss.defaultTexture.texture = texMap[tu].defaultTexture;
			if (paramTable && !paramDef) {
				ParamEntry pe;
				pe.arrayCount = 1;
				pe.autoName = ss.autoName;
				pe.context = ss.context;
				pe.maxSize = ss.size;
				pe.name = std::move(unitName);
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

void RenderContextGL::Capture(PixelBox& image, RenderTarget* rt, GLuint *pbo,
		FrameBuffer frameBuffer) {
	/* todo Expand this function for frame buffer,
	 * depth map capture etc. */
	GLenum readTargets[] = {
	GL_FRONT_LEFT,
	GL_FRONT_RIGHT,
	GL_BACK_LEFT,
	GL_BACK_RIGHT };

	Size size = rt->GetDimensions();
	// assuming color buffer capture
	size_t dataSize = size.dx * size.dy * 4;

	if (!image.Data()) {
		image.Data() = static_cast<uint8*>((NEX_ALLOC(dataSize, MEMCAT_GENERAL)));
		image.deleteData = true;
	}

	bool asyncCapture =
			RenderManager::Instance().GetRenderSettings().asyncCapture;

	glReadBuffer(readTargets[(uint32)frameBuffer]);
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
void RenderContextGL::SwitchPass(CommitContext& context, Pass::View* passView) {
	PassViewGL* passViewGl = static_cast<PassViewGL*>(passView);
	GLuint program = passViewGl->GetProgram();
	GlUseProgram(program);
	GL_CHECK();
	ParameterGroupList& paramList = passViewGl->sharedParameters;
	for (uint32 i = 0; i < paramList.size(); ++i) {
		UniformBufferGL* ubPtr = static_cast<UniformBufferGL*>(paramList[i]);
		GlBindBufferBase(GL_UNIFORM_BUFFER, i, ubPtr->ubNameGl);
		GL_CHECK();
		GlUniformBlockBinding(program, i, i);
		GL_CHECK();
	}
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
	}

	IndexBuffer* ibuffer = streamData->indices.indices.GetPtr();
	if (ibuffer) {
		GpuBufferViewGL* buffer = static_cast<GpuBufferViewGL*>(GetView(ibuffer));
		GLint indexsize = buffer->GetStride();
		// hack
		NEX_STATIC_ASSERT(GL_UNSIGNED_INT == GL_UNSIGNED_SHORT + 2);
		GLenum indextype = GL_UNSIGNED_SHORT + (indexsize - 2);
		GlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->GetBufferId());
		GL_CHECK();
		if (vd.start == 0) {
			if (streamData->instanceCount == 1)
				glDrawElements(primtype, streamData->indices.count, indextype,
						reinterpret_cast<const GLvoid*> (indexsize * (GLint)streamData->indices.start));
			else
				GlDrawElementsInstanced(primtype, streamData->indices.count,
						indextype,
						reinterpret_cast<const GLvoid*> (indexsize * (GLint)streamData->indices.start),
						streamData->instanceCount);
		} else {
			if (streamData->instanceCount == 1)
				GlDrawElementsBaseVertex(primtype, streamData->indices.count,
						indextype,
						reinterpret_cast<const GLvoid*> (indexsize * (GLint)streamData->indices.start),
						vd.start);
			else
				GlDrawElementsInstancedBaseVertex(primtype,
						streamData->indices.count, indextype,
						reinterpret_cast<const GLvoid*> (indexsize * (GLint)streamData->indices.start),
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

void RenderContextGL::SetCurrentTarget(RenderTarget* canvas) {
	if (canvas == nullptr)
		SetCurrentWindow(nullptr);
	else {
		switch (canvas->GetRenderTargetType()) {
		case RenderTargetType::RENDER_TEXTURE: {
			RenderTextureViewGL* textureView =
					static_cast<RenderTextureViewGL*>(GetView(
							static_cast<RenderTexture*>(canvas)));
			FrameBufferObjectGL& fbo = textureView->GetFBO();
			if (!fbo.IsValid())
				textureView->CreateFBO(this);
			fbo.Bind(false, this);
		}
			break;
		case RenderTargetType::RENDER_BUFFER: {
			RenderBufferViewGL* textureView =
					static_cast<RenderBufferViewGL*>(
							GetView(static_cast<RenderBuffer*>(canvas)));
			FrameBufferObjectGL& fbo = textureView->GetFBO();
			if (!fbo.IsValid())
				textureView->CreateFBO(this);
			fbo.Bind(false, this);
		}
			break;
		case RenderTargetType::RENDER_WINDOW:
			SetCurrentWindow(canvas);
			break;
		case RenderTargetType::MULTI_RENDER_TARGET: {
			MultiRenderTargetViewGL* textureView =
					static_cast<MultiRenderTargetViewGL*>(GetView(
							static_cast<MultiRenderTarget*>(canvas)));
			FrameBufferObjectGL& fbo = textureView->GetFBO();
			NEX_ASSERT(fbo.IsValid());
			fbo.Bind(false, this);
		}
			break;
		}
	}
}

void RenderContextGL::Clear(Color& c, float depth, uint16 stencil,
		ClearFlags flags) {
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

GLuint RenderContextGL::CreateBuffer(size_t size, GLenum usage, GLenum type) {
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

void RenderContextGL::CopyBuffer(GLuint src, GLuint dest, size_t size) {
	GlBindBuffer(GL_COPY_READ_BUFFER, src);
	GlBindBuffer(GL_COPY_WRITE_BUFFER, dest);
	GlCopyBufferSubData(GL_COPY_READ_BUFFER,
	GL_COPY_WRITE_BUFFER, 0, 0, size);
	GL_CHECK();
	GlBindBuffer(GL_COPY_READ_BUFFER, 0);
	GlBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void RenderContextGL::AllocateTexture(GLenum target, GLint levels,
		GLenum format, GLsizei width, GLsizei height, GLsizei depth) {
	switch (target) {
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

void RenderContextGL::WriteTextureLevel(GLenum target, GLint level,
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

GLint RenderContextGL::GetGlMinFilter(TextureMinFilter t) {
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

GLint RenderContextGL::GetGlMagFilter(TextureMagFilter t) {
	switch (t) {
	case TF_MAG_NEAREST:
		return GL_NEAREST;
	case TF_MAG_LINEAR:
		return GL_LINEAR;
	}
	return GL_LINEAR;
}

GLint RenderContextGL::GetGlAddressMode(TextureAddressMode t) {
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

GLenum RenderContextGL::GetGlCompareFunc(TextureComparisonMode type) {
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

VertexComponentType RenderContextGL::GetSemanticType(GLenum e) {
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

uint16 RenderContextGL::GetShaderParamSize(GLuint type) {
	switch (type) {
	case GL_BOOL:
		return (uint16) 1;
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
	return (uint16) 0;
}

ParamDataType RenderContextGL::GetShaderParamType(GLuint type) {
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

GLenum RenderContextGL::GetGlTextureType(TextureBase::TextureType type) {
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

PixelFormatGl RenderContextGL::GetGlPixelFormat(PixelFormat imageFormat,
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

ParameterContext RenderContextGL::GuessContextByName(const String& name,
		ParameterContext defaultContex) {
	String lowerName = name;
	StringUtils::ToLower(lowerName);
	if (lowerName.find("frame") != String::npos)
		return ParameterContext::CTX_FRAME;
	else if (lowerName.find("view") != String::npos)
		return ParameterContext::CTX_VIEW;
	else if (lowerName.find("pass") != String::npos)
		return ParameterContext::CTX_PASS;
	else if (lowerName.find("material") != String::npos)
		return ParameterContext::CTX_MATERIAL;
	else if (lowerName.find("object") != String::npos)
		return ParameterContext::CTX_OBJECT;
	return defaultContex;
}

ContextID RenderContextGL::RegisterObject(ContextObject::Type type, uint32 hint) {
	const uint32 dynamicLayoutHint = (VertexLayout::HAS_TRANSIENT_BUFFERS|
			VertexLayout::LOOSE_BUFFER_BINDING|VertexLayout::SHARED_VERTEX_LAYOUT);
	ContextObject::View* view = nullptr;
	switch(type) {
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
		view = (NEX_NEW(GpuBufferViewGL(GL_ARRAY_BUFFER)));
		break;
	case ContextObject::TYPE_VERTEX_BUFFER:
		if (hint == (uint32)GpuBuffer::REGULARLY_RELEASED ||
				hint == (uint32)GpuBuffer::IMMEDIATELY_RELEASED)
			view = (NEX_NEW(GpuTransientBufferViewGL(GL_ELEMENT_ARRAY_BUFFER)));
		else
			view = (NEX_NEW(GpuBufferViewGL(GL_ELEMENT_ARRAY_BUFFER)));
		break;
	case ContextObject::TYPE_VERTEX_LAYOUT:
		if (hint & dynamicLayoutHint) {
			if (GLE_ARB_vertex_attrib_binding)
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

void RenderContextGL::UnregisterObject(ContextID id) {
	if (id) {
		ContextObject::View* view = reinterpret_cast<ContextObject::View*>(id);
		view->Destroy(this);
		NEX_DELETE(view);
	}
}

}

