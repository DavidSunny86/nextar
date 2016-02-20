/*
 * CmdTarget.h
 *
 *  Created on: 17-Feb-2016
 *      Author: obhi
 */

#ifndef PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDTARGET_H_
#define PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDTARGET_H_

#include <RenderSystemScript.h>
#include <CommandHandler.h>

namespace RenderSystemScript {

class CmdTarget : public CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdTarget);

	virtual void BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

} /* namespace RenderSystemScript */

#endif /* PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDTARGET_H_ */
