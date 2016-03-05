/*
 * MaterialCmd.h
 *
 *  Created on: 04-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_CMDMATERIAL_H_
#define PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_CMDMATERIAL_H_

#include <MaterialScript.h>
#include <BlockCommandHandler.h>

namespace MaterialScript {

class CmdMaterial: public nextar::BlockCommandHandler {
public:

	NEX_SINGLE_INSTANCE(CmdMaterial);

	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
	virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const;

};

} /* namespace MaterialScript */

#endif /* PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_CMDMATERIAL_H_ */
