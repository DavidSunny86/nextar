#ifndef NEXTAR_DEPTHSTENCILSTATE_H
#define NEXTAR_DEPTHSTENCILSTATE_H

#include <NexBase.h>

namespace nextar {

enum DepthStencilCompare : uint8 {
	DSCOMP_NEVER,
	DSCOMP_LESS,
	DSCOMP_EQUAL,
	DSCOMP_LESS_EQUAL,
	DSCOMP_GREATER,
	DSCOMP_NOT_EQUAL,
	DSCOMP_GREATER_EQUAL,
	DSCOMP_ALWAYS,

	DSCOMP_DEFAULT,
};

enum StencilOp : uint8 {
	STENCILOP_KEEP,
	STENCILOP_ZERO,
	STENCILOP_REPLACE,
	STENCILOP_INCR_SAT,
	STENCILOP_DECR_SAT,
	STENCILOP_INVERT,
	STENCILOP_INCR,
	STENCILOP_DECR,
};

struct StencilFaceOp {
	uint32 stencilMask;
	int32 stencilRef;
	DepthStencilCompare stencilFunc;
	StencilOp stencilFail;
	StencilOp stencilPass;
	StencilOp depthPass;

	StencilFaceOp() :
			stencilMask(0xffffffff), 
			stencilRef(0),
			stencilFunc(DSCOMP_NEVER), 
			stencilFail(STENCILOP_ZERO), 
			stencilPass(STENCILOP_ZERO), 
			depthPass(STENCILOP_ZERO) {
	}
};

struct DepthStencilState {
	/* true to enable depth test */
	bool depthTest;
	/* true to enable depth write */
	bool depthWrite;
	/* The depth Compare function */
	DepthStencilCompare depthCompareFunc;
	/* true to enable stencil test */
	bool stencilTest;
	/* stencil operations */
	StencilFaceOp front;
	StencilFaceOp back;

	StringID hash;

	inline void UpdateHash() {}

	DepthStencilState() :
		depthTest(true), depthWrite(true), depthCompareFunc(DSCOMP_LESS_EQUAL),
		stencilTest(false), hash(StringUtils::NullID) {
	}
};
}

#endif //NEXTAR_DEPTHSTENCILSTATE_H