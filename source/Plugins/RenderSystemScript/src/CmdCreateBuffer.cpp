/*
 * CmdCreateBuffer.cpp
 *
 *  Created on: 17-Feb-2016
 *      Author: obhi
 */

#include <CmdCreateBuffer.h>
#include <RenderScriptContext.h>

namespace RenderSystemScript {

void CmdCreateBuffer::ParseDimension(const String& value, uint16& d, float& f) {
	float r = 0.0f;
	if (value.compare(0, 7, "view-x-") != 0) {
		d = (uint16)Convert::ToULong(value);
		f = 0.0f;
	} else {
		d = 0;
		f = Convert::ToFloat(&value.c_str()[7]);
	}
}

bool CmdCreateBuffer::BeginExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	auto it = command->GetParameters().Iterate(c->_resolver);

	c->_numTargets = 0;
	c->_bufferDim.combined = 0;
	c->_bufferDepth = 1;
	c->_bufferDimFactor = Vector2(1, 1);
	String temp;
	if (it.HasNext(temp))
		c->_bufferName = temp;
	if (it.HasNext(temp))
		ParseDimension(temp, c->_bufferDim.dx, c->_bufferDimFactor.x);
	if (it.HasNext(temp))
		ParseDimension(temp, c->_bufferDim.dy, c->_bufferDimFactor.y);
	float depthFactor;
	if (it.HasNext(temp))
		ParseDimension(temp, c->_bufferDepth, depthFactor);
	return true;
}

RenderTargetPtr CmdCreateBuffer::CreateTarget(RenderScriptContext* c,
		const Target& t, uint32 index) {
	RenderTargetPtr ret;
	String name = c->_bufferName;
	if (t.isDepthTarget) {
		name += ".depth";
	} else {
		name += ".color-";
		name += Convert::ToString(index);
	}
	StringID nameId = StringUtils::GetStringID(name);
	if (t.asTexture) {
		RenderTexture::CreateParams params;
		params.depth = c->_bufferDepth;
		params.width = c->_bufferDim.dx;
		params.height = c->_bufferDim.dy;
		params.type = TextureBase::TextureType::TEXTURE_2D;
		params.format = t.format;
		ret = c->_rsys.CreateRenderTexture(nameId, params, c->_bufferDimFactor.x,
				c->_bufferDimFactor.y);
	} else {
		RenderBuffer::CreateParams params;
		params.format = t.format;
		params.dimensions = c->_bufferDim;
		// @todo Set samples
		params.samples = 0;
		ret = c->_rsys.CreateRenderBuffer(nameId, params, c->_bufferDimFactor.x,
				c->_bufferDimFactor.y);
	}
	return ret;
}

void CmdCreateBuffer::EndExecute(CommandContext* pContext,
		const ASTCommand* command) const {
	RenderScriptContext* c = static_cast<RenderScriptContext*>(pContext);
	if (c->_bufferName == StringUtils::Null) {
		Warn("Unnamed buffer!");
		c->_bufferName = "buffer-";
		c->_bufferName += Convert::ToString(c->_rsys.GetRenderTargetCount());
	}

	StringID bufferNameId = StringUtils::GetStringID(c->_bufferName);

	if (c->_numTargets == 1) {
		RenderTargetPtr r = CreateTarget(c, c->_targets[0], 0);
		c->_rsys.RegisterTarget(bufferNameId, r);
	} else {
		MultiRenderTarget::CreateParams params;
		params.dimensions = c->_bufferDim;
		params.numColorTargets = 0;
		params.useDepth = false;
		for(uint32 i = 0; i < c->_numTargets; ++i) {
			auto& t = c->_targets[i];
			if(t.isDepthTarget) {
				params.useDepth = true;
				params.depth.format = t.format;
				params.depth.useAsTexture = t.asTexture;
				params.depth.useTarget = CreateTarget(c, t, 0);
			} else {
				auto& t2 = params.targets[params.numColorTargets++];
				t2.format = t.format;
				t2.useAsTexture = t.asTexture;
				t2.useTarget = CreateTarget(c, t, params.numColorTargets-1);
			}
		}
		c->_rsys.CreateMultiRenderTarget(bufferNameId, params, c->_bufferDimFactor.x, c->_bufferDimFactor.y);
	}
}

} /* namespace RenderSystemScript */
