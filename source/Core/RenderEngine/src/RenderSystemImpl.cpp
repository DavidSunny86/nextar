/*
 * RenderSystemImpl.cpp
 *
 *  Created on: 06-Feb-2016
 *      Author: obhi
 */

#include <RenderSystemImpl.h>
#include <RenderManager.h>
#include <ApplicationContext.h>

namespace nextar {

RenderSystemImpl::RenderSystemImpl() :
RenderSystem(),
flags(0) {
}

RenderSystemImpl::~RenderSystemImpl() {
}

void RenderSystemImpl::RegisterTarget(StringID as,
		RenderTargetPtr target) {
	targets[as] = target;
}

RenderTargetPtr RenderSystemImpl::GetTarget(StringID name) {
	auto it = targets.find(name);
	if (it != targets.end())
		return (*it).second;
	return RenderTargetPtr();
}

void RenderSystemImpl::Load(InputStreamPtr& stream, const String& fileType) {
	RenderSystem::Streamer* streamer = RenderManager::Instance().GetRenderStreamer(fileType);
	streamer->Load(*this, stream);
}

void RenderSystemImpl::Save(OutputStreamPtr& stream, const String& fileType) {
	RenderSystem::Streamer* streamer = RenderManager::Instance().GetRenderStreamer(fileType);
	streamer->Save(*this, stream);
}

void RenderSystemImpl::BeginConfig(bool bStoreMetaInfo) {
	renderSystemTicket = (uint32)(ptrdiff_t)this;
}

RenderTargetPtr RenderSystemImpl::CreateMultiRenderTarget(StringID name,
		const MultiRenderTarget::CreateParams& params,
		float viewRelativeWidthFactor, float viewRelativeHeightFactor) {

}

RenderTargetPtr RenderSystemImpl::CreateRenderTexture(StringID name,
		const RenderTexture::CreateParams& params,
		float viewRelativeWidthFactor, float viewRelativeHeightFactor) {
}

RenderTargetPtr RenderSystemImpl::CreateRenderBuffer(StringID name,
		const RenderBuffer::CreateParams& params, float viewRelativeWidthFactor,
		float viewRelativeHeightFactor) {
}

void RenderSystemImpl::AddPass(RenderPass* pass) {
}

void RenderSystemImpl::EndConfig() {
}

uint32 RenderSystemImpl::GetRenderTargetCount() const {
}

void RenderSystemImpl::Commit(CommitContext& context) {
}

RenderPass* RenderSystemImpl::FindPass(StringID name) {
}

void RenderSystemImpl::EnablePass(StringID pass) {
}

void RenderSystemImpl::DisablePass(StringID pass) {
}

void RenderSystemImpl::CreateResources() {
	for (auto& p : renderPasses) {
		static_cast<BaseRenderPass*>(p)->CreateResources();
	}
}

void RenderSystemImpl::DestroyResources() {
	for (auto& p : renderPasses) {
		static_cast<BaseRenderPass*>(p)->DestroyResources();
	}
}

} /* namespace nextar */
