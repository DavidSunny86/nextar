/*
 * CopyRenderPass.cpp
 *
 *  Created on: 08-Feb-2016
 *      Author: obhi
 */

#include <CompositorRenderPass.h>

namespace nextar {

CompositorRenderPass::CompositorRenderPass() : numTextureToResolve(0) {
	_rtBunchOf = nullptr;
}

CompositorRenderPass::~CompositorRenderPass() {
}


void CompositorRenderPass::Commit(CommitContext& context) {
	BeginRender(context);

	if (numTextureToResolve == 1) {
		TextureUnit tu;
		tu.texture = context.GetTargetByName(_rtJustOne.name);
		parameters.SetData(&tu, _rtJustOne.offset);
	} else {
		for(uint32 i = 0; i < numTextureToResolve; ++i) {
			TextureUnit tu;
			tu.texture = context.GetTargetByName(_rtBunchOf[i].name);
			parameters.SetData(&tu, _rtBunchOf[i].offset);
		}
	}

	RenderPrimitive(context, (uint32)(ptrdiff_t)this, &primitive);
	EndRender(context);
}

void CompositorRenderPass::CreateResources() {
	BaseMaterialPass::CreateResources();
	if (material) {
		primitive.material = material;
		primitive.parameters = &parameters;
		primitive.streamData = &RenderManager::Instance().GetFullScreenQuad();
	}
}

void CompositorRenderPass::DestroyResources() {
	primitive.material.Clear();
	primitive.streamData = nullptr;
	primitive.parameters = nullptr;
	parameters.Clear();
	BaseMaterialPass::DestroyResources();
}

} /* namespace nextar */
