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
#include <CmdClean.h>
#include <CmdEnabled.h>
#include <CmdCreateBuffer.h>
#include <BaseRenderPass.h>


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
	_root.RegisterHandler("pass", CmdPass::InstancePtr());
	_root.RegisterHandler("pass.clean", CmdClean::InstancePtr());
	_root.RegisterHandler("pass.material", CmdMaterial::InstancePtr());
	_root.RegisterHandler("pass.set-param", CmdSetParam::InstancePtr());
	_root.RegisterHandler("pass.render-to", CmdRenderTo::InstancePtr());
	_root.RegisterHandler("pass.enabled", CmdEnabled ::InstancePtr());
	_root.RegisterHandler("create-buffer", CmdCreateBuffer::InstancePtr());
	_root.RegisterHandler("create-buffer.target", CmdTarget::InstancePtr());
}

bool RenderScriptStreamer::Load(RenderSystem& s, InputStreamPtr& stream) {
	RenderScriptContext context(s);
	NeoCommandInterpreter::Execute("RenderSystem", stream, &context);
	return true;
}

bool RenderScriptStreamer::Save(RenderSystem& s, OutputStreamPtr& stream) {
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	return true;
}

} /* namespace nextar */
