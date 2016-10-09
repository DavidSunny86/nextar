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
ShaderTemplate::ShaderTemplate(const StringID name, const StringID factory) : AssetTemplate(name, factory),
renderFlags(0) {
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

String ShaderTemplate::GetPoxyAssetExtension() const {
	return "nexfx";
}

void ShaderTemplate::_AppendCompilerOption(const String& options,
	String& outCompilerOptions) {

	auto it = macros.find(options);
	if (it != macros.end()) {
		ConstMultiStringHelper h((*it).second.activateDefines);
		if (h.Length() > 0)
			StringUtils::PushBackWordList(outCompilerOptions, (*it).second.activateDefines);
	}
}

String ShaderTemplate::GetHashNameFromOptions(const set<String>::type& opt) {
	String word;
	String hashCode;
	set<uint32>::type listOfUniques;
	for (auto& e : opt) {
		auto it = registeredOptions.find(e);
		if (it != registeredOptions.end()) {
			listOfUniques.insert((*it).second);
		} else {
			Error("Option not registered or not supported: " + e);
		}
	}
	for(auto i : listOfUniques) {
		String intVal = Convert::ToString(i, ' ', std::ios::hex)+".";
		hashCode += intVal;
	}

	return hashCode;
}

ShaderAssetPtr& ShaderTemplate::GetShaderUnit(const StringUtils::WordList& definedParms,
	const StringUtils::WordList& enabledOptions) {
	// clean-up the options
	set<String>::type listOfOptions;
	String value;
	auto optIt = ConstMultiStringHelper::It(definedParms);
	while (optIt.HasNext(value)) {
		auto it = paramActivationOptions.find(value);
		if (it != paramActivationOptions.end() && (*it).second.length() > 0) {
			auto o = ConstMultiStringHelper::It((*it).second);
			while (o.HasNext(value))
				listOfOptions.insert(value);
		}		
	}

	auto enIt = ConstMultiStringHelper::It(enabledOptions);
	while (enIt.HasNext(value)) {
		if (value.length() > 0)
			listOfOptions.insert(value);
	}

	String hash = GetHashNameFromOptions(listOfOptions);
	auto it = shaders.find(hash);
	if (it != shaders.end())
		return (*it).second.shaderObject;
	return CreateShader(hash, listOfOptions);
}

ShaderAssetPtr& ShaderTemplate::CreateShader(const String& hash,
	const set<String>::type& options) {
	String compilerOptions;
	for (auto& e : options)
		_AppendCompilerOption(e, compilerOptions);
	String uniqueName = GetName() + "#" + Convert::ToString((uint32)shaders.size(), ' ', std::ios::hex);
	ShaderAssetPtr shaderObject = ShaderAsset::Traits::Instance(NamedObject::AsyncStringID(uniqueName), nullptr, GetGroup() );
	if (!shaderObject->AsyncIsLoaded()) {
		// load and prepare the shader object
		
		ShaderFromTemplate loader(this, compilerOptions);
		InputStreamPtr input;
		StreamInfo info(&loader, input);
		shaderObject->RequestLoad(info);
	}
	ShaderUnit &unit = shaders[hash];
	unit.shaderObject = shaderObject;
	unit.compilationOptions = compilerOptions;
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

void ShaderTemplate::RegisterOptions(const String& options) {
	if (options.length() <= 0) 
		return;
	String value;
	ConstMultiStringHelper::Iterator it = ConstMultiStringHelper::It(options);
	while (it.HasNext(value)) {
		auto it = registeredOptions.find(value);
		if (it == registeredOptions.end())
			registeredOptions.insert(CompilerMacroMap::value_type(value, 
			(uint32)registeredOptions.size()));
	}
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

void ShaderTemplate::LoadStreamRequest::AddSampler(const String& samplerName,
		const TextureUnitParams& unit) {
	auto& state = current->textureUnitStates[samplerName];
	state.params = unit;
}

void ShaderTemplate::LoadStreamRequest::AddTextureUnit(const String& unitName,
	const TextureUnitDesc& params) {
	auto& state = current->textureUnitStates[params.samplerName];
	state.unitsBound += unitName;
	state.unitsBound += '.';
	state.unitsBound += ShaderParameter::GetContextKey(params.context);
	state.unitsBound += ';';
}

void ShaderTemplate::LoadStreamRequest::AddParam(const String& param, const ParameterDesc& pdesc) {
	if (param.length() <= 0)
		return;
	ShaderTemplate* shader = static_cast<ShaderTemplate*>(GetStreamedObject());
	auto& parameter = shader->parameters[param];
	parameter.context = pdesc.context;
	parameter.description = pdesc.description;
	parameter.type = pdesc.type;
	if (pdesc.activateOption.length() > 0)	{ 
		shader->_BindParamToOp(param, pdesc.activateOption);
		shader->RegisterOptions(pdesc.activateOption);
	}
}

void ShaderTemplate::LoadStreamRequest::AddCompilerOption(const String& name, const String& options) {
	ShaderTemplate* shader = static_cast<ShaderTemplate*>(GetStreamedObject());
	auto& macro = shader->macros[name];
	StringUtils::PushBackWordList(macro.activateDefines, options);
	shader->RegisterOptions(name);
}

void ShaderTemplate::LoadStreamRequest::AddSemanticBinding(const String& var,
		AutoParamName name) {
	current->semanticMap[var] = name;
}

void ShaderTemplate::LoadStreamRequest::SetRenderFlags(uint32 rf) {
	ShaderTemplate* shader = static_cast<ShaderTemplate*>(GetStreamedObject());
	shader->SetRenderFlags(rf);
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
			request->AddSamplerUnit(tu.second.params, tu.second.unitsBound);
		}

		auto range = p.sourceMap.equal_range(language);
		for(auto i = range.first; i != range.second; ++i) {
			String sourceCpy = (*i).second.second;
			request->SetProgramSource((*i).second.first, std::move(sourceCpy));
		}

		request->SetSemanticMap(p.semanticMap);
	}


	request->SetRenderQueueFlags(parent->renderFlags);
	request->SetCompleted(true);
}

} /* namespace nextar */
