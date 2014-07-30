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

class TextureUnitStateCmd: public CommandDelegate {
public:
	static TextureUnitStateCmd command;
	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement);

};

class TextureUnitStateListener: public ScriptParser::BlockListener,
		public ScriptParser::StatementListener {
public:
	//static CommandNamePair commands[];
	//static const size_t commandCount;
	ShaderTemplate::LoadStreamRequest* shader;
	inline TextureUnitStateListener(ShaderTemplate::LoadStreamRequest* s) :
			shader(s) {
	}
	virtual void EnterBlock(ScriptParser::BlockContext& block);
	virtual void EnterStatement(ScriptParser::StatementContext& statement);
};

class UnitCmd: public CommandDelegate {
public:
	static UnitCmd command;
	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement);
};
} /* namespace ShaderCompiler */
#endif /* TEXTUREUNITSTATECMD_H_ */
