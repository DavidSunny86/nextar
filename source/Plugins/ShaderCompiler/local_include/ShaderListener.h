/*
 * ShaderListener.h
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#ifndef SHADERLISTENER_H_
#define SHADERLISTENER_H_

#include <CommonTypes.h>

namespace ShaderCompiler {

	/** Shader <name> {}*/
	struct ShaderListener :
			public ScriptParser::StatementListener,
			public ScriptParser::BlockListener {

		static CommandNamePair commands[];
		static const size_t commandCount;

		String name;
		ShaderAsset::StreamRequest* shader;
		ShaderListener(ShaderAsset::StreamRequest*, const String& name);

		virtual void EnterBlock(ScriptParser::BlockContext& block);
		virtual void EnterStatement(ScriptParser::StatementContext& statement);
	};
}


#endif /* SHADERLISTENER_H_ */
