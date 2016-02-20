/*
 * CmdCleanDepth.h
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#ifndef PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDCLEANDEPTH_H_
#define PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDCLEANDEPTH_H_

#include <RenderSystemScript.h>
#include <CommandHandler.h>

namespace RenderSystemScript {

class CmdCleanDepth: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdCleanDepth);

	virtual void BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

} /* namespace RenderSystemScript */

#endif /* PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDCLEANDEPTH_H_ */
