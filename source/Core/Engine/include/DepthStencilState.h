#ifndef NEXTAR_DEPTHSTENCILSTATE_H
#define NEXTAR_DEPTHSTENCILSTATE_H

#include <NexBase.h>

namespace nextar {

	enum DepthStencilCompare {
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

	enum StencilOp {
		STENCILOP_KEEP,
		STENCILOP_ZERO,
		STENCILOP_REPLACE,
		STENCILOP_INCR_SAT,
		STENCILOP_DECR_SAT,
		STENCILOP_INVERT,
		STENCILOP_INCR,
		STENCILOP_DECR,
	};

	typedef uint8 StencilOpType;
	typedef uint8 DepthStencilCompareType;

	struct StencilFaceOp {
		uint8 stencilMask;
		DepthStencilCompareType stencilFunc;
		StencilOpType stencilFail;
		StencilOpType stencilPass;
		StencilOpType depthPass;

		StencilFaceOp() : stencilMask(0xff),
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
		DepthStencilCompareType depthCompareFunc;
		/* true to enable stencil test */
		bool stencilTest;
		/* stencil operations */
		StencilFaceOp front;
		StencilFaceOp back;

		DepthStencilState()
				: depthTest(true), depthWrite(true), depthCompareFunc(
						DSCOMP_LESS), stencilTest(false) {
		}
	};
}

#endif //NEXTAR_DEPTHSTENCILSTATE_H
