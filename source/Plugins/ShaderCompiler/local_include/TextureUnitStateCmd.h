/*
 * TextureUnitStateCmd.h
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#ifndef TEXTUREUNITSTATECMD_H_
#define TEXTUREUNITSTATECMD_H_

#include <ProjectModelHeaders.h>
#include <ShaderTemplate.h>
#include <CommonTypes.h>

namespace ShaderCompiler {


class TextureUnitStateListener: public ScriptParser::BlockListener,
		public ScriptParser::StatementListener {
public:
	static CommandNamePair commands[];
	static const size_t commandCount;
	
	ShaderTemplate::LoadStreamRequest* shader;
	inline TextureUnitStateListener(ShaderTemplate::LoadStreamRequest* s) :
			shader(s) {
	}

	virtual void EnterBlock(ScriptParser::BlockContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& statement);

	static void UnitCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void SamplerCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
};

class SamplerListener : public ScriptParser::BlockListener,
	public ScriptParser::StatementListener {
		
public:

	TextureUnitParams params;

	static CommandNamePair commands[];
	static const size_t commandCount;

	virtual void EnterBlock(ScriptParser::BlockContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& statement);
	
	static void AddressUCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void AddressVCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void AddressWCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void AnisotropyCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void BorderColorCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void LodBiasCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void MagFilterCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void MaxLodCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void MinFilterCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void MinLodCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
	static void CompareCmd_Execute(int parentType, void* parentParam,
		ScriptParser::StatementContext& statement);
};

} /* namespace ShaderCompiler */
#endif /* TEXTUREUNITSTATECMD_H_ */
