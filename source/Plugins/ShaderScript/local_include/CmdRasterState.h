/*
 * CmdRasterState.h
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERSCRIPT_CMDRASTERSTATE_H_
#define PLUGINS_SHADERSCRIPT_CMDRASTERSTATE_H_

#include <ShaderScript.h>
#include <BlockCommandHandler.h>

namespace ShaderScript {

class CmdRasterState: public nextar::BlockCommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdRasterState);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
	virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdDepthClip: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdDepthClip);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdDepthBias: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdDepthBias);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdScissor: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdScissor);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdRaster: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdRaster);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdAntiAliasing: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdAntiAliasing);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

} /* namespace ShaderScript */

#endif /* PLUGINS_SHADERSCRIPT_CMDRASTERSTATE_H_ */
