/*
 * CmdProgram.h
 *
 *  Created on: 06-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERSCRIPT_LOCAL_INCLUDE_CMDPROGRAM_H_
#define PLUGINS_SHADERSCRIPT_LOCAL_INCLUDE_CMDPROGRAM_H_

#include <ShaderScript.h>
#include <BlockCommandHandler.h>

namespace ShaderScript {

class ShaderScriptContext;

class CmdProgram: public nextar::BlockCommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdProgram);
};

class CmdDefine: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdDefine);
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;
};

class SubProgramType: public nextar::CommandHandler {
public:
	SubProgramType(Pass::ProgramStage istage) : stage(istage) {}
	Pass::ProgramStage stage;
	virtual bool BeginExecute(CommandContext* pContext, const ASTCommand* command) const;

private:
	InputStreamPtr AddProgram(const String& programName, int i,
			const String& src, ShaderScriptContext* c) const;
};

class CmdDomainProgram: public SubProgramType {
public:
	NEX_SINGLE_INSTANCE(CmdDomainProgram);
	CmdDomainProgram() : SubProgramType(Pass::ProgramStage::STAGE_DOMAIN) {}
};

class CmdGeometryProgram: public SubProgramType {
public:
	NEX_SINGLE_INSTANCE(CmdGeometryProgram);
	CmdGeometryProgram() : SubProgramType(Pass::ProgramStage::STAGE_GEOMETRY) {}
};

class CmdHullProgram: public SubProgramType {
public:
	NEX_SINGLE_INSTANCE(CmdHullProgram);
	CmdHullProgram() : SubProgramType(Pass::ProgramStage::STAGE_HULL) {}
};

class CmdVertexProgram: public SubProgramType {
public:
	NEX_SINGLE_INSTANCE(CmdVertexProgram);
	CmdVertexProgram() : SubProgramType(Pass::ProgramStage::STAGE_VERTEX) {}
};

class CmdFragmentProgram: public SubProgramType {
public:
	NEX_SINGLE_INSTANCE(CmdFragmentProgram);
	CmdFragmentProgram() : SubProgramType(Pass::ProgramStage::STAGE_FRAGMENT) {}
};

} /* namespace ShaderScript */

#endif /* PLUGINS_SHADERSCRIPT_LOCAL_INCLUDE_CMDPROGRAM_H_ */
