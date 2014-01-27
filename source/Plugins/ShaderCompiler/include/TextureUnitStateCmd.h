/*
 * TextureUnitStateCmd.h
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#ifndef TEXTUREUNITSTATECMD_H_
#define TEXTUREUNITSTATECMD_H_

#include "CommonTypes.h"

namespace ShaderCompiler {

	class TextureUnitStateCmd :
		public CommandDelegate {
		static TextureUnitStateCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);

	};

	struct TextureUnitStateListener :
			public ScriptParser::BlockListener,
			public ScriptParser::StatementListener {
		//static CommandNamePair commands[];
		//static const size_t commandCount;
		ShaderAsset::StreamRequest* shader;
		inline TextureUnitStateListener(ShaderAsset::StreamRequest* s) : shader(s) {}
		virtual void EnterBlock(ScriptParser::BlockContext& block);
		virtual void EnterStatement(ScriptParser::StatementContext& statement);
	};

	struct UnitCmd :
			public CommandDelegate {
		static UnitCmd command;
		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement);
	};
} /* namespace ShaderCompiler */
#endif /* TEXTUREUNITSTATECMD_H_ */
