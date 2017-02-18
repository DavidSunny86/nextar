/*
 * CmdBlendState.cpp
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#include <CmdBlendState.h>
#include <ShaderScriptContext.h>

namespace ShaderScript {

const RenderTargetBlendOp CmdTarget::_alphaBlending(ColorMask::MASK_ALL,
	BlendDataSource::BDS_SRC_ALPHA, BlendDataSource::BDS_INV_DST_ALPHA, BlendOp::BOP_ADD,
	BlendDataSource::BDS_ONE, BlendDataSource::BDS_ZERO, BlendOp::BOP_ADD);

const RenderTargetBlendOp CmdTarget::_preMultipliedBlending(ColorMask::MASK_ALL,
	BlendDataSource::BDS_SRC_ALPHA, BlendDataSource::BDS_INV_DST_ALPHA, BlendOp::BOP_ADD,
	BlendDataSource::BDS_ONE, BlendDataSource::BDS_INV_SRC_ALPHA, BlendOp::BOP_ADD);

bool CmdBlendState::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto h = command->GetParameters().Iterate(c->templateResolver);
	BlendState defaultState;
	c->blendState = defaultState;
	String param;
	if (h.HasNext(param) && !Helper::IsDefault(param) && !Helper::IsDisabled(param))
		c->blendState.enabled = true;
	c->blendState.numRenderTargets = 0;
	return true;
}

void CmdBlendState::EndExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	c->blendState.UpdateHash();
	c->shader->SetBlendState(c->blendState);
}

bool CmdTarget::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);
	auto& blendOp = c->blendState.blendOp[c->blendState.numRenderTargets++];
	String value;
	if (it.HasNext(value)) {
		if (!value.compare("pre-multiplied-blending"))
			blendOp = _preMultipliedBlending;
		else if (!value.compare("alpha-blending"))
			blendOp = _alphaBlending;
		else if (!value.compare("advanced")) {
			const ASTParameter& param = command->GetParameters();
			if (param.Find("src-color", value)) {
				blendOp.srcCol = Helper::GetBlendSource(value);
			}
			if (param.Find("dest-color", value)) {
				blendOp.destCol = Helper::GetBlendSource(value);
			}
			if (param.Find("color-op", value)) {
				blendOp.colOp = Helper::GetBlendOperation(value);
			}
			if (param.Find("src-alpha", value)) {
				blendOp.srcAlpha = Helper::GetBlendSource(value);
			}
			if (param.Find("dest-alpha", value)) {
				blendOp.destAlpha = Helper::GetBlendSource(value);
			}
			if (param.Find("alpha-op", value)) {
				blendOp.alphaOp = Helper::GetBlendOperation(value);
			}
		}
	}
	return true;
}

bool CmdAlphaToCoverage::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);
	String value;

	c->blendState.alphaToCoverage = true;
	if (it.HasNext(value)) {
		c->blendState.alphaToCoverage = Convert::ToBool(value);
	}
	return true;
}

} /* namespace ShaderScript */
