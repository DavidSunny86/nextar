/*
 * BaseMaterialPass.h
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#ifndef CORE_RENDERENGINE_INCLUDE_BASEMATERIALPASS_H_
#define CORE_RENDERENGINE_INCLUDE_BASEMATERIALPASS_H_

#include <BaseRenderPass.h>

namespace nextar {

class _NexRenderAPI BaseMaterialPass : public BaseRenderPass {
	NEX_LOG_HELPER(BaseMaterialPass);
public:
	BaseMaterialPass();
	virtual ~BaseMaterialPass();

	void SetMaterialLocation(const URL& materialUrl) {
		this->materialUrl = materialUrl;
	}

	void SetMaterial(MaterialAssetPtr& material);
	virtual void OnMaterialLoad() {}

	MaterialAssetPtr& GetMaterial() {
		return material;
	}

	virtual void CreateResources();
	virtual void DestroyResources();
		
	virtual void Save(RenderSystem* rsysPtr, OutputSerializer& ser);
	virtual void Load(RenderSystem* rsysPtr, InputSerializer& ser);

protected:

	URL materialUrl;
	MaterialAsset::ID materialId;
	MaterialAssetPtr material;
};

} /* namespace nextar */

#endif /* CORE_RENDERENGINE_INCLUDE_BASEMATERIALPASS_H_ */
