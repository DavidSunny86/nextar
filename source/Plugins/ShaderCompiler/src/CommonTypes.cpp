/*
 * CommonTypes.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#include <BaseHeaders.h>
#include <CommonTypes.h>

#define NEX_SHADER_STRINGS_DEFINE
#include <ScriptStrings.h>
#undef NEX_SHADER_STRINGS_DEFINE

namespace ShaderCompiler {

/**************************************************************
	* Helper
	**************************************************************/
CommandDelegate_Execute Helper::FindCommand(CommandNamePair cmdMap[], size_t arraySize,
	const String& name) {
	CommandNamePair* ptr = std::lower_bound(cmdMap, cmdMap + arraySize, name,
		CommandNameCompare());
	if (ptr != cmdMap + arraySize && !(name.compare(ptr->name) < 0))
		return static_cast<CommandDelegate_Execute>(ptr->command);
	return 0;
}

ComparisonFunc Helper::GetComparisonFunc(const String& val) {
	static EnumNamePair names[] = {
		{ NFX_COMP_ALWAYS, _SS(ARG_ALWAYS) },
		{ NFX_COMP_EQUAL, _SS(ARG_EQUAL) },
		{ NFX_COMP_GREATER, _SS(ARG_GREATER) },
		{ NFX_COMP_GREATER_EQUAL, _SS(ARG_GREATER_EQUAL) },
		{ NFX_COMP_LESS, _SS(ARG_LESS) },
		{ NFX_COMP_LESS_EQUAL, _SS(ARG_LESS_EQUAL) },
		{ NFX_COMP_NEVER, _SS(ARG_NEVER) },
		{ NFX_COMP_NOT_EQUAL, _SS(ARG_NOT_EQUAL) },
	};

	const size_t arraySize = sizeof(names) / sizeof(names[0]);

	EnumNamePair* ptr = std::lower_bound(names, names + arraySize, val,
		EnumNameCompare());
	if (ptr != names + arraySize && !(val.compare(ptr->name) < 0))
		return static_cast<ComparisonFunc>(ptr->val);

	return NFX_COMP_UNKNOWN;
}

StencilOp Helper::GetStencilOp(const String& val) {
	static EnumNamePair names[] = {
		{ STENCILOP_DECR, _SS(ARG_DECREMENT) },
		{ STENCILOP_DECR_SAT, _SS(ARG_DECREMENT_SATURATE) },
		{ STENCILOP_INCR, _SS(ARG_INCREMENT) },
		{ STENCILOP_INCR_SAT, _SS(ARG_INCREMENT_SATURATE) },
		{ STENCILOP_INVERT, _SS(ARG_INVERT) },
		{ STENCILOP_KEEP, _SS(ARG_KEEP) },
		{ STENCILOP_REPLACE, _SS(ARG_REPLACE) },
		{ STENCILOP_ZERO, _SS(ARG_ZERO) },
	};

	const size_t array_size = sizeof(names) / sizeof(names[0]);

	EnumNamePair* ptr = std::lower_bound(names, names + array_size, val,
		EnumNameCompare());
	if (ptr != names + array_size && !(val.compare(ptr->name) < 0))
		return static_cast<StencilOp>(ptr->val);

	return STENCILOP_ZERO;
}

DepthStencilCompare Helper::GetDepthStencilFunc(const String& val) {
	switch (GetComparisonFunc(val)) {
	case NFX_COMP_ALWAYS:
		return DSCOMP_ALWAYS;
	case NFX_COMP_EQUAL:
		return DSCOMP_EQUAL;
	case NFX_COMP_GREATER:
		return DSCOMP_GREATER;
	case NFX_COMP_GREATER_EQUAL:
		return DSCOMP_GREATER_EQUAL;
	case NFX_COMP_LESS:
		return DSCOMP_LESS;
	case NFX_COMP_LESS_EQUAL:
		return DSCOMP_LESS_EQUAL;
	case NFX_COMP_NEVER:
		return DSCOMP_NEVER;
	case NFX_COMP_NOT_EQUAL:
		return DSCOMP_NOT_EQUAL;
	}
	return DSCOMP_DEFAULT;
}

ColorMask Helper::GetColorMask(const String& val) {
	ColorMask mask = (ColorMask)0;
	uint32 trv = std::min((uint32)4, (uint32)val.length());
	for (size_t i = 0; i < trv; ++i) {
		switch (val[i]) {
		case 'r':
			mask |= ColorMask::MASK_RED;
			break;
		case 'g':
			mask |= ColorMask::MASK_GREEN;
			break;
		case 'b':
			mask |= ColorMask::MASK_BLUE;
			break;
		case 'a':
			mask |= ColorMask::MASK_ALPHA;
			break;
		}
	}
	return mask;
}

