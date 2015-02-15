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

namespace RenderOpenGL {

MultiRenderTargetViewGL::MultiRenderTargetViewGL() :
		MultiRenderTarget::View() {
}

MultiRenderTargetViewGL::~MultiRenderTargetViewGL() {
}

void MultiRenderTargetViewGL::Destroy(nextar::RenderContext* rc) {
	auto gl = static_cast<RenderContextGL*>(rc);
	fbo.Destroy(gl);
}

void MultiRenderTargetViewGL::Update(nextar::RenderContext* rc, uint32 msg,
		ContextObject::ContextParamPtr params) {

	auto gl = static_cast<RenderContextGL*>(rc);
	switch (msg) {
	case MultiRenderTarget::MSG_RT_CREATE: {
		const MultiRenderTarget* rt =
				reinterpret_cast<const MultiRenderTarget*>(params);

		uint32 numColorTargets = rt->GetColorTargetsCount();
		fbo.CreateAndBind(gl);
		for (uint32 i = 0; i < numColorTargets; ++i) {
			RenderTarget* colorTarget = rt->GetAttachment(i).GetPtr();
			switch (colorTarget->GetRenderTargetType()) {
			case RenderTargetType::RENDER_BUFFER:
				fbo.Attach(gl, GL_COLOR_ATTACHMENT0 + i,
						static_cast<RenderBufferViewGL*>(gl->GetView(
								static_cast<RenderBuffer*>(colorTarget))));
				break;
			case RenderTargetType::TEXTURE:
				fbo.Attach(gl, i,
						static_cast<RenderTextureViewGL*>(gl->GetView(
								static_cast<RenderTexture*>(colorTarget))));
				break;
			}
		}

		RenderTarget* depthTarget = rt->GetDepthAttachment().GetPtr();
		switch (depthTarget->GetRenderTargetType()) {
		case RenderTargetType::RENDER_BUFFER: {
			RenderBufferViewGL* textureView =
					static_cast<RenderBufferViewGL*>(gl->GetView(
							static_cast<RenderBuffer*>(depthTarget)));
			fbo.Attach(gl, textureView->GetAttachment(), textureView);
			break;
		}
		case RenderTargetType::TEXTURE: {
			RenderTextureViewGL* textureView =
					static_cast<RenderTextureViewGL*>(gl->GetView(
							static_cast<RenderTexture*>(depthTarget)));
			fbo.Attach(gl, textureView->GetAttachment(), textureView);
		}
			break;
		}
		fbo.Validate(gl);
		fbo.Unbind(false, gl);
		break;
	}
	default:
		NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
	}
}

} /* namespace RenderOpenGL */
