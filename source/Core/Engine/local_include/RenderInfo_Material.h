/*
 * RenderInfo_Material.h
 *
 *  Created on: 05-Mar-2017
 *      Author: obhi
 */

#ifndef CORE_ENGINE_LOCAL_INCLUDE_RENDERINFO_MATERIAL_H_
#define CORE_ENGINE_LOCAL_INCLUDE_RENDERINFO_MATERIAL_H_

#include <Material.h>

namespace nextar {

struct RenderInfo_Material : public Material::RenderInfo {
	int16 shaderUnit;
};

}


#endif /* CORE_ENGINE_LOCAL_INCLUDE_RENDERINFO_MATERIAL_H_ */
