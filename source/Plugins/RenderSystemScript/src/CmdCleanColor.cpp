/*
 * CmdCleanColor.cpp
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#include <CmdCleanColor.h>

namespace RenderSystemScript {

CmdCleanColor::StringToColorMap CmdCleanColor::_colorMap;

void CmdCleanColor::BeginExecute(CommandContext* pContext,
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
	uint16 index = 0;
	Color color;
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
}

}
