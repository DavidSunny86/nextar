/*
 * MultiRenderTargetViewGL.cpp
 *
 *  Created on: 13-Jun-2014
 *      Author: obhi
 */

#include <MultiRenderTargetViewGL.h>

namespace RenderOpenGL {

	MultiRenderTargetViewGL::MultiRenderTargetViewGL() :
		MultiRenderTarget::View() {
	}

	MultiRenderTargetViewGL::~MultiRenderTargetViewGL() {
	}

	void MultiRenderTargetViewGL::Update(nextar::RenderContext* rc,
			uint32 msg,
			ContextObject::ContextParamPtr params) {

		auto gl = static_cast<RenderContextGL*>(rc);
		if (msg == MultiRenderTarget::MSG_RT_CREATE) {
			const MultiRenderTarget* rt =
					reinterpret_cast<const MultiRenderTarget*>(params);

			uint32 numColorTargets = rt->GetColorTargetsCount();
			fbo.CreateAndBind(gl);
			for(uint32 i = 0; i < numColorTargets; ++i) {
				RenderTargetPtr colorTarget = rt->GetAttachment(i);
				switch(colorTarget->GetRenderTargetType()) {
				case RenderTargetType::RENDER_BUFFER:
					break;
				case RenderTargetType::RENDER_TEXTURE:
					fbo.AttachColor(gl, i,
							static_cast<RenderTextureViewGL*>(
									gl->GetView(static_cast<RenderTexture*>(rt))));
					break;
				}

				RenderTargetPtr depthTarget = rt->GetDepthAttachment();
				switch(depthTarget->GetRenderTargetType()) {
				case RenderTargetType::RENDER_BUFFER:
					break;
				case RenderTargetType::RENDER_TEXTURE: {
					RenderTextureViewGL* textureView = static_cast<RenderTextureViewGL*>(
						gl->GetView(static_cast<RenderTexture*>(rt)));
					fbo.AttachDepth(gl, textureView->GetAttachment(), textureView);
				}
					break;
				}
			}
			fbo.Validate(gl);
			fbo.Unbind(false, gl);
		} else {
			NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
		}
	}

} /* namespace RenderOpenGL */
