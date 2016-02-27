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

protected:

	URL materialUrl;
	MaterialAssetPtr material;
};

} /* namespace nextar */

#endif /* CORE_RENDERENGINE_INCLUDE_BASEMATERIALPASS_H_ */
