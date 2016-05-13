/*
 * CmdShader.h
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERSCRIPT_LOCAL_INCLUDE_CMDSHADER_H_
#define PLUGINS_SHADERSCRIPT_LOCAL_INCLUDE_CMDSHADER_H_

#include <ShaderScript.h>
#include <BlockCommandHandler.h>

namespace ShaderScript {

class CmdShader: public nextar::BlockCommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdShader);

	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
	virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdInherit: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdInherit);
	map<String, uint32>::type tagsMap;

	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdTags: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdTags);
	map<String, uint32>::type tagsMap;

	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdPass: public nextar::BlockCommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdPass);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
	virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdImportConstBuffer :public nextar::BlockCommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdImportConstBuffer);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdConstBuffer :public BlockCommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdConstBuffer);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
	virtual void EndExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class CmdDeclare : public CommandHandler {
	NEX_LOG_HELPER(CmdDeclare);
public:
	NEX_SINGLE_INSTANCE(CmdDeclare);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

} /* namespace ShaderScript */

#endif /* PLUGINS_SHADERSCRIPT_LOCAL_INCLUDE_CMDSHADER_H_ */
