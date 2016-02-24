/*
 * CmdTarget.cpp
 *
 *  Created on: 17-Feb-2016
 *      Author: obhi
 */

#include <CmdTarget.h>
#include <RenderScriptContext.h>

namespace RenderSystemScript {

void CmdTarget::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	ConstMultiStringHelper h(command->GetParameters().AsString());
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	if ( c->_numTargets >= RenderConstants::MAX_RENDER_TARGETS )
		return;
	Target& data = c->_targets[c->_numTargets++];
	auto it = h.Iterate();
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
}

} /* namespace RenderSystemScript */
