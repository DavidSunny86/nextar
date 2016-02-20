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
	CompositorRenderPass();
	virtual ~CompositorRenderPass();

	ParameterBuffer& GetParameterBuffer() {
		return parameters;
	}

	const ParameterBuffer& GetParameterBuffer() const {
		return parameters;
	}

	virtual void Commit(CommitContext& context);

	virtual void CreateResources();
	virtual void DestroyResources();

protected:

	struct TexturesToResolve {
		RenderTargetName name;
		uint32 offset;
	};

	union {
		TexturesToResolve* _rtBunchOf;
		TexturesToResolve  _rtJustOne;
	};

	VisiblePrimitive primitive;
	uint32 numTextureToResolve;
	ParameterBuffer parameters;
};

} /* namespace nextar */

#endif /* CORE_RENDERENGINE_LOCAL_INCLUDE_COPYRENDERPASS_H_ */
