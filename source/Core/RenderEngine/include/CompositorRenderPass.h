/*
 * CopyRenderPass.h
 *
 *  Created on: 08-Feb-2016
 *      Author: obhi
 */

#ifndef CORE_RENDERENGINE_LOCAL_INCLUDE_COPYRENDERPASS_H_
#define CORE_RENDERENGINE_LOCAL_INCLUDE_COPYRENDERPASS_H_

#include <BaseMaterialPass.h>

namespace nextar {

class _NexRenderAPI CompositorRenderPass: public BaseMaterialPass {
public:
	struct TexturesToResolve {
		RenderTargetName name;
		uint32 offset;
	};

	static RenderPass* CreateInstance();

	CompositorRenderPass();
	virtual ~CompositorRenderPass();

	virtual void OnMaterialLoad();

	ParameterBuffer& GetParameterBuffer() {
		return parameters;
	}

	const ParameterBuffer& GetParameterBuffer() const {
		return parameters;
	}

	virtual void Commit(CommitContext& context);

	virtual void CreateResources();
	virtual void DestroyResources();

	void AddTexturesToResolve(const TexturesToResolve* toResolve, 
		uint32 numToResolve);

	virtual void Save(RenderSystem* rsysPtr, OutputSerializer& ser);
	virtual void Load(RenderSystem* rsysPtr, InputSerializer& ser);

protected:

	union {
		TexturesToResolve* _rtBunchOf;
		TexturesToResolve  _rtJustOne;
	};

	uint32 renderSystemTicket;
	uint32 numTextureToResolve;
	VisiblePrimitive primitive;
	ParameterBuffer parameters;
};

} /* namespace nextar */

#endif /* CORE_RENDERENGINE_LOCAL_INCLUDE_COPYRENDERPASS_H_ */
