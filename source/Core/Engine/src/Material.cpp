/*
 * Material.cpp
 *
 *  Created on: 05-Mar-2017
 *      Author: obhi
 */

#include <Material.h>
#include <RenderManager.h>
namespace nextar {

Material::~Material() {
	if (_reserved_p) {
		RenderManager::Instance().FreeMaterialRenderInfo(_reserved_p);
		_reserved_p = nullptr;
	}
}

} /* namespace nextar */
