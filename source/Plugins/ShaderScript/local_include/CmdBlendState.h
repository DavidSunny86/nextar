/*
 * CmdBlendState.h
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERSCRIPT_CMDBLENDSTATE_H_
#define PLUGINS_SHADERSCRIPT_CMDBLENDSTATE_H_

#include <ShaderScript.h>
#include <BlockCommandHandler.h>

namespace ShaderScript {

class CmdBlendState: public nextar::BlockCommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdBlendState);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
	virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const;

};

class CmdTarget: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdTarget);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;

	static const RenderTargetBlendOp _alphaBlending;
};

class CmdAlphaToCoverage: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdAlphaToCoverage);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

} /* namespace ShaderScript */

#endif /* PLUGINS_SHADERSCRIPT_CMDBLENDSTATE_H_ */
