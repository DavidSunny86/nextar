/*
 * ShaderScriptImplv10.cpp
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#include <ShaderScriptImplv10.h>
#include <ShaderScriptContext.h>
#include <CmdShader.h>
#include <CmdBlendState.h>
#include <CmdDepthStencilState.h>
#include <CmdProgram.h>
#include <CmdRasterState.h>
#include <CmdTextureState.h>
#include <LanguageTranslator.h>

namespace ShaderScript {

ShaderScriptImplv1_0::ShaderScriptImplv1_0() : _rootTranslator(&_rootTranslatorCommandHandler), _rootShaderScript(nullptr) {
	// TODO Auto-generated constructor stub

}

ShaderScriptImplv1_0::~ShaderScriptImplv1_0() {
	// TODO Auto-generated destructor stub
}

void ShaderScriptImplv1_0::UnregisterDictionary() {
	CommandDictionaryArchive::Instance().UnregisterDictionary("ConstBufferScript");
	CommandDictionaryArchive::Instance().UnregisterDictionary("ShaderScript");
}

void ShaderScriptImplv1_0::RegisterDictionary() {
	CommandDictionary* dict = CommandDictionaryArchive::Instance().RegisterDictionary("ShaderScript",
			&_rootShaderScript);
	dict->RegisterRegionHandler("predefs", RegProgramRegion::InstancePtr());
	dict->RegisterRegionHandler("prefix", RegProgramRegion::InstancePtr());
	dict->RegisterRegionHandler("main", RegShaderRegion::InstancePtr());
	auto root = RegShaderRegion::InstancePtr();
	root->RegisterHandler("shader", CmdShader::InstancePtr());
	root->RegisterHandler("shader.inherit", CmdInherit::InstancePtr());
	root->RegisterHandler("shader.tags", CmdTags::InstancePtr());
	root->RegisterHandler("shader.pass", CmdPass::InstancePtr());
	root->RegisterHandler("shader.pass.import-cbuffer", CmdImportConstBuffer::InstancePtr());
	root->RegisterHandler("shader.pass.cbuffer", CmdConstBuffer::InstancePtr());
	root->RegisterHandler("shader.pass.cbuffer.declare", CmdDeclare::InstancePtr());
	root->RegisterHandler("shader.pass.blend-state", CmdBlendState::InstancePtr());
	root->RegisterHandler("shader.pass.blend-state.target", CmdTarget::InstancePtr());
	root->RegisterHandler("shader.pass.blend-state.alpha-to-coverage", CmdAlphaToCoverage::InstancePtr());
	root->RegisterHandler("shader.pass.depth-stencil-state", CmdDepthStencilState::InstancePtr());
	root->RegisterHandler("shader.pass.depth-stencil-state.depth-state", CmdDepthState::InstancePtr());
	root->RegisterHandler("shader.pass.depth-stencil-state.stencil-state", CmdStencilState::InstancePtr());
	root->RegisterHandler("shader.pass.program", CmdProgram::InstancePtr());
	root->RegisterHandler("shader.pass.program.activate", CmdActivate::InstancePtr());
	root->RegisterHandler("shader.pass.program.domain-program", CmdDomainProgram::InstancePtr());
	root->RegisterHandler("shader.pass.program.hull-program", CmdHullProgram::InstancePtr());
	root->RegisterHandler("shader.pass.program.geometry-program", CmdGeometryProgram::InstancePtr());
	root->RegisterHandler("shader.pass.program.vertex-program", CmdVertexProgram::InstancePtr());
	root->RegisterHandler("shader.pass.program.fragment-program", CmdFragmentProgram::InstancePtr());
	root->RegisterHandler("shader.pass.raster-state", CmdRasterState::InstancePtr());
	root->RegisterHandler("shader.pass.raster-state.depth-clip", CmdDepthClip::InstancePtr());
	root->RegisterHandler("shader.pass.raster-state.depth-bias", CmdDepthBias::InstancePtr());
	root->RegisterHandler("shader.pass.raster-state.scissor", CmdScissor::InstancePtr());
	root->RegisterHandler("shader.pass.raster-state.raster", CmdRaster::InstancePtr());
	root->RegisterHandler("shader.pass.raster-state.anti-aliasing", CmdAntiAliasing::InstancePtr());
	root->RegisterHandler("shader.pass.texture-state", CmdTextureState::InstancePtr());
	root->RegisterHandler("shader.pass.texture-state.unit", CmdUnit::InstancePtr());
	root->RegisterHandler("shader.pass.texture-state.sampler", CmdSampler::InstancePtr());
	root->RegisterHandler("shader.pass.texture-state.sampler.address", CmdAddress::InstancePtr());
	root->RegisterHandler("shader.pass.texture-state.sampler.anisotropy", CmdAnisotropy::InstancePtr());
	root->RegisterHandler("shader.pass.texture-state.sampler.texture-lod", CmdTextureLod::InstancePtr());
	root->RegisterHandler("shader.pass.texture-state.sampler.compare", CmdCompare::InstancePtr());
	root->RegisterHandler("shader.pass.texture-state.sampler.filter", CmdFilter::InstancePtr());

	dict = CommandDictionaryArchive::Instance().RegisterDictionary("ConstBufferScript",
				&_rootTranslator);
	_rootTranslator.RegisterHandler("cbuffer", LanguageTranslator::CmdConstBuffer::InstancePtr());
	_rootTranslator.RegisterHandler("cbuffer.declare", LanguageTranslator::CmdDeclare::InstancePtr());
}

void ShaderScriptImplv1_0::Configure(const nextar::Config&) {
}

void ShaderScriptImplv1_0::Load(nextar::InputStreamPtr& input,
		nextar::AssetLoader& shader) {
	auto request = static_cast<ShaderTemplate::LoadStreamRequest*>(shader.GetRequestPtr());
	auto shaderPtr = static_cast<ShaderTemplate*>(request->GetStreamedObject());
	ShaderTemplate::ID shaderId;
	shaderPtr->GetID(shaderId);
	ShaderScriptContext context(request, shaderId);
	NeoCommandInterpreter::Execute("ShaderScript", input, &context);
	request->SetCompleted(true);
}

} /* namespace ShaderScript */
