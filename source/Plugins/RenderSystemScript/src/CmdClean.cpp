/*
 * CmdClean.cpp
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#include <CmdClean.h>
#include <RenderScriptContext.h>

namespace RenderSystemScript {

CmdClean::StringToColorMap CmdClean::_colorMap;

bool CmdClean::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {

	if (_colorMap.size() <= 0) {
		_colorMap["black"] = Color::Black;
		_colorMap["blue"] = Color::Blue;
		_colorMap["red"] = Color::Red;
		_colorMap["green"] = Color::Green;
		_colorMap["white"] = Color::White;
	}

	ConstMultiStringHelper h(command->GetParameters().AsString());
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	auto it = h.Iterate();
	String value;
	if (it.HasNext(value)) {
		if (value == "depth") {
			float cleanDepthValue = 1.0f;
			if (it.HasNext(value))
				cleanDepthValue = Convert::ToFloat(h.Get(0));
			c->_pass->SetCleanDepth(true, cleanDepthValue);
		} else if (value == "color") {
			uint16 index = 0;
			Color color = Color::Black;
			if (it.HasNext(value)) {
				index = Convert::ToULong(value);
			}
			if (it.HasNext(value)) {
				auto it2 = _colorMap.find(value);
				if (it2 == _colorMap.end())
					color = Convert::ToColor(value);
				else
					color = (*it2).second;
			}
			c->_pass->SetCleanColor(index, color);
		} else if (value == "stencil") {
			int16 stencil = 0;
			if (it.HasNext(value)) {
				stencil = (int16)Convert::ToLong(value);
			}
			c->_pass->SetCleanStencil(true, stencil);
		}
	}
	return true;
}

}
