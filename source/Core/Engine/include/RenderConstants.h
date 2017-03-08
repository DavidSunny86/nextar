/*
 * RenderConstants.h
 *
 *  Created on: 04-Jan-2014
 *      Author: obhi
 */

#ifndef RENDERCONSTANTS_H_
#define RENDERCONSTANTS_H_

#include <NexBase.h>
#include <EngineConfig.h>

namespace nextar {

enum RenderConstants
	: uint32 {
	FORWARD_MAX_LIGHT_COUNT = 255,
	FORWARD_MAX_LIGHT_PER_PRIMITIVE = 8,

	MAX_COLOR_TARGETS = 4,
	MAX_RENDER_TARGETS = 8,
	MAX_TEXTURE_DIM = 16384,
	MAX_TEXTURE_LAYER = 2048,
	MAX_MIP_LEVELS = 14,
	MAX_VERTEX_ELEMENT = 32,
	MAX_VERTEX_STREAMS = 4,
	// maximum number of transient buffers supported
	MAX_TRANSIENT_BUFFER = 3,
	// for async capture, this will pre capture the n-3rd frame from before-hand
	MAX_FRAME_PRE_CAPTURE = 3,
};
NEX_ENUM_UINT(RenderConstants, uint32);


enum RenderTargetName : uint32 {
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
	RT_NAME_COUNT
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

enum class FrameBuffer : uint32 {
	NONE = 0,
	FRONT_LEFT = 1,
	FRONT_RIGHT,
	BACK_LEFT,
	BACK_RIGHT,
	DEPTH,

	COLOR_0,
	COLOR_1,
	COLOR_2,
	COLOR_3,
	COLOR_4,
	COLOR_5,
	COLOR_6,
	COLOR_7,

	STENCIL,
	FBTYPE_COUNT,

	FRONT = FRONT_LEFT,
	BACK = BACK_LEFT,
};

struct _NexEngineAPI PipelineStage {
	enum Name : uint32 {
		VERTEX_STAGE,
		HULL_STAGE,
		DOMAIN_STAGE,
		GEOMETRY_STAGE,
		FRAGMENT_STAGE,
		EFFECT_STAGE_COUNT,
		GPGPU_STAGE,
	};

	static const char* ToString(PipelineStage::Name name);
};

typedef uint8 ShaderUnitHandle;

}

#endif /* RENDERCONSTANTS_H_ */
