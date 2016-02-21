/*
 * CmdCreateBuffer.h
 *
 *  Created on: 17-Feb-2016
 *      Author: obhi
 */

#ifndef PLUGINS_RENDERSYSTEMSCRIPT_CMDCREATEBUFFER_H_
#define PLUGINS_RENDERSYSTEMSCRIPT_CMDCREATEBUFFER_H_

#include <RenderSystemScript.h>
#include <BlockCommandHandler.h>

namespace RenderSystemScript {

class RenderScriptContext;
class CmdCreateBuffer : public nextar::BlockCommandHandler {
	NEX_LOG_HELPER(CmdCreateBuffer);
public:
	NEX_SINGLE_INSTANCE(CmdCreateBuffer);

	virtual void BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
	virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const;

	static RenderTargetPtr CreateTarget(RenderScriptContext* context, const Target& t, uint32 index);
	static void ParseDimension(const String& value, uint16& d, float& f);
};

} /* namespace RenderSystemScript */

#endif /* PLUGINS_RENDERSYSTEMSCRIPT_CMDCREATEBUFFER_H_ */
