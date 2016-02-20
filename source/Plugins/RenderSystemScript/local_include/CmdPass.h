/*
 * CmdPass.h
 *
 *  Created on: 14-Feb-2016
 *      Author: obhi
 */

#ifndef PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDPASS_H_
#define PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDPASS_H_

#include <RenderSystemScript.h>
#include <BlockCommandHandler.h>

namespace RenderSystemScript {

class CmdPass: public nextar::BlockCommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdPass);

	virtual void BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
	virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const;

protected:

};

} /* namespace RenderSystemScript */

#endif /* PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDPASS_H_ */
