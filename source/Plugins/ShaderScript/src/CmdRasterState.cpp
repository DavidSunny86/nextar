/*
 * CmdRasterState.cpp
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#include "CmdRasterState.h"
#include <ShaderScriptContext.h>

namespace ShaderScript {

bool ShaderScript::CmdRasterState::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);
	String name;
	RasterState defaultState;
	c->rasterState = defaultState;
	if (it.HasNext(name)) {
		if (!name.compare(_SS(ARG_WIREFRAME)))
			c->rasterState.fill = FillMode::FM_WIREFRAME;
		else if (!name.compare(_SS(ARG_NO_CULL)))
			c->rasterState.cull = CullMode::CULL_NONE;
	}
	return true;
}

void ShaderScript::CmdRasterState::EndExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	c->rasterState.UpdateHash();
	c->shader->SetRasterState(c->rasterState);
}

bool ShaderScript::CmdDepthClip::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);
	String value;
	auto& rasterState = c->rasterState;
	rasterState.depthClip = true;
	if (it.HasNext(value)) {
		rasterState.depthClip = Convert::ToBool(value);
	}
	return true;
}

bool ShaderScript::CmdDepthBias::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);
	String value;
	auto& rasterState = c->rasterState;
	rasterState.usingScissors = true;
	if (it.HasNext(value)) {
		rasterState.constantDepthBias = Convert::ToFloat(value);
	}
	if (it.HasNext(value)) {
		rasterState.slopeScaledDepthBias = Convert::ToFloat(value);
	}
	if (it.HasNext(value)) {
		rasterState.depthBiasClamp = Convert::ToFloat(value);
	}
	return true;
}

bool ShaderScript::CmdScissor::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);
	String value;
	auto& rasterState = c->rasterState;
	rasterState.usingScissors = true;
	if (it.HasNext(value)) {
		rasterState.usingScissors = Convert::ToBool(value);
	}
	return true;
}

bool ShaderScript::CmdRaster::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);
	String value;
	auto& rasterState = c->rasterState;
	if (it.HasNext(value)) {
		rasterState.fill = Helper::GetFillMode(value);
	}
	if (it.HasNext(value)) {
		rasterState.cull = Helper::GetCullMode(value);
	}
	if (it.HasNext(value)) {
		rasterState.trianglesAreClockwise = Convert::ToBool(value);
	}
	return true;
}

bool ShaderScript::CmdAntiAliasing::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ShaderScriptContext* c = static_cast<ShaderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->templateResolver);
	String value;
	auto& rasterState = c->rasterState;
	rasterState.usingMultisample = true;
	if (it.HasNext(value)) {
		rasterState.usingMultisample = Convert::ToBool(value);
	}
	if (it.HasNext(value)) {
		rasterState.usingLineAa = Convert::ToBool(value);
	}
	return true;
}


} /* namespace ShaderScript */
