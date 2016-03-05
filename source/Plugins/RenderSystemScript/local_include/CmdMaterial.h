/*
 * CmdMaterial.h
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#ifndef PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDMATERIAL_H_
#define PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDMATERIAL_H_

#include <RenderSystemScript.h>
#include <CommandHandler.h>

namespace RenderSystemScript {

class CmdMaterial: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdMaterial);

	virtual bool BeginExecute(CommandContext* pContext,
			const ASTCommand* command) const;
};

} /* namespace RenderSystemScript */

#endif /* PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDMATERIAL_H_ */
