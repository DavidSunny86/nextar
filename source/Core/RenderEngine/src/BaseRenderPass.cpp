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
,toLastSubTarget(RT_NONE) {
	info.info.clearDepth = 1.0f;

}

BaseRenderPass::~BaseRenderPass() {
}

void BaseRenderPass::BeginRender(CommitContext& ctx) {
	if (info.rt) {
		ctx.renderContext->BeginRender(&info, clearFlags);
	} else if (toLastSubTarget != RenderTargetName::RT_NONE) {
		info.rt = ctx.GetTargetByName(toLastSubTarget);
		ctx.renderContext->BeginRender(&info, clearFlags);
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
		context.passNumber = pass.GetPassNumber();
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
	context.pass->UpdateParams(context, ParameterContext::CTX_OBJECT, (((uint16)key) << 16) | ((uint16)context.frameNumber));
	context.renderContext->Draw(vis->GetStreamData(), context);
}

void BaseRenderPass::CreateResources() {
}

void BaseRenderPass::DestroyResources() {
}

void BaseRenderPass::Save(RenderSystem *renderSys, OutputSerializer& ser) {
	uint32 vLastSubTarget = static_cast<uint32>(toLastSubTarget);
	uint32 vClearFlags = static_cast<uint32>(clearFlags);
	ser << flags << vLastSubTarget << vClearFlags;
	for (uint32 i = 0; i < RenderConstants::MAX_COLOR_TARGETS; ++i)
		ser << info.info.clearColor[i];
	ser << info.info.clearDepth << info.info.clearStencil;
	ser << renderSys->GetTargetName(info.rt);
}

void BaseRenderPass::Load(RenderSystem *renderSys, InputSerializer& ser) {
	uint32 vLastSubTarget = 0;
	uint32 vClearFlags = 0;
	StringID target = StringUtils::NullID;
	ser >> flags >> vLastSubTarget >> vClearFlags;
	toLastSubTarget = static_cast<RenderTargetName>(vLastSubTarget);
	clearFlags = static_cast<ClearFlags>(vClearFlags);
	for (uint32 i = 0; i < RenderConstants::MAX_COLOR_TARGETS; ++i)
		ser >> info.info.clearColor[i];
	ser >> info.info.clearDepth >> info.info.clearStencil;
	ser >> target;
	info.rt = renderSys->GetTarget(target);
}

} /* namespace nextar */
