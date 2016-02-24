/*
 * CmdSetParam.cpp
 *
 *  Created on: 20-Feb-2016
 *      Author: obhi
 */

#include <CompositorRenderPass.h>
#include <CmdSetParam.h>
#include <ShaderParamAgent.h>
#include <RenderScriptContext.h>

namespace RenderSystemScript {

void CmdSetParam::BeginExecute(CommandContext* pContext,
	const ASTCommand* command) const {

	ConstMultiStringHelper h(command->GetParameters().AsString());
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	if (c->_passType.find("Compositor") != String::npos) {
		CompositorRenderPass* pass = static_cast<CompositorRenderPass*>(c->_pass);
		String temp;
		ParameterBuffer& pb = pass->GetParameterBuffer();
		auto it = h.Iterate();
		if (it.HasNext(temp)) {
			auto entryAndOffset = pb.GetParamEntryAndOffset(temp);
			if (entryAndOffset.first) {
				String valueList;
				while ((it.HasNext(temp))) {
					StringUtils::PushBackWord(valueList, temp);
				}

				if (entryAndOffset.first->type == ParamDataType::PDT_TEXTURE) {
					// check if the name is an URL, 
					// temp - should contain the name
					RenderTargetName lastTarget = CommitContext::ParseTargetName(temp);
					if (lastTarget != RenderTargetName::RT_NONE) {
						c->_unresolvedTextures[c->_numUnresolvedTextures++] =
							CompositorRenderPass::TexturesToResolve{ lastTarget, entryAndOffset.second };
						return;
					}
				}
				ShaderParamAgent* agent = ShaderParamAgent::GetAgent(
					entryAndOffset.first->type);
				if (agent) {
					agent->SetParamValue(entryAndOffset.second, &pb, *entryAndOffset.first, valueList);
				}
			}
		}

	}
}


} /* namespace RenderSystemScript */
