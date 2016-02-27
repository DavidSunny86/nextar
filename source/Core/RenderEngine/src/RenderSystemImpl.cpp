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

RenderSystemImpl::RenderSystemImpl(Size size) :
RenderSystem(),
currentSize(size),
flags(0) {
}

RenderSystemImpl::~RenderSystemImpl() {
	for (auto& p : renderPasses) {
		NEX_DELETE(p);
	}
	ApplicationContext::Instance().Unsubscribe(ApplicationContext::EVENT_DESTROY_RESOURCES, DestroyResources);
}

void RenderSystemImpl::RegisterTarget(StringID as,
		RenderTargetPtr target) {
	targets[as].target = target;
}

RenderTargetPtr RenderSystemImpl::GetTarget(StringID name) {
	auto it = targets.find(name);
	if (it != targets.end())
		return (*it).second.target;
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
		const MultiRenderTarget::CreateParams& iparams,
		float dx, float dy) {

	renderSystemTicket++;

	MultiRenderTarget::CreateParams params = iparams;
	MultiRenderTargetPtr renderTarget = Assign(NEX_NEW(MultiRenderTarget()));
	if (params.dimensions.combined == 0) {
		// factored
		params.dimensions.dx = (uint16)(currentSize.dx * dx);
		params.dimensions.dy = (uint16)(currentSize.dy * dy);
		flags |= HAS_RELATIVE_TARGETS;
	}

	renderTarget->Create(params);
	auto& e = targets[name];
	e.target = renderTarget;
	e.viewRelativeX = dx;
	e.viewRelativeY = dy;

	return e.target;
}

RenderTargetPtr RenderSystemImpl::CreateRenderTexture(StringID name,
		const RenderTexture::CreateParams& iparams,
		float dx, float dy) {

	renderSystemTicket++;

	RenderTexture::CreateParams params = iparams;
	RenderTexturePtr renderTarget = Assign(NEX_NEW(RenderTexture()));
	if (params.width == 0 && params.height == 0) {
		// factored
		// assert depth is 1??
		params.width = (uint16)(currentSize.dx * dx);
		params.height = (uint16)(currentSize.dy * dy);
		flags |= HAS_RELATIVE_TARGETS;
	}

	renderTarget->Create(params);
	auto& e = targets[name];
	e.target = renderTarget;
	e.viewRelativeX = dx;
	e.viewRelativeY = dy;
	return e.target;
}

RenderTargetPtr RenderSystemImpl::CreateRenderBuffer(StringID name,
		const RenderBuffer::CreateParams& iparams,
		float dx,
		float dy) {

	renderSystemTicket++;

	RenderBuffer::CreateParams params = iparams;
	RenderBufferPtr renderTarget = Assign(NEX_NEW(RenderBuffer()));
	if (params.dimensions.combined == 0) {
		// factored
		params.dimensions.dx = (uint16)(currentSize.dx * dx);
		params.dimensions.dy = (uint16)(currentSize.dy * dy);
		flags |= HAS_RELATIVE_TARGETS;
	}

	renderTarget->Create(params);
	auto& e = targets[name];
	e.target = renderTarget;
	e.viewRelativeX = dx;
	e.viewRelativeY = dy;
	return e.target;
}

RenderPass* RenderSystemImpl::AddPass(const String& type) {

	renderSystemTicket++;

	RenderPass* r = RenderManager::Instance().CreateRenderPass(type);
	renderPasses.push_back(r);
	return r;
}

void RenderSystemImpl::EndConfig() {

	renderSystemTicket++;
}

uint32 RenderSystemImpl::GetRenderTargetCount() const {
	return (uint32)targets.size();
}

void RenderSystemImpl::Commit(CommitContext& context) {
	if (context.viewDimensions.combined != currentSize.combined) {
		if (flags & HAS_RELATIVE_TARGETS) {
			for(auto& e : targets) {
				if (e.second.viewRelativeX > 0 &&
						e.second.viewRelativeY > 0	) {
					Size newDim = Size( (uint16)(context.viewDimensions.dx * e.second.viewRelativeX),
							(uint16)(context.viewDimensions.dy * e.second.viewRelativeY));
					e.second.target->Resize(newDim);
				}
			}
		}
		currentSize = context.viewDimensions;
	}

	context.rsys = this;
	for(auto& p : renderPasses) {
		p->Commit(context);
	}
}

RenderPass* RenderSystemImpl::FindPass(StringID name) {
	for(auto& p : renderPasses) {
		if (p->GetID() == name)
			return p;
	}
	return nullptr;
}

void RenderSystemImpl::EnablePass(StringID pass) {

	renderSystemTicket++;

	RenderPass* p = FindPass(pass);
	if (p)
		static_cast<BaseRenderPass*>(p)->SetEnabled(true);
}

void RenderSystemImpl::DisablePass(StringID pass) {

	renderSystemTicket++;

	RenderPass* p = FindPass(pass);
	if (p)
		static_cast<BaseRenderPass*>(p)->SetEnabled(false);
}

void RenderSystemImpl::CreateResources() {
	renderSystemTicket++;

	for (auto& p : renderPasses) {
		static_cast<BaseRenderPass*>(p)->CreateResources();
	}
	ApplicationContext::Instance().Subscribe(ApplicationContext::EVENT_DESTROY_RESOURCES, DestroyResources, this);
}

void RenderSystemImpl::DestroyResources() {

	renderSystemTicket++;

	for (auto& p : renderPasses) {
		if (static_cast<BaseRenderPass*>(p)->IsEnabled())
			static_cast<BaseRenderPass*>(p)->DestroyResources();
	}
}

void RenderSystemImpl::DestroyResources(void* thisPointer) {
	reinterpret_cast<RenderSystemImpl*>(thisPointer)->DestroyResources();
}

} /* namespace nextar */
