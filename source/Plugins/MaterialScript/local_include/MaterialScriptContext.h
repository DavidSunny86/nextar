/*
 * MaterialScriptContext.h
 *
 *  Created on: 03-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_MATERIALSCRIPTCONTEXT_H_
#define PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_MATERIALSCRIPTCONTEXT_H_

#include <MaterialScript.h>
#include <CommandContext.h>

namespace MaterialScript {

class MaterialScriptContext: public nextar::CommandContext {
public:
	MaterialScriptContext(MaterialTemplate::StreamRequest* req, const MaterialAsset::ID& id);
	virtual ~MaterialScriptContext();

	ASTParameter::Resolver templateResolver;
	MaterialTemplate::StreamRequest* material;
	MaterialAsset::ID materialId;
};

} /* namespace MaterialScript */

#endif /* PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_MATERIALSCRIPTCONTEXT_H_ */
