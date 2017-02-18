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
#include <Serializer.h>

namespace nextar {

class _NexRenderAPI BaseRenderPass: public RenderPass {
public:

	struct Meta {
		const char* _type;
		RenderPass::CreateInstance _instantiator;

		Meta(const char* type, RenderPass::CreateInstance instance) : 
			_type(type),
			_instantiator(instance) {
		}
	};

	template <typename PassClass>
	struct MetaType : public Meta {
		typedef MetaType<PassClass> Type;
		static Type _instance;

		static nextar::RenderPass* CreateInstanceFunc() {
			return NEX_NEW(PassClass(&_instance));
		}

		MetaType(const char* pName) : Meta(pName, &Type::CreateInstanceFunc) {}
	};

	BaseRenderPass(const Meta* meta);
	virtual ~BaseRenderPass();

	void SetTarget(RenderTarget* target);
	void SetTarget(RenderTargetName rt);
	void SetCleanDepth(bool yesNo, float value = -1.0f);
	void SetCleanColor(uint32 i, const Color& c);
	void SetCleanStencil(bool yesNo, int16 value);
	void BeginRender(CommitContext& ctx);
	void EndRender(CommitContext& ctx);

	virtual void CreateResources();
	virtual void DestroyResources();

	void RenderPrimitive(CommitContext& ctx, uint32 key, VisiblePrimitive* vis);

	virtual void Save(RenderSystem* rsysPtr, OutputSerializer& ser);
	virtual void Load(RenderSystem* rsysPtr, InputSerializer& ser);

	inline const char* GetPassType() const {
		return _meta->_type;
	}

protected:
		
	const Meta* _meta;
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
		clearFlags &=~ClearFlags::CLEAR_DEPTH;
	}
}

inline void BaseRenderPass::SetCleanColor(uint32 i, const Color& c) {
	clearFlags |= (ClearFlags)(1 << ((uint16)ClearFlags::CLEAR_COLOR_I + i));
	info.info.clearColor[i] = c;
}

inline void BaseRenderPass::SetCleanStencil(bool clean, int16 value) {
	if (clean) {
		clearFlags |= ClearFlags::CLEAR_STENCIL;
		info.info.clearStencil = value;
	} else {
		clearFlags &=~ClearFlags::CLEAR_STENCIL;
	}
}

inline void BaseRenderPass::EndRender(CommitContext& ctx) {
	ctx.renderContext->EndRender();
	ctx.lastRenderTarget = info.rt;
	if (toLastSubTarget != RenderTargetName::RT_NONE)
		info.rt = nullptr;
}

} /* namespace nextar */

#endif /* CORE_RENDERENGINE_LOCAL_INCLUDE_BASERENDERPASS_H_ */
