/*
 * MultiRenderTargetViewGL.cpp
 *
 *  Created on: 13-Jun-2014
 *      Author: obhi
 */

#include <MultiRenderTargetViewGL.h>
#include <RenderBuffer.h>
#include <RenderTarget.h>
#include <RenderTexture.h>
#include <RenderBufferViewGL.h>
#include <RenderTextureViewGL.h>
#include <RenderContext_Base_GL.h>

namespace RenderOpenGL {

MultiRenderTargetViewGL::MultiRenderTargetViewGL() :
		MultiRenderTarget::View(), colorAttachmentCount(0) {
}

MultiRenderTargetViewGL::~MultiRenderTargetViewGL() {
}

void MultiRenderTargetViewGL::Destroy(nextar::RenderContext* rc) {
	auto gl = static_cast<RenderContext_Base_GL*>(rc);
	gl->DestroyFBO(fbo);
}

void MultiRenderTargetViewGL::Update(nextar::RenderContext* rc, uint32 msg,
		ContextObject::ContextParamPtr params) {

	auto gl = static_cast<RenderContext_Base_GL*>(rc);
	switch (msg) {

	case MultiRenderTarget::MSG_RT_RESIZE:
	case MultiRenderTarget::MSG_RT_CREATE: {
		const MultiRenderTarget* rt =
				reinterpret_cast<const MultiRenderTarget*>(params);

		uint32 numColorTargets = rt->GetColorTargetsCount();
		colorAttachmentCount = numColorTargets;
		if(fbo.IsValid())
			gl->DestroyFBO(fbo);

		gl->CreateFBO(fbo);
		gl->BindFBO(fbo);
		for (uint32 i = 0; i < numColorTargets; ++i) {
			RenderTarget* colorTarget = rt->GetAttachment(i).GetPtr();
			switch (colorTarget->GetRenderTargetType()) {
			case RenderTargetType::RENDER_BUFFER:
				gl->AttachToFBO(static_cast<RenderBufferViewGL*>(gl->GetView(
					static_cast<RenderBuffer*>(colorTarget))), GL_COLOR_ATTACHMENT0 + i);
				break;
			case RenderTargetType::TEXTURE:
				gl->AttachToFBO(static_cast<RenderTextureViewGL*>(gl->GetView(
					static_cast<RenderTexture*>(colorTarget))), GL_COLOR_ATTACHMENT0 + i);
				break;
			}
		}

		RenderTarget* depthTarget = rt->GetDepthAttachment().GetPtr();
		if (depthTarget) {
			switch (depthTarget->GetRenderTargetType()) {
			case RenderTargetType::RENDER_BUFFER: {
				RenderBufferViewGL* textureView =
					static_cast<RenderBufferViewGL*>(gl->GetView(
					static_cast<RenderBuffer*>(depthTarget)));
				gl->AttachToFBO(textureView, textureView->GetAttachment());
				break;
			}
			case RenderTargetType::TEXTURE: {
				RenderTextureViewGL* textureView =
					static_cast<RenderTextureViewGL*>(gl->GetView(
					static_cast<RenderTexture*>(depthTarget)));
				gl->AttachToFBO(textureView, textureView->GetAttachment());
			}
				break;
			}
		}
		gl->ValidateFBO();
		gl->UnbindFBO();
		break;
	}
	default:
		NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	}
}

} /* namespace RenderOpenGL */
