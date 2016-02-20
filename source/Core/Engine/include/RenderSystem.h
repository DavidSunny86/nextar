/*
 * RenderPass.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef RENDERSYSTEM_H_
#define RENDERSYSTEM_H_

#include <RenderContext.h>
#include <RenderPass.h>
#include <NamedObject.h>
#include <MultiRenderTarget.h>
#include <RenderTexture.h>
#include <RenderBuffer.h>

namespace nextar {

class _NexEngineAPI RenderSystem:
	public Referenced<RenderSystem, AllocGraphics>,
	public NamedObject {
public:
	class Streamer {
	public:
		virtual bool Load(RenderSystem& s, InputStreamPtr& stream) = 0;
		virtual bool Save(RenderSystem& s, OutputStreamPtr& stream) = 0;
	protected:
		~Streamer() {}
	};

	RenderSystem(Size viewerInitialDim) : viewerDim(viewerInitialDim) {}
	virtual ~RenderSystem();

	virtual void BeginConfig(bool bStoreMetaInfo = false) = 0;
	virtual RenderTargetPtr CreateMultiRenderTarget(StringID name,
			const MultiRenderTarget::CreateParams& params,
			float viewRelativeWidthFactor,
			float viewRelativeHeightFactor) = 0;
	virtual RenderTargetPtr CreateRenderTexture(StringID name,
			const RenderTexture::CreateParams& params,
			float viewRelativeWidthFactor,
			float viewRelativeHeightFactor) = 0;
	virtual RenderTargetPtr CreateRenderBuffer(StringID name,
			const RenderBuffer::CreateParams& params,
			float viewRelativeWidthFactor,
			float viewRelativeHeightFactor) = 0;
	virtual void AddPass(RenderPass* pass) = 0;
	virtual void EndConfig() = 0;

	virtual uint32 GetRenderTargetCount() const = 0;

	virtual void Commit(CommitContext& context) = 0;
	virtual void RegisterTarget(StringID as, RenderTargetPtr target) = 0;
	virtual RenderTargetPtr GetTarget(StringID name) = 0;

	/* Streaming in-out */
	virtual void Load(InputStreamPtr& stream, const String& fileType) = 0;
	virtual void Save(OutputStreamPtr& stream, const String& fileType) = 0;

protected:

	Size viewerDim;
};

typedef vector<RenderPass*>::type RenderPassList;

} /* namespace nextar */
#endif /* RENDERSYSTEM_H_ */
