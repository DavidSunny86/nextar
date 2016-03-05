/*
 * CmdDepthStencilState.h
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERSCRIPT_CMDDEPTHSTENCILSTATE_H_
#define PLUGINS_SHADERSCRIPT_CMDDEPTHSTENCILSTATE_H_

#include <ShaderScript.h>
#include <BlockCommandHandler.h>

namespace ShaderScript {

class CmdDepthStencilState: public nextar::BlockCommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdDepthStencilState);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
	virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdDepthState: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdDepthState);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdStencilState: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdStencilState);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

} /* namespace ShaderScript */

#endif /* PLUGINS_SHADERSCRIPT_CMDDEPTHSTENCILSTATE_H_ */
