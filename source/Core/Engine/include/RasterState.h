#ifndef NEXTAR_RASTERSTATE_H
#define NEXTAR_RASTERSTATE_H

#include <NexBase.h>
#include <RenderStateDesc.h>

namespace nextar {

enum FillMode : uint8 {
	FM_SOLID,
	FM_WIREFRAME,
	FM_POINT,
	FM_DEFAULT = FM_SOLID
};

enum CullMode : uint8 {
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
struct _NexEngineAPI RasterState : public RenderStateDesc<RasterState> {
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
	FillMode fill;
	/* The cull mode */
	CullMode cull;
	/* padding the bools and uint8's */
	bool _irrelevant_padding;
	/* The constant depth bias */
	float constantDepthBias;
	/* The slope sclaed depth bias */
	float slopeScaledDepthBias;
	/* The depth bias clamp */
	float depthBiasClamp;
	// hash
	RasterState() :
			trianglesAreClockwise(true), depthClip(false), usingScissors(
			false), usingMultisample(false), usingLineAa(false), fill(FM_SOLID), cull(
					CULL_BACK), _irrelevant_padding(0), constantDepthBias(0), 
					slopeScaledDepthBias(0), depthBiasClamp(0)
					 {
	}
};
}

#endif //NEXTAR_RASTERSTATE_H