BlendDataSource Helper::GetBlendSource(const String& val) {
	static EnumNamePair names[] = {
		{ BDS_DST_ALPHA, _SS(ARG_DEST_ALPHA) },
		{ BDS_DST_COLOR, _SS(ARG_DEST_COLOR) },
		{ BDS_INV_DST_ALPHA, _SS(ARG_INV_DEST_ALPHA) },
		{ BDS_INV_DST_COLOR, _SS(ARG_INV_DEST_COLOR) },
		{ BDS_INV_SRC_ALPHA, _SS(ARG_INV_SRC_ALPHA) },
		{ BDS_INV_SRC_COLOR, _SS(ARG_INV_SRC_COLOR) },
		{ BDS_ONE, _SS(ARG_ONE) },
		{ BDS_SRC_ALPHA, _SS(ARG_SRC_ALPHA) },
		{ BDS_SRC_COLOR, _SS(ARG_SRC_COLOR) },
		{ BDS_ZERO, _SS(ARG_ZERO) },
	};

	const size_t array_size = sizeof(names) / sizeof(names[0]);

	EnumNamePair* ptr = std::lower_bound(names, names + array_size, val,
		EnumNameCompare());
	if (ptr != names + array_size && !(val.compare(ptr->name) < 0))
		return static_cast<BlendDataSource>(ptr->val);
	return BDS_DEFAULT;
}

BlendOp Helper::GetColorOperation(const String& val) {
	static EnumNamePair names[] = {
		{ BOP_ADD, _SS(ARG_ADD) },
		{ BOP_INV_SUB, _SS(ARG_INV_SUB) },
		{ BOP_MAX, _SS(ARG_MAX) },
		{ BOP_MIN, _SS(ARG_MIN) },
		{ BOP_SUB, _SS(ARG_SUB) },
	};

	const size_t array_size = sizeof(names) / sizeof(names[0]);

	EnumNamePair* ptr = std::lower_bound(names, names + array_size, val,
		EnumNameCompare());
	if (ptr != names + array_size && !(val.compare(ptr->name) < 0))
		return static_cast<BlendOp>(ptr->val);
	return BOP_ADD;
}

FillMode Helper::GetFillMode(const String& val) {
	if (val == _SS(ARG_SOLID))
		return FM_SOLID;
	if (val == _SS(ARG_WIRE))
		return FM_WIREFRAME;
	if (val == _SS(ARG_POINT))
		return FM_POINT;
	return FM_DEFAULT;
}

CullMode Helper::GetCullMode(const String& val) {
	if (val == _SS(ARG_FRONT))
		return CULL_FRONT;
	if (val == _SS(ARG_BACK))
		return CULL_BACK;
	if (val == _SS(ARG_NONE))
		return CULL_NONE;
	if (val == _SS(ARG_ALL))
		return CULL_ALL;
	return CULL_DEFAULT;
}

TextureComparisonMode Helper::GetTextureCompareFunc(const String& val) {
	switch (GetComparisonFunc(val)) {
	case NFX_COMP_ALWAYS:
		return TEXCOMP_ALWAYS;
	case NFX_COMP_EQUAL:
		return TEXCOMP_EQUAL;
	case NFX_COMP_GREATER:
		return TEXCOMP_GREATER;
	case NFX_COMP_GREATER_EQUAL:
		return TEXCOMP_GREATER_EQUAL;
	case NFX_COMP_LESS:
		return TEXCOMP_LESS;
	case NFX_COMP_LESS_EQUAL:
		return TEXCOMP_LESS_EQUAL;
	case NFX_COMP_NEVER:
		return TEXCOMP_NEVER;
	case NFX_COMP_NOT_EQUAL:
		return TEXCOMP_NOT_EQUAL;
	}
	return TEXCOMP_NONE;
}

FilterType Helper::GetFilterType(const String& val) {
	static EnumNamePair names[] = {
		{ NFX_LINEAR, _SS(ARG_LINEAR) },
		{ NFX_LINEAR_MIPMAP_LINEAR, _SS(ARG_LINEAR_MIP_LINEAR) },
		{ NFX_LINEAR_MIPMAP_NEAREST, _SS(ARG_LINEAR_MIP_NEAREST) },
		{ NFX_NEAREST, _SS(ARG_NEAREST) },
		{ NFX_NEAREST_MIPMAP_LINEAR, _SS(ARG_NEAREST_MIP_LINEAR) },
		{ NFX_NEAREST_MIPMAP_NEAREST, _SS(ARG_NEAREST_MIP_NEAREST) },
	};

	const size_t array_size = sizeof(names) / sizeof(names[0]);

	EnumNamePair* ptr = std::lower_bound(names, names + array_size, val,
		EnumNameCompare());
	if (ptr != names + array_size && !(val.compare(ptr->name) < 0))
		return static_cast<FilterType>(ptr->val);

	return NFX_LINEAR;
}

