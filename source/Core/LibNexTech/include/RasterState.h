#ifndef NEXTAR_RASTERSTATE_H
#define NEXTAR_RASTERSTATE_H

#include "NexSys.h"

namespace nextar {

	typedef uint8 FillModeType;
	typedef uint8 CullModeType;

	enum FillMode {
		FM_SOLID,
		FM_WIREFRAME,
		FM_POINT,

		FM_DEFAULT = FM_SOLID
	};

	enum CullMode {
		CULL_BACK,
		CULL_FRONT,
		CULL_ALL,
		CULL_NONE,

		CULL_DEFAULT = CULL_BACK
	};

	/**
	 * @brief	Description of the raster state.
	 *
	 * @author	Abhishek Dey
	 * @date	12/25/2010
	 */
	struct RasterState {
		/* true if triangles are clockwise */
		bool trianglesAreClockwise;
		/* true to depth clip */
		bool depthClip;
		/* true if using scissor rectangles */
		bool usingScissors;
		/* true if using multisampling */
		bool usingMultisample;
		/* true if using line anti aliasing */
		bool usingLineAa;
		/* The fill mode */
		FillModeType fill;
		/* The cull mode */
		CullModeType cull;
		/* padding the bools and uint8's */
		bool _irrelevant_padding;
		/* The constant depth bias */
		float constantDepthBias;
		/* The slope sclaed depth bias */
		float slopeScaledDepthBias;
		/* The depth bias clamp */
		float depthBiasClamp;

		RasterState()
				: trianglesAreClockwise(true), depthClip(false), usingScissors(
						false), usingMultisample(false), usingLineAa(false), fill(
						FM_SOLID), cull(CULL_BACK), _irrelevant_padding(0),
						constantDepthBias(0), slopeScaledDepthBias(
						0), depthBiasClamp(0) {
		}
	};
}

#endif //NEXTAR_RASTERSTATE_H
