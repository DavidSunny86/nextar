/*
 * ShaderLoaderImplv10.h
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */

#ifndef SHADERCOMPILERIMPLV10_H_
#define SHADERCOMPILERIMPLV10_H_

#include <FactoryPlugin.h>
#include <ScriptParser.h>

using namespace nextar;
namespace ShaderCompiler {

class ShaderCompilerImplv1_0: public AllocGeneral, public AssetLoaderImpl {
public:

	ShaderCompilerImplv1_0();
	virtual ~ShaderCompilerImplv1_0();

	virtual void Configure(const Config&);
	virtual void Load(InputStreamPtr& input, AssetLoader& shader);

protected:

};

} /* namespace nextar */
#endif /* SHADERCOMPILERIMPLV10_H_ */
