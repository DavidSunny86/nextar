#ifndef NEXTAR_BLENDING_H
#define NEXTAR_BLENDING_H

#include <NexBase.h>
#include <RenderStateBase.h>

namespace nextar {


enum BlendDataSource : uint8 {
	BDS_ONE,
	BDS_ZERO,
	BDS_SRC_ALPHA,
	BDS_INV_SRC_ALPHA,
	BDS_SRC_COLOR,
	BDS_INV_SRC_COLOR,
	BDS_DST_ALPHA,
	BDS_INV_DST_ALPHA,
	BDS_DST_COLOR,
	BDS_INV_DST_COLOR,
	BDS_DEFAULT,
};

enum BlendOp : uint8 {
	BOP_NONE,
	BOP_ADD,
	BOP_SUB,
	BOP_INV_SUB,
	BOP_MIN,
	BOP_MAX,
};

struct RenderTargetBlendOp {
	/* default: false */
	bool enabled;
	// this should be a mask
	ColorMask mask;
	BlendDataSource srcCol;
	BlendDataSource destCol;
	BlendOp colOp;
	BlendDataSource srcAlpha;
	BlendDataSource destAlpha;
	BlendOp alphaOp;

	RenderTargetBlendOp(ColorMask m, BlendDataSource sc, BlendDataSource dc, BlendOp c,
			BlendDataSource sa, BlendDataSource da, BlendOp a) : enabled(true), mask(m), srcCol(sc),
			destCol(dc), colOp(c), srcAlpha(sa), destAlpha(da), alphaOp(a) {
	}

	RenderTargetBlendOp() :
			enabled(false), mask((ColorMask) 0xff), srcCol(BDS_ZERO), destCol(
					BDS_ZERO), colOp(BOP_NONE), srcAlpha(BDS_ZERO),
					destAlpha(BDS_ZERO), alphaOp(BOP_NONE) {
	}
};

struct _NexEngineAPI BlendState : pubRenderStateBasetate<BlendState> {
	bool enabled;
	bool alphaToCoverage;
	uint16 numRenderTargets;
	RenderTargetBlendOp blendOp[(uint32) RenderConstants::MAX_RENDER_TARGETS];

	BlendState() :
			enabled(false), alphaToCoverage(false), numRenderTargets(1) {
	}
};
}

#endif //NEXTAR_BLENDING_H