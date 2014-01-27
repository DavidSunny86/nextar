/*
 * ShaderLoaderImplv10.h
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */

#ifndef SHADERLOADERIMPLV10_H_
#define SHADERLOADERIMPLV10_H_

#include "FactoryPlugin.h"
#include "ScriptParser.h"

using namespace nextar;
namespace ShaderCompiler {

	class ShaderLoaderImplv1_0 : public AllocGeneral,
	public Shader::LoaderImpl {
	public:

		class Script : public ScriptParser::ScriptListener,
					public ScriptParser::StatementListener {
			Shader::StreamRequest* shader;
		public:
			Script(Shader::StreamRequest* s) : shader(s) {}
			virtual void EnterScript(ScriptParser::ScriptContext& block);
			virtual void EnterStatement(ScriptParser::StatementContext& ctx);
		};

		ShaderLoaderImplv1_0();
		virtual ~ShaderLoaderImplv1_0();

		virtual void Load(InputStreamPtr& input, Shader::Loader& shader);

	protected:

	};

} /* namespace nextar */
#endif /* SHADERLOADERIMPLV10_H_ */
