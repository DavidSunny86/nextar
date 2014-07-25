/*
 * ShaderLoaderImplv10.h
 *
 *  Created on: 21-Sep-2013
 *      Author: obhi
 */

#ifndef SHADERLOADERIMPLV10_H_
#define SHADERLOADERIMPLV10_H_

#include <FactoryPlugin.h>
#include <ScriptParser.h>

using namespace nextar;
namespace ShaderCompiler {

class ShaderLoaderImplv1_0: public AllocGeneral, public AssetLoaderImpl {
public:

	ShaderLoaderImplv1_0();
	virtual ~ShaderLoaderImplv1_0();

	virtual void Configure(const Config&);
	virtual void Load(InputStreamPtr& input, AssetLoader& shader);

protected:

};

} /* namespace nextar */
#endif /* SHADERLOADERIMPLV10_H_ */
