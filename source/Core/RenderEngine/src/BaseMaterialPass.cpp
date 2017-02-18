/*
 * BaseMaterialPass.cpp
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#include <BaseMaterialPass.h>

namespace nextar {

BaseMaterialPass::BaseMaterialPass(const Meta* meta) : 
	BaseRenderPass(meta) {

}

BaseMaterialPass::~BaseMaterialPass() {

}

void BaseMaterialPass::SetMaterial(MaterialAssetPtr& material) {
	this->material = material;
	material->RequestLoad();
	materialUrl = material->GetAssetLocator();
	material->GetID(materialId);
	OnMaterialLoad();
}

void BaseMaterialPass::CreateResources() {
	if (!material) {
		material = MaterialAsset::Traits::Instance(materialId, materialUrl);
		if (material) {
			material->RequestLoad();
			OnMaterialLoad();
		} else
			Error("Failed to load material: " + materialUrl.ToString());
	}
}

void BaseMaterialPass::DestroyResources() {
	material.Clear();
}

void BaseMaterialPass::Save(RenderSystem* rsysPtr, OutputSerializer& ser) {
	BaseRenderPass::Save(rsysPtr, ser);
	ser << materialId << materialUrl;
}

void BaseMaterialPass::Load(RenderSystem* rsysPtr, InputSerializer& ser) {
	BaseRenderPass::Load(rsysPtr, ser);
	ser >> materialId >> materialUrl;
}

} /* namespace nextar */
