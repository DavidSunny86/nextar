/*
 * RenderScriptContext.cpp
 *
 *  Created on: 14-Feb-2016
 *      Author: obhi
 */

#include <RenderScriptContext.h>

namespace RenderSystemScript {

RenderScriptContext::RenderScriptContext(RenderSystem& rsys) :
_rsys(rsys)
,_pass(nullptr)
,_numTargets(0)
{
}

RenderScriptContext::~RenderScriptContext() {
}

} /* namespace RenderSystemScript */
