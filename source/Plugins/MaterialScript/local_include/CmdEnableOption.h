/*
 * CmdEnableOption.h
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_CMDENABLEOPTION_H_
#define PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_CMDENABLEOPTION_H_

#include <MaterialScript.h>

namespace MaterialScript {

class CmdEnableOption: public nextar::CommandHandler {

public:
	NEX_SINGLE_INSTANCE(CmdEnableOption);

	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

} /* namespace MaterialScript */

#endif /* PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_CMDENABLEOPTION_H_ */
