/*
 * PassScriptStreamerImpl.cpp
 *
 *  Created on: 07-Feb-2016
 *      Author: obhi
 */


#include <RenderScriptStreamer.h>
#include <RenderScriptContext.h>
#include <CmdPass.h>
#include <CmdMaterial.h>
#include <CmdRenderTo.h>
#include <CmdSetParam.h>
#include <CmdTarget.h>
#include <CmdCleanDepth.h>
#include <CmdCleanColor.h>
#include <CmdCreateBuffer.h>

namespace RenderSystemScript {

RenderScriptStreamer::RenderScriptStreamer() {

}

RenderScriptStreamer::~RenderScriptStreamer() {
}

void RenderScriptStreamer::UnregisterDictionary() {
	CommandDictionaryArchive::Instance().UnregisterDictionary("RenderSystem");
}

void RenderScriptStreamer::RegisterDictionary() {
	CommandDictionary* dict = CommandDictionaryArchive::Instance().RegisterDictionary("RenderSystem",
			&_root);
	dict->RegisterHandler("pass", CmdPass::InstancePtr());
	dict->RegisterHandler("create-buffer", CmdCreateBuffer::InstancePtr());
	dict->RegisterHandler("pass.clean-color", CmdCleanColor::InstancePtr());
	dict->RegisterHandler("pass.clean-depth", CmdCleanDepth::InstancePtr());
	dict->RegisterHandler("pass.material", CmdMaterial::InstancePtr());
	dict->RegisterHandler("pass.set-param", CmdSetParam::InstancePtr());
	dict->RegisterHandler("pass.render-to", CmdRenderTo::InstancePtr());
	dict->RegisterHandler("create-buffer.target", CmdTarget::InstancePtr());
}

bool RenderScriptStreamer::Load(RenderSystem& s, InputStreamPtr& stream) {

	RenderScriptContext context(s);
	NeoCommandInterpreter::Execute("RenderSystem", stream, &context);

	return true;

}

} /* namespace nextar */
