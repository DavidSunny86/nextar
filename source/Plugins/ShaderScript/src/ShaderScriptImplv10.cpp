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

ShaderScriptImplv1_0::ShaderScriptImplv1_0() {
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
	dict->RegisterHandler("shader", CmdShader::InstancePtr());
	dict->RegisterHandler("shader.inherit", CmdInherit::InstancePtr());
	dict->RegisterHandler("shader.tags", CmdTags::InstancePtr());
	dict->RegisterHandler("shader.pass", CmdPass::InstancePtr());
	dict->RegisterHandler("shader.pass.import-cbuffer", CmdImportConstBuffer::InstancePtr());
	dict->RegisterHandler("shader.pass.cbuffer", CmdConstBuffer::InstancePtr());
	dict->RegisterHandler("shader.pass.cbuffer.declare", CmdDeclare::InstancePtr());
	dict->RegisterHandler("shader.pass.blend-state", CmdBlendState::InstancePtr());
	dict->RegisterHandler("shader.pass.blend-state.target", CmdTarget::InstancePtr());
	dict->RegisterHandler("shader.pass.blend-state.alpha-to-coverage", CmdAlphaToCoverage::InstancePtr());
	dict->RegisterHandler("shader.pass.depth-stencil-state", CmdDepthStencilState::InstancePtr());
	dict->RegisterHandler("shader.pass.depth-stencil-state.depth-state", CmdDepthState::InstancePtr());
	dict->RegisterHandler("shader.pass.depth-stencil-state.stencil-state", CmdStencilState::InstancePtr());
	dict->RegisterHandler("shader.pass.program", CmdProgram::InstancePtr());
	dict->RegisterHandler("shader.pass.program.activate", CmdActivate::InstancePtr());
	dict->RegisterHandler("shader.pass.program.domain-program", CmdDomainProgram::InstancePtr());
	dict->RegisterHandler("shader.pass.program.hull-program", CmdHullProgram::InstancePtr());
	dict->RegisterHandler("shader.pass.program.geometry-program", CmdGeometryProgram::InstancePtr());
	dict->RegisterHandler("shader.pass.program.vertex-program", CmdVertexProgram::InstancePtr());
	dict->RegisterHandler("shader.pass.program.fragment-program", CmdFragmentProgram::InstancePtr());
	dict->RegisterHandler("shader.pass.raster-state", CmdRasterState::InstancePtr());
	dict->RegisterHandler("shader.pass.raster-state.depth-clip", CmdDepthClip::InstancePtr());
	dict->RegisterHandler("shader.pass.raster-state.depth-bias", CmdDepthBias::InstancePtr());
	dict->RegisterHandler("shader.pass.raster-state.scissor", CmdScissor::InstancePtr());
	dict->RegisterHandler("shader.pass.raster-state.raster", CmdRaster::InstancePtr());
	dict->RegisterHandler("shader.pass.raster-state.anti-aliasing", CmdAntiAliasing::InstancePtr());
	dict->RegisterHandler("shader.pass.texture-state", CmdTextureState::InstancePtr());
	dict->RegisterHandler("shader.pass.texture-state.unit", CmdUnit::InstancePtr());
	dict->RegisterHandler("shader.pass.texture-state.sampler", CmdSampler::InstancePtr());
	dict->RegisterHandler("shader.pass.texture-state.sampler.address", CmdAddress::InstancePtr());
	dict->RegisterHandler("shader.pass.texture-state.sampler.anisotropy", CmdAnisotropy::InstancePtr());
	dict->RegisterHandler("shader.pass.texture-state.sampler.texture-lod", CmdTextureLod::InstancePtr());
	dict->RegisterHandler("shader.pass.texture-state.sampler.compare", CmdCompare::InstancePtr());
	dict->RegisterHandler("shader.pass.texture-state.sampler.filter", CmdFilter::InstancePtr());

	dict = CommandDictionaryArchive::Instance().RegisterDictionary("ConstBufferScript",
				&_rootTranslator);
	dict->RegisterHandler("cbuffer", LanguageTranslator::CmdConstBuffer::InstancePtr());
	dict->RegisterHandler("cbuffer.declare", LanguageTranslator::CmdDeclare::InstancePtr());
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
