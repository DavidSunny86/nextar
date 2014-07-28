/*
 * Shader.cpp
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <Pass.h>
#include <ShaderAsset.h>
#include <RenderManager.h>

namespace nextar {

/*****************************************************/
/* Shader											 */
/*****************************************************/
ShaderAsset::ShaderAsset(const StringID name, const StringID factory) :
		nextar::Asset(name, factory), translucency(0), renderQueue((uint8)-1), visibilityMask(0) {
}

ShaderAsset::~ShaderAsset() {
	_DestroyPasses();
}

void ShaderAsset::_DestroyPasses() {
	passes.clear();
}

uint32 ShaderAsset::GetClassID() const {
	return CLASS_ID;
}

void ShaderAsset::NotifyAssetLoaded() {
	StreamRequest* creationParams = static_cast<StreamRequest*>(streamRequest);
	/* update programs */
	/* build with compilation options */
	_DestroyPasses();

	StreamPassList &spl = creationParams->passes;
	passes.reserve(spl.size());

	for (uint32 passIndex = 0; passIndex < spl.size(); ++passIndex) {
		spl[passIndex].compileParams.passIndex = passIndex;
		passes.emplace_back(spl[passIndex].name);
		_CompilePass(passes.back(), spl[passIndex],
				creationParams->compilationOptions);
	}
	/* update */
	_BuildParameterTable(spl);
	/* mark request as complete */
	creationParams->flags |= StreamRequest::COMPLETED;
	if (renderQueue == (uint8)-1) {
		auto& renderQueueInfo = RenderManager::Instance().GetRenderQueueInfo();
		for(auto r = renderQueueInfo.begin(); r != renderQueueInfo.end(); ++r) {
			if (
				( (flags & (ShaderAsset::DEFERRED)) && ((*r).flags & RenderQueueFlags::DEFERRED) ) ||
				( (flags & ShaderAsset::TRANSLUCENT) && ((*r).flags & RenderQueueFlags::TRANSLUCENCY) )  ||
				( (flags & ShaderAsset::OVERLAY) && ((*r).flags & RenderQueueFlags::OVERLAY) ) ||
				( (flags & ShaderAsset::BACKGROUND) && ((*r).flags & RenderQueueFlags::BACKGROUND) )
				) {
				renderQueue = (uint8)std::distance(renderQueueInfo.begin(), r);
				break;
			} else if ((*r).flags & RenderQueueFlags::FORWARD) {
				renderQueue = (uint8)std::distance(renderQueueInfo.begin(), r);
			}
		}
		if (renderQueue == (uint8)-1) {
			renderQueue = 0; // default it out, may be very incorrect
			Error("Could not determine render queue!");
		}
	}

	if (flags & ShaderAsset::BACKGROUND)
		visibilityMask = VisibilityMask::VISIBILITY_BACKGROUND;
	else if(flags & ShaderAsset::TRANSLUCENT)
		visibilityMask = VisibilityMask::VISIBILITY_TRANSLUCENT;
	else if (flags & ShaderAsset::OVERLAY)
		visibilityMask = VisibilityMask::VISIBILITY_OVERLAY;
	else
		visibilityMask = VisibilityMask::VISIBILITY_OPAQUE;

	/* notify dependents */
	Asset::NotifyAssetLoaded();
	SetReady(true);
}

void ShaderAsset::_CompilePass(Pass& r, ShaderAsset::StreamPass& p,
		const String& co) {
	p.compileParams.compileOptions = &co;
	p.compileParams.parameters = &paramLookup;
	r.SetID(p.name);
	r.RequestUpdate(Pass::MSG_PASS_COMPILE,
			reinterpret_cast<ContextObject::ContextParamPtr>(&p.compileParams));
}

void ShaderAsset::NotifyAssetUnloaded() {
	_DestroyPasses();
	/* notify dependents */
	Asset::NotifyAssetUnloaded();
}

void ShaderAsset::UnloadImpl() {
	_DestroyPasses();
}

void ShaderAsset::NotifyAssetUpdated() {
	// _BuildParameterTable();
	Asset::NotifyAssetUpdated();
}

nextar::StreamRequest* ShaderAsset::CreateStreamRequestImpl(bool load) {
	return NEX_NEW(ShaderAsset::StreamRequest(this));
}

void ShaderAsset::DestroyStreamRequestImpl(nextar::StreamRequest*& request,
		bool load) {
	ShaderAsset::StreamRequest* req =
			static_cast<ShaderAsset::StreamRequest*>(request);
	NEX_DELETE(req);
	request = nullptr;
}