TextureMinFilter Helper::GetMinFilter(const String& val) {
	switch (GetFilterType(val)) {
	case NFX_LINEAR:
		return TF_MIN_LINEAR;
	case NFX_LINEAR_MIPMAP_LINEAR:
		return TF_MIN_LINEAR_MIPMAP_LINEAR;
	case NFX_LINEAR_MIPMAP_NEAREST:
		return TF_MIN_LINEAR_MIPMAP_NEAREST;
	case NFX_NEAREST:
		return TF_MIN_NEAREST;
	case NFX_NEAREST_MIPMAP_LINEAR:
		return TF_MIN_NEAREST_MIPMAP_LINEAR;
	case NFX_NEAREST_MIPMAP_NEAREST:
		return TF_MIN_NEAREST_MIPMAP_NEAREST;
	}
	return TF_MIN_LINEAR;
}

TextureMagFilter Helper::GetMagFilter(const String& val) {
	switch (GetFilterType(val)) {
	case NFX_LINEAR:
		return TF_MAG_LINEAR;
	case NFX_NEAREST:
		return TF_MAG_NEAREST;
	}
	return TF_MAG_LINEAR;
}

TextureAddressMode Helper::GetTextureAddressMode(const String& val) {
	static EnumNamePair names[] = {
		{ TAM_BORDER, _SS(ARG_BORDER) },
		{ TAM_CLAMP, _SS(ARG_CLAMP) },
		{ TAM_MIRROR, _SS(ARG_MIRROR) },
		{ TAM_WRAP, _SS(ARG_WARP) },
	};

	const size_t array_size = sizeof(names) / sizeof(names[0]);

	EnumNamePair* ptr = std::lower_bound(names, names + array_size, val,
		EnumNameCompare());
	if (ptr != names + array_size && !(val.compare(ptr->name) < 0))
		return static_cast<TextureAddressMode>(ptr->val);
	return TAM_WRAP;
}

uint32 Helper::GetShaderFlag(const String& val) {
	static EnumNamePair names[] = {
		{ RenderQueueFlags::BACKGROUND, _SS(ARG_BACKGROUND) },
		{ RenderQueueFlags::DEBUG, _SS(ARG_DEBUG) },
		{ RenderQueueFlags::DEFERRED, _SS(ARG_BASIC_DEFERRED) },
		{ RenderQueueFlags::DEFERRED_LIGHTING, _SS(ARG_DEFERRED_LIGHTING) },
		{ RenderQueueFlags::OVERLAY, _SS(ARG_OVERLAY) },
		{ RenderQueueFlags::POST_FX, _SS(ARG_POST_FX) },
		{ RenderQueueFlags::TRANSLUCENCY, _SS(ARG_TRANSLUCENT) },
	};

	const size_t array_size = sizeof(names) / sizeof(names[0]);

	EnumNamePair* ptr = std::lower_bound(names, names + array_size, val,
		EnumNameCompare());
	if (ptr != names + array_size && !(val.compare(ptr->name) < 0))
		return static_cast<TextureAddressMode>(ptr->val);
	return 0;
}

AutoParamName Helper::GetAutoParam(const String& val) {
	static EnumNamePair names[] = {
		{ AutoParamName::AUTO_ALBEDO_MAP, _SS(ARG_ALBEDO_MAP) },
		{ AutoParamName::AUTO_CONSTANT_SCALE_FACTOR, _SS(ARG_CONSTANT_SCALE_FACTOR) },
		{ AutoParamName::AUTO_DEPTH_MAP, _SS(ARG_DEPTH_MAP) },
		{ AutoParamName::AUTO_DIFFUSE_COLOR, _SS(ARG_DIFFUSE_COLOR) },
		{ AutoParamName::AUTO_INV_PROJECTION, _SS(ARG_INV_PROJECTION) },
		{ AutoParamName::AUTO_MODEL_VIEW_PROJECTION, _SS(ARG_MODEL_VIEW_PROJECTION) },
		{ AutoParamName::AUTO_NORMAL_MAP, _SS(ARG_NORMAL_MAP) },
		{ AutoParamName::AUTO_OBJECT_TRANSFORM, _SS(ARG_OBJECT_TRANSFORM) },
		{ AutoParamName::AUTO_OMNI_LIGHT_PROPERTIES, _SS(ARG_OMNI_LIGHT_PROPERTIES) },
		{ AutoParamName::AUTO_SPECULAR_AND_GLOSS_MAP, _SS(ARG_SPECULAR_AND_GLOSS_MAP) },
	};

	const size_t array_size = sizeof(names) / sizeof(names[0]);

	EnumNamePair* ptr = std::lower_bound(names, names + array_size, val,
		EnumNameCompare());
	if (ptr != names + array_size && !(val.compare(ptr->name) < 0))
		return static_cast<AutoParamName>(ptr->val);
	return AutoParamName::AUTO_INVALID_PARAM;
}

}
