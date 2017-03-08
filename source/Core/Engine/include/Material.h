/*
 * Material.h
 *
 *  Created on: 05-Mar-2017
 *      Author: obhi
 */

#ifndef CORE_ENGINE_INCLUDE_MATERIAL_H_
#define CORE_ENGINE_INCLUDE_MATERIAL_H_

#include <ShaderParam.h>
#include <ParameterBuffer.h>


namespace nextar {

class _NexEngineAPI Material : public AllocGeneral {

	struct RenderInfo {
	};

	Material() : _reserved(nullptr) {}
	~Material();

	RenderInfo* _reserved;
	//ConstantBufferPtr materialParameters;
	// todo
	ParameterBuffer materialParamData;

};

} /* namespace nextar */

#endif /* CORE_ENGINE_INCLUDE_MATERIAL_H_ */