void ShaderAsset::_BuildParameterTable(StreamPassList& spl) {
	paramLookup.shrink_to_fit();
	// sort by context type
	std::sort(std::begin(paramLookup), std::end(paramLookup),
			[] (const ParamEntry& e1, const ParamEntry& e2) -> bool {
				/* for ordered compilation no need to check passIndex order
				 return e1.context == e2.context ? e1.passIndex < e2.passIndex :
				 e1.context < e2.context;
				 */
				return e1.context <= e2.context;
			});

	ParameterContext lastContext = ParameterContext::CTX_UNKNOWN;
	uint32 index = -1;
	uint32 offset = 0;
	for (auto& e : paramLookup) {
		if (e.context != lastContext) {
			lastContext = e.context;
			offset = 0;
		}
		if (index != e.passIndex) {
			index = e.passIndex;
			spl[index].offsets.offset[(uint32)lastContext] = offset;
		}
		offset += e.maxSize;
	}

	for (auto i = std::pair<StreamPassList::iterator,PassList::iterator>(spl.begin(), passes.begin()); i.first != spl.end();
			++i.first, ++i.second) {
		(*i.second).RequestUpdate(Pass::MSG_PASS_UPDATE_PARAMBUFFER_OFFSET,
				reinterpret_cast<ContextObject::ContextParamPtr>(&(*i.first).compileParams));
	}
}


/*****************************************************/
/* Shader::StreamPass                                */
/*****************************************************/
ShaderAsset::StreamPass::StreamPass(const ShaderAsset::StreamPass& p) : name(StringUtils::NullID) {
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
}

/*****************************************************/
/* Shader::StreamRequest							 */
/*****************************************************/
ShaderAsset::StreamRequest::StreamRequest(ShaderAsset* shader) :
		AssetStreamRequest(shader) {
}

ShaderAsset::StreamRequest::~StreamRequest() {
}

void ShaderAsset::StreamRequest::SetProgramSource(Pass::ProgramStage stage,
		String&& src) {
	(*currentPass).compileParams.programSources[stage] = std::move(src);
}

void ShaderAsset::StreamRequest::SetCompilationOptions(const String& options) {
	compilationOptions = options;
}

void ShaderAsset::StreamRequest::AddTextureUnit(const String& unitName,
		TextureUnitParams& tu, TextureBase* texture) {
	ShaderAsset* shader = static_cast<ShaderAsset*>(streamedObject);
	Pass::TextureDescMap& defaultTextureUnits =
			(*currentPass).compileParams.textureStates;
	Pass::SamplerDesc& sd = defaultTextureUnits[unitName];
	sd.texUnitParams = tu;
	sd.defaultTexture = texture;
	if (texture->IsTextureAsset())
		metaInfo.AddDependency(static_cast<TextureAsset*>(texture));
}

void ShaderAsset::StreamRequest::SetBlendState(BlendState& state) {
	(*currentPass).compileParams.blendState = state;
}

void ShaderAsset::StreamRequest::SetRasterState(RasterState& state) {
	(*currentPass).compileParams.rasterState = state;
}

void ShaderAsset::StreamRequest::SetDepthStencilState(
		DepthStencilState& state) {
	(*currentPass).compileParams.depthStencilState = state;
}

void ShaderAsset::StreamRequest::AddPass(StringID name) {
	passes.emplace_back(name);
	currentPass = passes.rbegin();
}

void ShaderAsset::StreamRequest::SetParamterBuffer(ParameterBuffer&& data) {
	ShaderAsset* shader = static_cast<ShaderAsset*>(streamedObject);
	shader->passParamData = std::move(data);
}

ParamDataType ShaderAsset::MapParamType(const String& typeName) {
	typedef std::pair<const char*, ParamDataType> ParamTypeNamePair;
	static ParamTypeNamePair paramTypeNameTable[] = {
		ParamTypeNamePair("bool", ParamDataType::PDT_BOOL),
		ParamTypeNamePair("count", ParamDataType::PDT_COUNT),
		ParamTypeNamePair("float", ParamDataType::PDT_FLOAT),
		ParamTypeNamePair("int", ParamDataType::PDT_INT),
		ParamTypeNamePair("ivec2", ParamDataType::PDT_IVEC2),
		ParamTypeNamePair("ivec3", ParamDataType::PDT_IVEC3),
		ParamTypeNamePair("ivec4", ParamDataType::PDT_IVEC4),
		ParamTypeNamePair("mat3x4", ParamDataType::PDT_MAT3x4),
		ParamTypeNamePair("mat4x4", ParamDataType::PDT_MAT4x4),
		ParamTypeNamePair("struct", ParamDataType::PDT_STRUCT),
		ParamTypeNamePair("texture", ParamDataType::PDT_TEXTURE),
		ParamTypeNamePair("uint", ParamDataType::PDT_UINT),
		ParamTypeNamePair("unknown", ParamDataType::PDT_UNKNOWN),
		ParamTypeNamePair("vec2", ParamDataType::PDT_VEC2),
		ParamTypeNamePair("vec3", ParamDataType::PDT_VEC3),
		ParamTypeNamePair("vec4", ParamDataType::PDT_VEC4),
	};
	static const uint32 arraySize = sizeof(paramTypeNameTable) / sizeof(paramTypeNameTable[0]);
	ParamTypeNamePair* last = paramTypeNameTable + arraySize;
	ParamTypeNamePair* ptr = std::lower_bound(paramTypeNameTable, last, typeName,
			[] (const ParamTypeNamePair& p1, const String& searchText) -> int {
				return searchText.compare(p1.first);
	});

	if (ptr && ptr != last) {
		return ptr->second;
	}

	return ParamDataType::PDT_UNKNOWN;
}

} /* namespace nextar */
