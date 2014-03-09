/*
 * CommonTypes.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#include "BaseHeaders.h"
#include "CommonTypes.h"

namespace ShaderCompiler {

	/**************************************************************
	 * Helper
	 **************************************************************/
	CommandDelegate* Helper::FindCommand(CommandNamePair cmdMap[], size_t arraySize, const String& name) {
	    CommandNamePair* ptr = std::lower_bound(cmdMap,
	            cmdMap + arraySize,
	            name, CommandNameCompare());
	    if (ptr != cmdMap + arraySize)
	        return static_cast<CommandDelegate*> (ptr->command);
	    return 0;
	}

	ComparisonFunc Helper::GetComparisonFunc(const String& val) {
	    static EnumNamePair names[] = {
	        { NFX_COMP_ALWAYS, "always"},
	        { NFX_COMP_EQUAL, "equal"},
	        { NFX_COMP_GREATER, "greater"},
	        { NFX_COMP_GREATER_EQUAL, "greaterEq"},
	        { NFX_COMP_LESS, "less"},
	        { NFX_COMP_LESS_EQUAL, "lessEq"},
	        { NFX_COMP_NEVER, "never"},
	        { NFX_COMP_NOT_EQUAL, "notEq"},
	    };

	    const size_t arraySize = sizeof (names) / sizeof (names[0]);

	    EnumNamePair* ptr = std::lower_bound(names,
	            names + arraySize,
	            val, EnumNameCompare());
	    if (ptr != names + arraySize)
	        return static_cast<ComparisonFunc> (ptr->val);

	    return NFX_COMP_UNKNOWN;
	}

	StencilOpType Helper::GetStencilOp(const String& val) {
	    static EnumNamePair names[] = {
	        { STENCILOP_DECR, "dec"},
	        { STENCILOP_DECR_SAT, "decSat"},
	        { STENCILOP_INCR, "inc"},
	        { STENCILOP_INCR_SAT, "incSat"},
	        { STENCILOP_INVERT, "invert"},
	        { STENCILOP_KEEP, "keep"},
	        { STENCILOP_REPLACE, "replace"},
	        { STENCILOP_ZERO, "zero"},
	    };

	    const size_t array_size = sizeof (names) / sizeof (names[0]);

	    EnumNamePair* ptr = std::lower_bound(names,
	            names + array_size,
	            val, EnumNameCompare());
	    if (ptr != names + array_size)
	        return static_cast<StencilOpType> (ptr->val);

	    return DSCOMP_DEFAULT;
	}

	DepthStencilCompareType Helper::GetDepthStencilFunc(const String& val) {
	    switch (GetComparisonFunc(val)) {
	        case NFX_COMP_ALWAYS: return DSCOMP_ALWAYS;
	        case NFX_COMP_EQUAL: return DSCOMP_EQUAL;
	        case NFX_COMP_GREATER: return DSCOMP_GREATER;
	        case NFX_COMP_GREATER_EQUAL: return DSCOMP_GREATER_EQUAL;
	        case NFX_COMP_LESS: return DSCOMP_LESS;
	        case NFX_COMP_LESS_EQUAL: return DSCOMP_LESS_EQUAL;
	        case NFX_COMP_NEVER: return DSCOMP_NEVER;
	        case NFX_COMP_NOT_EQUAL: return DSCOMP_NOT_EQUAL;
	    }
	    return DSCOMP_DEFAULT;
	}

	ColorMaskType Helper::GetColorMask(const String& val) {
	    uint8 mask = 0;
	    uint32 trv = std::min((uint32)4, (uint32)val.length());
	    for(size_t i = 0; i < trv; ++i) {
	    	switch(val[i]) {
	    	case 'r': mask |= COLOR_MASK_RED; break;
	    	case 'g': mask |= COLOR_MASK_GREEN; break;
	    	case 'b': mask |= COLOR_MASK_BLUE; break;
	    	case 'a': mask |= COLOR_MASK_ALPHA; break;
	    	}
	    }
	    return mask;
	}

	BlendDataSourceType Helper::GetBlendSource(const String& val) {
	    static EnumNamePair names[] = {
	        { BDS_DST_ALPHA, "dstAlpha"},
	        { BDS_DST_COLOR, "dstColor"},
	        { BDS_INV_DST_ALPHA, "invDstAlpha"},
	        { BDS_INV_DST_COLOR, "invDstColor"},
	        { BDS_INV_ALPHA, "invSrcAlpha"},
	        { BDS_INV_SRC_COLOR, "invSrcColor"},
	        { BDS_ONE, "one"},
	        { BDS_ALPHA, "srcAlpha"},
	        { BDS_SRC_COLOR, "srcColor"},
	        { BDS_ZERO, "zero"},
	    };

	    const size_t array_size = sizeof (names) / sizeof (names[0]);

	    EnumNamePair* ptr = std::lower_bound(names,
	            names + array_size,
	            val, EnumNameCompare());
	    if (ptr != names + array_size)
	        return static_cast<BlendDataSourceType> (ptr->val);
	    return BDS_DEFAULT;
	}

	BlendOpType Helper::GetColorOperation(const String& val) {
	    static EnumNamePair names[] = {
	        { BOP_ADD, "add"},
	        { BOP_INV_SUB, "invSub"},
	        { BOP_MAX, "max"},
	        { BOP_MIN, "min"},
	        { BOP_SUB, "sub"},
	    };

	    const size_t array_size = sizeof (names) / sizeof (names[0]);

	    EnumNamePair* ptr = std::lower_bound(names,
	            names + array_size,
	            val, EnumNameCompare());
	    if (ptr != names + array_size)
	        return static_cast<BlendOpType> (ptr->val);
	    return BDS_DEFAULT;
	}

	FillModeType Helper::GetFillMode(const String& val) {
	    if (val == "solid")
	        return FM_SOLID;
	    if (val == "wire")
	        return FM_WIREFRAME;
	    if (val == "point")
	        return FM_POINT;
	    return FM_DEFAULT;
	}

	FillModeType Helper::GetCullMode(const String& val) {
	    if (val == "front")
	        return CULL_FRONT;
	    if (val == "back")
	        return CULL_BACK;
	    if (val == "none")
	        return CULL_NONE;
	    if (val == "all")
	        return CULL_ALL;
	    return CULL_DEFAULT;
	}


	TextureComparisonModeType Helper::GetTextureCompareFunc(const String& val) {
	    switch (GetComparisonFunc(val)) {
	        case NFX_COMP_ALWAYS: return TEXCOMP_ALWAYS;
	        case NFX_COMP_EQUAL: return TEXCOMP_EQUAL;
	        case NFX_COMP_GREATER: return TEXCOMP_GREATER;
	        case NFX_COMP_GREATER_EQUAL: return TEXCOMP_GREATER_EQUAL;
	        case NFX_COMP_LESS: return TEXCOMP_LESS;
	        case NFX_COMP_LESS_EQUAL: return TEXCOMP_LESS_EQUAL;
	        case NFX_COMP_NEVER: return TEXCOMP_NEVER;
	        case NFX_COMP_NOT_EQUAL: return TEXCOMP_NOT_EQUAL;
	    }
	    return TEXCOMP_NONE;
	}

	FilterType Helper::GetFilterType(const String& val) {
	    static EnumNamePair names[] = {
	        { NFX_LINEAR, "linear"},
	        { NFX_LINEAR_MIPMAP_LINEAR, "linearMipLinear"},
	        { NFX_LINEAR_MIPMAP_NEAREST, "linearMipNearest"},
	        { NFX_NEAREST, "nearest"},
	        { NFX_NEAREST_MIPMAP_LINEAR, "nearestMipLinear"},
	        { NFX_NEAREST_MIPMAP_NEAREST, "nearestMipNearest"},
	    };

	    const size_t array_size = sizeof (names) / sizeof (names[0]);

	    EnumNamePair* ptr = std::lower_bound(names,
	            names + array_size,
	            val, EnumNameCompare());
	    if (ptr != names + array_size)
	        return static_cast<FilterType> (ptr->val);

	    return NFX_LINEAR;
	}

	TextureMinFilterType Helper::GetMinFilter(const String& val) {
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

	TextureMagFilterType Helper::GetMagFilter(const String& val) {
	    switch (GetFilterType(val)) {
	        case NFX_LINEAR:
	            return TF_MAG_LINEAR;
	        case NFX_NEAREST:
	            return TF_MAG_NEAREST;
	    }
	    return TF_MAG_LINEAR;
	}

	TextureAddressModeType Helper::GetTextureAddressMode(const String& val) {
	    static EnumNamePair names[] = {
	        { TAM_BORDER, "border" },
	        { TAM_CLAMP, "clamp" },
	        { TAM_MIRROR, "mirror" },
	        { TAM_WRAP, "warp" },
	    };

	    const size_t array_size = sizeof (names) / sizeof (names[0]);

	    EnumNamePair* ptr = std::lower_bound(names,
	            names + array_size,
	            val, EnumNameCompare());
	    if (ptr != names + array_size)
	        return static_cast<TextureAddressModeType> (ptr->val);

	    return TAM_WRAP;
	}
}

