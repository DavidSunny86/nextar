/*
 * ShaderTemplate.cpp
 *
 *  Created on: 19-Jul-2014
 *      Author: obhi
 */

#include <NexProjectModel.h>
#include <ShaderTemplate.h>

namespace nextar {

/********************************************
 * ShaderTemplate
 *********************************************/
ShaderTemplate::ShaderTemplate(const StringID name, const StringID factory) : AssetTemplate(name, factory) {
}

ShaderTemplate::~ShaderTemplate() {

}

nextar::StreamRequest* ShaderTemplate::CreateStreamRequestImpl(bool load) {
	// @urgent return for save
	if (load)
		return NEX_NEW(StreamRequest(this));
	else
		return NEX_ALLOC_T(AssetStreamRequest, MEMCAT_GENERAL);
}

void ShaderTemplate::DestroyStreamRequestImpl(
		nextar::StreamRequest*& streamRequest, bool load) {
	//NEX_DELETE(static_cast<StreamRequest*>(streamRequest));
}

String ShaderTemplate::GetHashNameFromOptions(const StringUtils::WordList& opt) {
	StringUtils::TokenIterator it = 0;
	String word;
	String hashCode;
	vector<uint32>::type listOfUniques;
	while( (it = StringUtils::NextWord(opt, word, it)) != String::npos ) {
		MacroTable::iterator it = macros.find(word);
		if (it != macros.end()) {
			listOfUniques.push_back((*it).second.index);
		}
	}

	std::sort(listOfUniques.begin(), listOfUniques.end());
	for(auto i : listOfUniques) {
		String intVal = Convert::ToString(i, ' ', std::ios::hex)+".";
		hashCode += intVal;
	}

	return hashCode;
}

ShaderAssetPtr& ShaderTemplate::GetShaderUnit(const StringUtils::WordList& options) {

	String hash = GetHashNameFromOptions(options);
	auto it = shaders.find(hash);
	if (it != shaders.end())
		return (*it).second.shaderObject;
	return CreateShader(hash, options);
}

ShaderAssetPtr& ShaderTemplate::CreateShader(const String& hash,
		const StringUtils::WordList& options) {
	String uniqueName = GetName() + "#" + Convert::ToString((uint32)shaders.size(), ' ', std::ios::hex);
	ShaderAssetPtr shaderObject = ShaderAsset::Traits::Instance(NamedObject::AsyncStringID(uniqueName), nullptr, GetGroup() );
	// load and prepare the shader object
	auto streamRequest = static_cast<AssetStreamRequest*>(
			shaderObject->GetStreamRequest(true));

	ShaderFromTemplate loader(this, options);
	streamRequest->SetManualLoader(&loader);
	shaderObject->Load(false);

	return shaderObject;
}

void ShaderTemplate::NotifyAssetLoaded() {
	AssetTemplate::NotifyAssetLoaded();
}

void ShaderTemplate::NotifyAssetUnloaded() {
	AssetTemplate::NotifyAssetUnloaded();
}

void ShaderTemplate::NotifyAssetUpdated() {
	AssetTemplate::NotifyAssetUpdated();
}

void ShaderTemplate::NotifyAssetSaved() {
	AssetTemplate::NotifyAssetSaved();
}

void ShaderTemplate::UnloadImpl() {
	passes.clear();
	shaders.clear();
	parameters.clear();
	macros.clear();
}

/********************************************
 * ShaderTemplate::StreamRequest
 *********************************************/
ShaderTemplate::StreamRequest::StreamRequest(ShaderTemplate* shaderTemplate) :
		AssetStreamRequest(shaderTemplate),
		current(nullptr) {
}

void ShaderTemplate::StreamRequest::AddPass(StringID name) {
	passes.resize(passes.size()+1);
	current = &passes.back();
	current->name = name;
}
void ShaderTemplate::StreamRequest::SetProgramSource(Pass::ProgramStage stage,
		RenderManager::ShaderProgramLanguage lang, String&& source) {
	uint32 key = ((((uint32)stage)<<16 & 0xffff0000))|(((uint32)lang) & 0xffff);
	current->sourceMap[key] = std::move(source);
}

void ShaderTemplate::StreamRequest::SetRasterState(RasterState& state) {
	current->rasterState = state;
}

void ShaderTemplate::StreamRequest::SetBlendState(BlendState& state) {
	current->blendState = state;
}

void ShaderTemplate::StreamRequest::SetDepthStencilState(
		DepthStencilState& state) {
	current->depthStencilState = state;
}

void ShaderTemplate::StreamRequest::AddTextureUnit(const String& unitName,
		TextureUnitParams& unit, URL& defaultTexturePath) {
	auto& state = current->textureUnitStates[unitName];
	state.defaultTexturePath = defaultTexturePath;
	state.params = unit;
}

void ShaderTemplate::StreamRequest::AddParam(const String& param,
		const String& name, const String& description, ParamDataType type) {
	auto& parameter = parameters[param];
	auto namePair = StringUtils::Split(name, '.');

	parameter.uiName = StringUtils::FormatCamelCaseString(namePair.second);
	parameter.uiDescription = description;
	parameter.type = type;
	parameter.catagory = StringUtils::FormatCamelCaseString(namePair.first);
}

void ShaderTemplate::StreamRequest::AddMacro(const String& param,
		const String& name, const String& description) {
	auto& macro = macros[name];
	if (macro.index == -1) {
		macro.index = macros.size();
		macro.uiName = StringUtils::FormatCamelCaseString(name);
		macro.uiDescription = description;
	}
}


/********************************************
 * ShaderTemplate::ShaderFromTemplate
 *********************************************/
ShaderTemplate::ShaderFromTemplate::ShaderFromTemplate(ShaderTemplate* _parent,
		const StringUtils::WordList& _compilationOptions) : parent(_parent),
				compilationOptions(_compilationOptions) {
}

void ShaderTemplate::ShaderFromTemplate::Load(InputStreamPtr& stream, AssetLoader& loader) {
	ShaderAsset::StreamRequest* request = static_cast<ShaderAsset::StreamRequest*>(
			loader.GetRequestPtr());
	request->SetCompilationOptions(compilationOptions);
	auto passes = parent->passes;
	uint8 language = (uint8)RenderManager::Instance().GetProgramLanguage();
	for(auto &p : passes) {
		request->AddPass(p.name);
		request->SetBlendState(p.blendState);
		request->SetDepthStencilState(p.depthStencilState);
		request->SetRasterState(p.rasterState);
		for(auto& tu : p.textureUnitStates) {
			if (tu.second.defaultTexturePath != URL::Invalid) {
				StringID name = NamedObject::AsyncStringID(
						tu.second.defaultTexturePath.GetComputedName());
				tu.second.defaultTexture = TextureAsset::Traits::Instance(name, tu.second.defaultTexturePath);
			}
			request->AddTextureUnit(tu.first, tu.second.params, tu.second.defaultTexture.GetPtr());
		}

		for(uint32 i = 0; i < (uint32)Pass::ProgramStage::STAGE_COUNT; ++i) {
			uint32 key = ((i<<16 & 0xffff0000))|(language & 0xffff);
			auto srcIt = p.sourceMap.find(key);
			if (srcIt != p.sourceMap.end() && (*srcIt).second.length()) {
				String sourceCpy = (*srcIt).second;
				request->SetProgramSource((Pass::ProgramStage)i, std::move(sourceCpy));
			}
		}
	}
}

} /* namespace nextar */
