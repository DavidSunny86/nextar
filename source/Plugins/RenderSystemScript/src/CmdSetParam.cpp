/*
 * CmdSetParam.cpp
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#include <CompositorRenderPass.h>
#include <CmdSetParam.h>

namespace RenderSystemScript {

void CmdSetParam::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {

	ConstMultiStringHelper h(command->GetParameters().AsString());
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	if (c->_passType.find("Compositor") != String::npos) {
		CompositorRenderPass* pass = static_cast<CompositorRenderPass*>(c->_pass);
		String temp;
		const ParameterBuffer& pb = pass->GetParameterBuffer();
		auto it = h.Iterate();
		if (it.HasNext(temp)) {
			auto entryAndOffset = pb.GetParamEntryAndOffset(temp);
			if (entryAndOffset.first) {
				ShaderParamAgent* agent=ShaderParamAgent::GetAgent(entryAndOffset.first->type);
				if (agent) {
					String valueList;
					while( (it.HasNext(temp)) ) {
						StringUtils::PushBackWord(valueList, temp);
					}
					agent->SetParamValue(entryAndOffset.second, &pb, *entryAndOffset.first, valueList);
				}
			}

		}
	}
}

} /* namespace RenderSystemScript */
