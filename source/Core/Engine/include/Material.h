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
public:
	enum {
		RENDER_INFO_PER_PASS = 1 << 0,
	};
		
	Material() : _reserved_p(nullptr), flags(0) {}
	~Material();
		
	union {
		ShaderUnitRenderInfo* _reserved_p;
		ShaderUnitRenderInfo _reserved_i;
	};


	uint32 flags;
	//ConstantBufferPtr materialParameters;
	// todo
	ParameterBuffer materialParamData;
};

} /* namespace nextar */

#endif /* CORE_ENGINE_INCLUDE_MATERIAL_H_ */
