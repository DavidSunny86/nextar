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
	if (load)
		return NEX_NEW(LoadStreamRequest(this));
	else
		return NEX_NEW(SaveStreamRequest(this));
}

uint32 ShaderTemplate::GetClassID() const {
	return CLASS_ID;
}

uint32 ShaderTemplate::GetProxyID() const {
	return ShaderAsset::CLASS_ID;
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
	if (!shaderObject->AsyncIsLoaded()) {
		// load and prepare the shader object
		ShaderFromTemplate loader(this, options);
		InputStreamPtr input;
		StreamInfo info(&loader, input);
		shaderObject->RequestLoad(info);
	}
	ShaderUnit &unit = shaders[hash];
	unit.shaderObject = shaderObject;
	unit.compilationOptions = options;
	return unit.shaderObject;
}

StreamNotification ShaderTemplate::NotifyAssetSavedImpl(StreamRequest* ) {
	// make sure these shaders are loaded from the current saved path
	for (auto &s : shaders) {
		if(s.second.shaderObject)
			s.second.shaderObject->SetAssetLocator(GetAssetLocator());
	}
	return StreamNotification::NOTIFY_COMPLETED_AND_READY;
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
ShaderTemplate::LoadStreamRequest::LoadStreamRequest(ShaderTemplate* shaderTemplate) :
		AssetStreamRequest(shaderTemplate),
		current(nullptr) {
}

void ShaderTemplate::LoadStreamRequest::AddPass(StringID name) {
	ShaderTemplate* shader = static_cast<ShaderTemplate*>(GetStreamedObject());
	auto& passes = shader->passes;
	passes.resize(passes.size()+1);
	current = &(passes.back());
	current->name = name;
}

void ShaderTemplate::LoadStreamRequest::SetProgramSource(Pass::ProgramStage stage,
		RenderManager::ShaderLanguage lang, String&& source) {
	current->sourceMap.emplace(lang,
			std::pair<Pass::ProgramStage, String>(stage, std::move(source)));
}

void ShaderTemplate::LoadStreamRequest::SetRasterState(RasterState& state) {
	current->rasterState = state;
}

void ShaderTemplate::LoadStreamRequest::SetBlendState(BlendState& state) {
	current->blendState = state;
}

void ShaderTemplate::LoadStreamRequest::SetDepthStencilState(
		DepthStencilState& state) {
	current->depthStencilState = state;
}

void ShaderTemplate::LoadStreamRequest::AddTextureUnit(const String& unitName,
		TextureUnitParams& unit, URL& defaultTexturePath) {
	auto& state = current->textureUnitStates[unitName];
	state.defaultTexturePath = defaultTexturePath;
	state.params = unit;
}

void ShaderTemplate::LoadStreamRequest::AddParam(const String& param,
		const String& name, const String& description, ParamDataType type) {
	ShaderTemplate* shader = static_cast<ShaderTemplate*>(GetStreamedObject());
	auto& parameter = shader->parameters[param];
	auto namePair = StringUtils::Split(name, '.');

	parameter.uiName = StringUtils::FormatName(namePair.second);
	parameter.uiDescription = description;
	parameter.type = type;
	parameter.catagory = StringUtils::FormatName(namePair.first);
}

void ShaderTemplate::LoadStreamRequest::AddMacro(const String& param,
		const String& name, const String& description) {
	ShaderTemplate* shader = static_cast<ShaderTemplate*>(GetStreamedObject());
	auto& macro = shader->macros[name];
	if (macro.index == -1) {
		macro.index = shader->macros.size();
		macro.uiName = StringUtils::FormatName(name);
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
	RenderManager::ShaderLanguage language = RenderManager::Instance().GetProgramLanguage();
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

		auto range = p.sourceMap.equal_range(language);
		for(auto i = range.first; i != range.second; ++i) {
			String sourceCpy = (*i).second.second;
			request->SetProgramSource((*i).second.first, std::move(sourceCpy));
		}
	}

	request->SetRenderQueueFlags(parent->renderFlags);
}

} /* namespace nextar */
