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
	if (_reserved)
		RenderManager::Instance().FreeMaterialRenderInfo(_reserved);
	_reserved = nullptr;
}

} /* namespace nextar */
