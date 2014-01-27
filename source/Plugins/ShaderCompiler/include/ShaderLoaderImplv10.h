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
	public ShaderAsset::LoaderImpl {
	public:

		class Script : public ScriptParser::ScriptListener,
					public ScriptParser::StatementListener {
			ShaderAsset::StreamRequest* shader;
		public:
			Script(ShaderAsset::StreamRequest* s) : shader(s) {}
			virtual void EnterScript(ScriptParser::ScriptContext& block);
			virtual void EnterStatement(ScriptParser::StatementContext& ctx);
		};

		ShaderLoaderImplv1_0();
		virtual ~ShaderLoaderImplv1_0();

		virtual void Load(InputStreamPtr& input, ShaderAsset::Loader& shader);

	protected:

	};

} /* namespace nextar */
#endif /* SHADERLOADERIMPLV10_H_ */
