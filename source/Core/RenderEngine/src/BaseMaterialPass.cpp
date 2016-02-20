/*
 * BaseMaterialPass.cpp
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#include <BaseMaterialPass.h>

namespace nextar {

BaseMaterialPass::BaseMaterialPass() {

}

BaseMaterialPass::~BaseMaterialPass() {

}

void BaseMaterialPass::CreateResources() {
	if (!material) {
		material = Asset::AssetLoad(materialUrl);
	} else {
		material->RequestLoad();
	}
}

void BaseMaterialPass::DestroyResources() {
	material.Clear();
}

} /* namespace nextar */
