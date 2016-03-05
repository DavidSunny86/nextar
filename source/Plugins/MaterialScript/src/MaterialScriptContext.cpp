/*
 * MaterialScriptContext.cpp
 *
 *  Created on: 03-Mar-2016
 *      Author: obhi
 */

#include <MaterialScriptContext.h>

namespace MaterialScript {

MaterialScriptContext::MaterialScriptContext(MaterialTemplate::StreamRequest* req,
		const MaterialAsset::ID& id) :material(req)
, materialId(id) {
}

MaterialScriptContext::~MaterialScriptContext() {
}

} /* namespace MaterialScript */
