/*
 * RenderSystemImpl.h
 *
 *  Created on: 06-Feb-2016
 *      Author: obhi
 */

#ifndef CORE_RENDERENGINE_LOCAL_INCLUDE_RENDERSYSTEMIMPL_H_
#define CORE_RENDERENGINE_LOCAL_INCLUDE_RENDERSYSTEMIMPL_H_

#include <NexRenderEngine.h>
#include <RenderSystem.h>
#include <MultiRenderTarget.h>
#include <RenderBuffer.h>

namespace nextar {

class RenderSystemImpl: public RenderSystem {
public:
	RenderSystemImpl();
	virtual ~RenderSystemImpl();

	virtual void RegisterTarget(StringID as, RenderTargetPtr target) override;
	virtual RenderTargetPtr GetTarget(StringID name) override;

	virtual void Load(InputStreamPtr& stream, const String& fileType);
	virtual void Save(OutputStreamPtr& stream, const String& fileType);

	virtual void BeginConfig(bool bStoreMetaInfo = false);
	virtual RenderTargetPtr CreateMultiRenderTarget(StringID name,
		const MultiRenderTarget::CreateParams& params,
		float viewRelativeWidthFactor,
		float viewRelativeHeightFactor);
	virtual RenderTargetPtr CreateRenderTexture(StringID name,
		const RenderTexture::CreateParams& params,
		float viewRelativeWidthFactor,
		float viewRelativeHeightFactor);
	virtual RenderTargetPtr CreateRenderBuffer(StringID name,
		const RenderBuffer::CreateParams& params,
		float viewRelativeWidthFactor,
		float viewRelativeHeightFactor);
	virtual void AddPass(RenderPass* pass);
	virtual void EndConfig();

	virtual uint32 GetRenderTargetCount() const;

	virtual void Commit(CommitContext& context);
		
	virtual RenderPass* FindPass(StringID name);
	virtual void EnablePass(StringID pass);
	virtual void DisablePass(StringID pass);

	void CreateResources();
	void DestroyResources();


protected:
		
	enum Flags {
		STORE_META_INFO = 1 << 0,
	};

	typedef map<StringID, RenderTargetPtr>::type TargetMap;
	
	uint32 flags;
	TargetMap targets;
	RenderPassList renderPasses;
};

typedef RefPtr<RenderSystemImpl> RenderSystemImplPtr;
} /* namespace nextar */

#endif /* CORE_RENDERENGINE_LOCAL_INCLUDE_RENDERSYSTEMIMPL_H_ */
