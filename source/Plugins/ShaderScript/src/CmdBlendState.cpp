/*
 * CmdBlendState.cpp
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#include <CmdBlendState.h>
#include <ShaderScriptContext.h>

namespace ShaderScript {

bool ShaderScript::CmdBlendState::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	BlendState defaultState;
	c->blendState = defaultState;
	String param = h.Get(0);
	if (!Helper::IsDefault(param) && !Helper::IsDisabled(param))
		c->blendState.enabled = true;
	c->blendState.numRenderTargets = 0;
	return true;
}

void ShaderScript::CmdBlendState::EndExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	c->blendState.UpdateHash();
	c->shader->SetBlendState(c->blendState);
}

bool ShaderScript::CmdTarget::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto& blendOp = c->blendState.blendOp[c->blendState.numRenderTargets++];
	String value;
	auto it = h.Iterate();
	if (it.HasNext(value)) {
		if (!value.compare("alpha-blending"))
			blendOp = _alphaBlending;
		else if (!value.compare("advanced")) {
			if (it.HasNext(value)) {
				blendOp.srcCol = Helper::GetBlendSource(value);
			}
			if (it.HasNext(value)) {
				blendOp.destCol = Helper::GetBlendSource(value);
			}
			if (it.HasNext(value)) {
				blendOp.colOp = Helper::GetBlendOperation(value);
			}
			if (it.HasNext(value)) {
				blendOp.srcAlpha = Helper::GetBlendSource(value);
			}
			if (it.HasNext(value)) {
				blendOp.destAlpha = Helper::GetBlendSource(value);
			}
			if (it.HasNext(value)) {
				blendOp.alphaOp = Helper::GetBlendOperation(value);
			}
		}
	}
	return true;
}

bool ShaderScript::CmdAlphaToCoverage::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = h.Iterate();
	String value;

	c->blendState.alphaToCoverage = true;
	if (it.HasNext(value)) {
		c->blendState.alphaToCoverage = Convert::ToBool(value);
	}
	return true;
}

} /* namespace ShaderScript */
