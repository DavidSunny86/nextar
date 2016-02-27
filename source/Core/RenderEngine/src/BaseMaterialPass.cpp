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

void BaseMaterialPass::SetMaterial(MaterialAssetPtr& material) {
	this->material = material;
	material->RequestLoad();
	OnMaterialLoad();
}

void BaseMaterialPass::CreateResources() {
	if (!material) {
		material = Asset::AssetLoad(materialUrl);
		OnMaterialLoad();
	}
}

void BaseMaterialPass::DestroyResources() {
	material.Clear();
}

} /* namespace nextar */
