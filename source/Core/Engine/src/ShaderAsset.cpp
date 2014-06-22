/*
 * Shader.cpp
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <Pass.h>
#include <ShaderAsset.h>
#include <ParameterIterator.h>
#include <RenderManager.h>

namespace nextar {

/*****************************************************/
/* Shader											 */
/*****************************************************/
ShaderAsset::ShaderAsset(const StringID name) :
		nextar::Asset(name), translucency(0) {
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
	passes.resize(spl.size());

	for (uint32 passIndex = 0; passIndex < spl.size(); ++passIndex) {
		spl[passIndex].compileParams.passIndex = passIndex;
		_CompilePass(passes[passIndex], spl[passIndex],
				creationParams->compilationOpt);
	}
	/* update */
	_BuildParameterTable(spl);
	/* mark request as complete */
	creationParams->flags |= StreamRequest::COMPLETED;
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

void ShaderAsset::UnloadImpl(nextar::StreamRequest*, bool) {
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
	size_t offset = 0;
	for (auto& e : paramLookup) {
		if (e.context != lastContext) {
			lastContext = e.context;
			offset = 0;
		}
		if (index != e.passIndex) {
			index = e.passIndex;
			spl[index].offsets.offset[lastContext] = offset;
		}
		offset += e.maxSize;
	}

	for (auto i = std::pair(spl.begin(), passes.begin()); i.first != spl.end();
			++i.first, ++i.second) {
		(*i.second).RequestUpdate(Pass::MSG_PASS_UPDATE_PARAMBUFFER_OFFSET,
				reinterpret_cast<ContextObject::ContextParamPtr>(&(*i.first).compileParams));
	}
}

/*****************************************************/
/* Shader::StreamRequest							 */
/*****************************************************/
ShaderAsset::StreamRequest::StreamRequest(ShaderAsset* shader) :
		AssetStreamRequest(shader), currentPass(-1) {
}

ShaderAsset::StreamRequest::~StreamRequest() {
}

void ShaderAsset::StreamRequest::SetOptions(const String& options) {
	compilationOpt = options;
}

void ShaderAsset::StreamRequest::SetProgramSource(Pass::ProgramStage stage,
		String&& src) {
	passes[currentPass].compileParams.programSources[stage] = std::move(src);
}

void ShaderAsset::StreamRequest::AddParam(const String& name,
		const String& param, const String& description,
		const String& defaultValue, ParamDataType type) {
}

void ShaderAsset::StreamRequest::AddMacro(const String& name,
		const String& param, const String& description, bool defaultValue) {
}

void ShaderAsset::StreamRequest::AddTextureUnit(const String& unitName,
		TextureUnitParams& tu, TextureBase* texture) {
	ShaderAsset* shader = static_cast<ShaderAsset*>(streamedObject);
	Pass::TextureDescMap& defaultTextureUnits =
			passes[currentPass].compileParams.textureStates;
	Pass::SamplerDesc& sd = defaultTextureUnits[unitName];
	sd.texUnitParams = tu;
	sd.defaultTexture = texture;
	if (texture->IsTextureAsset())
		metaInfo.AddDependency(static_cast<TextureAsset*>(texture));
}

void ShaderAsset::StreamRequest::SetBlendState(BlendState& state) {
	passes[currentPass].compileParams.blendState = state;
}

void ShaderAsset::StreamRequest::SetRasterState(RasterState& state) {
	passes[currentPass].compileParams.rasterState = state;
}

void ShaderAsset::StreamRequest::SetDepthStencilState(
		DepthStencilState& state) {
	passes[currentPass].compileParams.depthStencilState = state;
}

void ShaderAsset::StreamRequest::AddPass(StringID name) {
	currentPass = (uint32) passes.size();
	passes.resize(currentPass + 1);
	passes[currentPass].name = name;
}

void ShaderAsset::StreamRequest::SetParamterBuffer(ParameterBuffer&& data) {
	ShaderAsset* shader = static_cast<ShaderAsset*>(streamedObject);
	shader->passParamData = std::move(data);
}
} /* namespace nextar */
