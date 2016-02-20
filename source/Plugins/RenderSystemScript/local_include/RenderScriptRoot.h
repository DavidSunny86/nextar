/*
 * RenderScriptRoot.h
 *
 *  Created on: 14-Feb-2016
 *      Author: obhi
 */

#ifndef PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_RENDERSCRIPTROOT_H_
#define PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_RENDERSCRIPTROOT_H_

#include <RenderSystemScript.h>
#include <RootBlockCommandHandler.h>

namespace RenderSystemScript {

class RenderScriptRoot: public nextar::RootBlockCommandHandler {
public:
	RenderScriptRoot();
	virtual ~RenderScriptRoot();

	virtual void BeginDocument(CommandContext* pContext, const ASTDocument* pDoc) const;
	virtual void EndDocument(CommandContext* pContext, const ASTDocument* pDoc) const;
};

} /* namespace RenderSystemScript */

#endif /* PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_RENDERSCRIPTROOT_H_ */
