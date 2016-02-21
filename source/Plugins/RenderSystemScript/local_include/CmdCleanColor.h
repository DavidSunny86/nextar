/*
 * CmdCleanColor.h
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#ifndef PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDCLEANCOLOR_H_
#define PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDCLEANCOLOR_H_

#include <RenderSystemScript.h>
#include <CommandHandler.h>

namespace RenderSystemScript {

class CmdCleanColor: public nextar::CommandHandler {
public:
	NEX_SINGLE_INSTANCE(CmdCleanColor);

	virtual void BeginExecute(CommandContext* pContext, const ASTCommand* command) const;

protected:
	typedef map<String, Color>::type StringToColorMap;
	static StringToColorMap _colorMap;
};

} /* namespace RenderSystemScript */

#endif /* PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_CMDCLEANCOLOR_H_ */
