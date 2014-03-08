#ifndef NEXTAR_BLENDING_H
#define NEXTAR_BLENDING_H

#include "NexSys.h"
#include "RenderConstants.h"

namespace nextar {

	typedef uint8 BlendDataSourceType;
	typedef uint8 BlendOpType;

	enum BlendDataSource {
		BDS_ONE,
		BDS_ZERO,
		BDS_ALPHA,
		BDS_INV_ALPHA,
		BDS_SRC_COLOR,
		BDS_INV_SRC_COLOR,
		BDS_DST_ALPHA,
		BDS_INV_DST_ALPHA,
		BDS_DST_COLOR,
		BDS_INV_DST_COLOR,
		BDS_DEFAULT,
	};

	enum BlendOp {
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
		ColorMaskType mask;
		BlendDataSourceType srcCol;
		BlendDataSourceType destCol;
		BlendOpType colOp;
		BlendDataSourceType srcAlpha;
		BlendDataSourceType destAlpha;
		BlendOpType alphaOp;

		RenderTargetBlendOp()
				: enabled(false), mask(0xff), srcCol(BDS_ZERO), destCol(
						BDS_ZERO), colOp(BOP_NONE), srcAlpha(BDS_ZERO), destAlpha(
						BDS_ZERO), alphaOp(BOP_NONE) {
		}
	};

	struct BlendState {
		bool enabled;
		bool alphaToCoverage;
		uint16 numRenderTargets;
		RenderTargetBlendOp blendOp[(uint32)RenderConstants::MAX_RENDER_TARGETS];

		BlendState() : enabled(false), alphaToCoverage(false), numRenderTargets(0) {
		}
	};
}

#endif //NEXTAR_BLENDING_H
