/*
 * CmdShader.h
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_CMDSHADER_H_
#define PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_CMDSHADER_H_

#include <MaterialScript.h>

namespace MaterialScript {

class CmdShader : public CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdShader);

	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;

};

} /* namespace MaterialScript */

#endif /* PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_CMDSHADER_H_ */
