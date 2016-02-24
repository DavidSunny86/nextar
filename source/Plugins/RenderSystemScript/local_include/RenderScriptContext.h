/*
 * RenderScriptContext.h
 *
 *  Created on: 14-Feb-2016
 *      Author: obhi
 */

#ifndef PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_RENDERSCRIPTCONTEXT_H_
#define PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_RENDERSCRIPTCONTEXT_H_

#include <RenderSystemScript.h>
#include <CommandContext.h>

namespace RenderSystemScript {

struct Target {
	bool asTexture;
	bool isDepthTarget;
	PixelFormat format;
};

class RenderScriptContext: public nextar::CommandContext {
public:
	RenderScriptContext(RenderSystem& rsys);
	virtual ~RenderScriptContext();

	Size _bufferDim;
	uint16 _bufferDepth;
	Vector2 _bufferDimFactor;

	Target _targets[RenderConstants::MAX_RENDER_TARGETS+1];
	uint32 _numTargets;

	CompositorRenderPass::TexturesToResolve _unresolvedTextures[RenderTargetName::RT_NAME_COUNT];
	uint32 _numUnresolvedTextures;
	BaseRenderPass* _pass;
	String _passType;
	String _bufferName;
	RenderSystem& _rsys;
};

} /* namespace RenderSystemScript */

#endif /* PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_RENDERSCRIPTCONTEXT_H_ */
