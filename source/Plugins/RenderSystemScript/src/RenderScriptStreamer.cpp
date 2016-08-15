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
#include <CmdCreateBuffer.h>


namespace RenderSystemScript {

enum RenderScriptHeaders {
	FILE_MAGIC_RSCRIPT = 0xff817191,
	RSCRIPT_PASS_DATA = 0x55ee,
	RSCRIPT_BEGIN_PASS = 0x51fe,
};

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
	dict->RegisterHandler("pass.clean", CmdClean::InstancePtr());
	dict->RegisterHandler("pass.material", CmdMaterial::InstancePtr());
	dict->RegisterHandler("pass.set-param", CmdSetParam::InstancePtr());
	dict->RegisterHandler("pass.render-to", CmdRenderTo::InstancePtr());
	dict->RegisterHandler("create-buffer", CmdCreateBuffer::InstancePtr());
	dict->RegisterHandler("create-buffer.target", CmdTarget::InstancePtr());
}

bool RenderScriptStreamer::Load(RenderSystem& s, InputStreamPtr& stream) {
	RenderScriptContext context(s);
	NeoCommandInterpreter::Execute("RenderSystem", stream, &context);
	return true;
}

void RenderScriptStreamer::_WriteVersion(OutputStreamPtr& stream) {
	OutputSerializer ser(stream);
	uint32 meshMagic = FILE_MAGIC_RSCRIPT;
	VersionID ver = NEX_MAKE_VERSION(1, 0, 0);
	String versionInfo = Convert::ToVersionString(ver);
	ser << meshMagic << versionInfo;
}

void RenderScriptStreamer::_WritePass(RenderPass* pass, ChunkOutputStream& ostr) {
	OutputSerializer& ser = ostr.BeginChunk(RSCRIPT_BEGIN_PASS);
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	ostr.EndChunk();
}

bool RenderScriptStreamer::Save(RenderSystem& s, OutputStreamPtr& stream) {

	_WriteVersion(stream);
	ChunkOutputStream ostr(stream);
		
	OutputSerializer& ser = ostr.BeginChunk(RSCRIPT_PASS_DATA);

	uint32 passCount = s.GetPassCount();
	ser << passCount;
	for (uint32 i = 0; i < passCount; ++i) {
		_WritePass(s.GetPass(i), ostr);
	}

	ostr.EndChunk();
	return false;
}


} /* namespace nextar */
