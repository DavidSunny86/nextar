/*
 * CmdTarget.cpp
 *
 *  Created on: 17-Feb-2016
 *      Author: obhi
 */

#include <CmdTarget.h>
#include <RenderScriptContext.h>

namespace RenderSystemScript {

bool CmdTarget::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->_resolver);
	if ( c->_numTargets >= RenderConstants::MAX_RENDER_TARGETS )
		return true;
	Target& data = c->_targets[c->_numTargets++];
	data.format = PixelFormat::UNKNOWN;
	data.asTexture = true;
	data.isDepthTarget = false;
	String temp;
	if (it.HasNext(temp)) {
		data.isDepthTarget = (temp == "depth") != 0;
	}
	if (it.HasNext(temp)) {
		data.format = PixelUtils::GetFormatFromString(temp);
	}
	if (it.HasNext(temp)) {
		data.asTexture = Convert::ToBool(temp);
	}
	return true;
}

} /* namespace RenderSystemScript */
