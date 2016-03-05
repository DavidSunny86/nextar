/*
 * CmdDepthStencilState.cpp
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#include "CmdDepthStencilState.h"
#include <ShaderScriptContext.h>

namespace ShaderScript {

bool ShaderScript::CmdDepthStencilState::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	DepthStencilState defaultState;
	c->depthStencilState = defaultState;
	String param = h.Get(0);
	if (Helper::IsDisabled(param)) {
		c->depthStencilState.depthTest = false;
		c->depthStencilState.stencilTest = false;
	}
	return true;
}

void ShaderScript::CmdDepthStencilState::EndExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	c->depthStencilState.UpdateHash();
	c->shader->SetDepthStencilState(c->depthStencilState);
}

bool ShaderScript::CmdDepthState::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	String value;
	auto it = h.Iterate();
	auto& depthStencil = c->depthStencilState;
	if (it.HasNext(value)) {
		// expected and set
		if (!value.compare("disable")) {
			depthStencil.depthTest = false;
		} else if (!value.compare("test-and-write")) {
		} else if (!value.compare("advanced")) {
			if (it.HasNext(value)) {
				depthStencil.depthTest = Convert::ToBool(value);
			}
			if (it.HasNext(value)) {
				depthStencil.depthWrite = Convert::ToBool(value);
			}
			if (it.HasNext(value)) {
				depthStencil.depthCompareFunc = Helper::GetDepthStencilFunc(value);
			}
		}
	}
	return true;
}

bool ShaderScript::CmdStencilState::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	String value;
	auto it = h.Iterate();
	auto& depthStencil = c->depthStencilState;
	if (it.HasNext(value)) {
		// expected and set
		if (!value.compare("disable")) {
			depthStencil.stencilTest = false;
		} else if (!value.compare("advanced")) {
			StencilFaceOp* faceOp = &depthStencil.front;
			if (it.HasNext(value) && value == "back") {
				faceOp = &depthStencil.back;
			}
			if (it.HasNext(value)) {
				faceOp->stencilFunc = Helper::GetDepthStencilFunc(value);
			}
			if (it.HasNext(value)) {
				faceOp->stencilFail = Helper::GetStencilOp(value);
			}
			if (it.HasNext(value)) {
				faceOp->stencilPass = Helper::GetStencilOp(value);
			}
			if (it.HasNext(value)) {
				faceOp->depthPass = Helper::GetStencilOp(value);
			}
			if (it.HasNext(value)) {
				faceOp->stencilMask = Convert::ToULong(value);
			}
			if (it.HasNext(value)) {
				faceOp->stencilRef = Convert::ToULong(value);
			}
		}
	}
	return true;
}

} /* namespace ShaderScript */
