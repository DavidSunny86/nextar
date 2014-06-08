/*
 * RenderConstants.h
 *
 *  Created on: 04-Jan-2014
 *      Author: obhi
 */

#ifndef RENDERCONSTANTS_H_
#define RENDERCONSTANTS_H_

#include <NexBase.h>

namespace nextar {

	enum class RenderConstants : uint32 {
			FORWARD_MAX_LIGHT_COUNT = 255,
			FORWARD_MAX_LIGHT_PER_PRIMITIVE = 8,

			MAX_RENDER_TARGETS = 8,
			MAX_TEXTURE_DIM = 16384,
			MAX_TEXTURE_LAYER = 2048,
			MAX_MIP_LEVELS = 14,
			MAX_PROGRAM_STAGES = 5,
			MAX_VERTEX_ELEMENT = 32,
			MAX_VERTEX_STREAMS = 4,
			// for async capture, this will pre capture the n-3rd frame from before-hand
			MAX_FRAME_PRE_CAPTURE = 3,
	};

	NEX_ENUM_UINT(RenderConstants, uint32);
}


#endif /* RENDERCONSTANTS_H_ */
