/*
 * BaseRenderPass.cpp
 *
 *  Created on: 06-Feb-2016
 *      Author: obhi
 */

#include <BaseRenderPass.h>
#include <Geometry.h>

namespace nextar {

BaseRenderPass::BaseRenderPass() :
clearFlags(ClearFlags::CLEAR_NONE)
,enabled(true)
,toLastSubTarget(RT_NONE) {
	info.info.clearDepth = 1.0f;

}

BaseRenderPass::~BaseRenderPass() {
}

void BaseRenderPass::BeginRender(CommitContext& ctx) {
	if (info.rt) {
		ctx.renderContext->BeginRender(&info, clearFlags);
		ctx.lastRenderTarget = info.rt;
	} else if (toLastSubTarget != RenderTargetName::RT_NONE) {
		info.rt = ctx.GetTargetByName(toLastSubTarget);
		ctx.renderContext->BeginRender(&info, clearFlags);
		if (info.rt) {
			ctx.lastRenderTarget = info.rt;
			info.rt = nullptr;
		}
	}
}

void BaseRenderPass::RenderPrimitive(
		CommitContext& context,
		uint32 key,
		VisiblePrimitive* vis) {
	MaterialAsset* material = vis->GetMaterial();
	ShaderAsset* shader = material->GetShader();
	if (context.shader != shader) {
		context.shader = shader;
		// deferred pass at 0
		Pass& pass = context.shader->GetPass(0);
		context.passNumber = pass.GetID();
		context.pass = static_cast<Pass::View*>(context.renderContext->GetView(&pass));
		context.paramBuffers[(uint32)ParameterContext::CTX_PASS] = &shader->GetParameters();
		context.pass->SwitchAndUpdateParams(context);
	}
	context.primitive = vis;
	if (context.material != material) {
		context.material = material;
		context.materialNumber = (uint32)(ptrdiff_t)material;
		context.paramBuffers[(uint32)ParameterContext::CTX_MATERIAL] = context.material->GetParameters();
		context.pass->UpdateParams(context, ParameterContext::CTX_MATERIAL, context.materialNumber);
	}

	context.paramBuffers[(uint32)ParameterContext::CTX_OBJECT] = vis->GetParameters();
	context.pass->UpdateParams(context, ParameterContext::CTX_OBJECT, key + context.frameNumber);
	context.renderContext->Draw(vis->GetStreamData(), context);
}

void BaseRenderPass::CreateResources() {
}

void BaseRenderPass::DestroyResources() {
}

} /* namespace nextar */
