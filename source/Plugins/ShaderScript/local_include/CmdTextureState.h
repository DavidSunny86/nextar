/*
 * CmdTextureState.h
 *
 *  Created on: 06-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERSCRIPT_CMDTEXTURESTATE_H_
#define PLUGINS_SHADERSCRIPT_CMDTEXTURESTATE_H_

#include <ShaderScript.h>
#include <BlockCommandHandler.h>

namespace ShaderScript {

class CmdTextureState: public nextar::BlockCommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdTextureState);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
	virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdUnit: public nextar::CommandHandler {
	NEX_LOG_HELPER(CmdUnit);
public:
	NEX_SINGLE_INSTANCE(CmdUnit);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdSampler: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdSampler);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
	virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const;

};

class CmdAddress: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdAddress);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdAnisotropy: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdAnisotropy);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdTextureLod: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdTextureLod);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdCompare: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdCompare);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdFilter: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdFilter);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

} /* namespace ShaderScript */

#endif /* PLUGINS_SHADERSCRIPT_CMDTEXTURESTATE_H_ */
