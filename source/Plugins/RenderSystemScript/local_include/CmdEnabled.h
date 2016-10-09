/*
 * CmdEnabled.h
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#ifndef PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDENABLED_H_
#define PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDENABLED_H_

#include <RenderSystemScript.h>
#include <CommandHandler.h>

namespace RenderSystemScript {

class CmdEnabled: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdEnabled);

	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;

protected:
};

} /* namespace RenderSystemScript */

#endif /* PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDENABLED_H_ */
