/*
 * CmdRenderTo.h
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#ifndef PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDRENDERTO_H_
#define PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDRENDERTO_H_

#include <RenderSystemScript.h>
#include <BlockCommandHandler.h>
#include <CommandHandler.h>

namespace RenderSystemScript {

class CmdRenderTo: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdRenderTo);

	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

} /* namespace RenderSystemScript */

#endif /* PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDRENDERTO_H_ */
