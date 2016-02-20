/*
 * BaseRenderPass.h
 *
 *  Created on: 06-Feb-2016
 *      Author: obhi
 */

#ifndef CORE_RENDERENGINE_LOCAL_INCLUDE_BASERENDERPASS_H_
#define CORE_RENDERENGINE_LOCAL_INCLUDE_BASERENDERPASS_H_

#include <RenderPass.h>
#include <CommitContext.h>
#include <NexRenderEngine.h>

namespace nextar {

class _NexRenderAPI BaseRenderPass: public RenderPass {
public:
	BaseRenderPass();
	virtual ~BaseRenderPass();

	void SetTarget(RenderTarget* target);
	void SetTarget(RenderTargetName rt);
	void SetCleanDepth(bool yesNo, float value = -1.0f);
	void SetCleanColor(uint32 i, const Color& c);
	void BeginRender(CommitContext& ctx);
	void EndRender(CommitContext& ctx);

	virtual void CreateResources();
	virtual void DestroyResources();

	void RenderPrimitive(CommitContext& ctx, uint32 key, VisiblePrimitive* vis);

protected:


	RenderTargetName toLastSubTarget;
	ClearFlags clearFlags;
	RenderInfo info;
};

inline void BaseRenderPass::SetTarget(RenderTargetName rt) {
	info.rt = nullptr;
	toLastSubTarget = rt;
}

inline void BaseRenderPass::SetTarget(RenderTarget* target) {
	info.rt = target;
	toLastSubTarget = RenderTargetName::RT_NONE;
}

inline void BaseRenderPass::SetCleanDepth(bool clean, float v) {
	if (clean) {
		clearFlags |= ClearFlags::CLEAR_DEPTH;
		info.info.clearDepth = v;
	} else {
		clearFlags &= ~ClearFlags::CLEAR_DEPTH;
	}
}

inline void BaseRenderPass::SetCleanColor(uint32 i, const Color& c) {
	clearFlags |= (1 << (ClearFlags::CLEAR_COLOR_I + i));
	info.info.clearColor[i] = c;
}

inline void BaseRenderPass::EndRender(CommitContext& ctx) {
	ctx.renderContext->EndRender();
}

} /* namespace nextar */

#endif /* CORE_RENDERENGINE_LOCAL_INCLUDE_BASERENDERPASS_H_ */
