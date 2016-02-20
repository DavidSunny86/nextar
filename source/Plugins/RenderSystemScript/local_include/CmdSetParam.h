/*
 * CmdSetParam.h
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#ifndef PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDSETPARAM_H_
#define PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDSETPARAM_H_

#include <RenderSystemScript.h>
#include <CommandHandler.h>

namespace RenderSystemScript {

class CmdSetParam: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdSetParam);

	virtual void BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

} /* namespace RenderSystemScript */

#endif /* PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDSETPARAM_H_ */
