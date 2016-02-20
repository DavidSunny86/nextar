/*
 * RenderPass.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef RENDERPASS_H_
#define RENDERPASS_H_

#include <RenderContext.h>
#include <VisibilitySet.h>

namespace nextar {

enum RenderTargetName {
	LAST_RT_MT,
	LAST_RT_COLOR_0,
	LAST_RT_COLOR_1,
	LAST_RT_COLOR_3,
	LAST_RT_COLOR_4,
	LAST_RT_COLOR_5,
	LAST_RT_DEPTH,

	VIEW_RT_MT,
	VIEW_RT_COLOR_0,
	VIEW_RT_COLOR_1,
	VIEW_RT_COLOR_3,
	VIEW_RT_COLOR_4,
	VIEW_RT_COLOR_5,
	VIEW_RT_DEPTH,

	RT_NONE,
};

enum class ClearFlags : uint16 {
	CLEAR_NONE = 0,
	CLEAR_COLOR_I = 0,
	CLEAR_COLOR_0 = 1 << 0,
	CLEAR_COLOR_1 = 1 << 1,
	CLEAR_COLOR_2 = 1 << 2,
	CLEAR_COLOR_3 = 1 << 3,
	CLEAR_COLOR_4 = 1 << 4,
	CLEAR_COLOR_5 = 1 << 5,
	CLEAR_DEPTH = 1 << 6,
	CLEAR_STENCIL = 1 << 7,
	CLEAR_COLOR = CLEAR_COLOR_0|CLEAR_COLOR_1|CLEAR_COLOR_2|CLEAR_COLOR_3|CLEAR_COLOR_4|CLEAR_COLOR_5,
	CLEAR_ALL = CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL
};
NEX_ENUM_FLAGS(ClearFlags, uint16);

struct ClearBufferInfo {
	uint16 clearStencil;
	float clearDepth;
	Color clearColor[RenderConstants::MAX_COLOR_TARGETS];
	ClearBufferInfo() : clearDepth(1.0f), clearStencil(0) {}
};

struct RenderInfo {
	RenderTarget* rt;
	ClearBufferInfo info;
	RenderInfo() : rt(0) {}
};

class _NexEngineAPI RenderPass: public AllocGraphics {
public:
	
	typedef RenderPass* (*CreateInstance) ();

	RenderPass();
	virtual ~RenderPass();
		
	virtual void Commit(CommitContext& context) = 0;
};

typedef vector<RenderPass*>::type RenderPassList;

} /* namespace nextar */
#endif /* RENDERPASS_H_ */
