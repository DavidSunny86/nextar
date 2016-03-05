/*
 * CmdLayer.h
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_CMDLAYER_H_
#define PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_CMDLAYER_H_

#include <MaterialScript.h>
#include <CommandHandler.h>

namespace MaterialScript {

class CmdLayer: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdLayer);

	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

} /* namespace MaterialScript */

#endif /* PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_CMDLAYER_H_ */
