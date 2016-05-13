#ifndef COMMONTYPES_H_
#define COMMONTYPES_H_

#include <ProjectModelHeaders.h>
#include <DepthStencilState.h>
#include <BlendState.h>
#include <RasterState.h>
#include <TextureUnitState.h>
#include <ShaderAsset.h>
#include <ShaderTemplate.h>
#include <RenderManager.h>
#include <ScriptStrings.h>

using namespace nextar;
namespace ShaderScript {
/**************************************************************
 * Declrations
 **************************************************************/
struct EnumNamePair {
	int32 val;
	const char* name;
};

struct EnumNameCompare {

	inline bool operator ()(const EnumNamePair& enp, const String& name) const {
		return name.compare(enp.name) > 0;
	}

	inline bool operator ()(const String& name,
			const EnumNamePair & enp) const {
		return name.compare(enp.name) < 0;
	}
};

enum ComparisonFunc {
	NFX_COMP_ALWAYS,
	NFX_COMP_EQUAL,
	NFX_COMP_GREATER,
	NFX_COMP_GREATER_EQUAL,
	NFX_COMP_LESS,
	NFX_COMP_LESS_EQUAL,
	NFX_COMP_NEVER,
	NFX_COMP_NOT_EQUAL,
	NFX_COMP_UNKNOWN
};

enum FilterType {
	NFX_NEAREST,
	NFX_LINEAR,
	NFX_NEAREST_MIPMAP_NEAREST,
	NFX_LINEAR_MIPMAP_NEAREST,
	NFX_NEAREST_MIPMAP_LINEAR,
	NFX_LINEAR_MIPMAP_LINEAR
};

enum CommandDelegateBlock {
	INVALID_TYPE = -1,
	SHADER_BLOCK,
	PASS_BLOCK,
	DEPTH_STENCIL_BLOCK,
	BLEND_STATE_BLOCK,
	RASTER_STATE_BLOCK,
	TEXTURE_STATE_BLOCK,
};

class Helper {
public:
	static bool IsDefault(const String& v) {
		return v.compare(_SS(VAL_DEFAULT)) == 0;
	}

	static bool IsDisabled(const String& v) {
		return v.compare(_SS(VAL_DISABLE)) == 0;
	}

	static ColorMask GetColorMask(const String& val);
	static ComparisonFunc GetComparisonFunc(const String& val);
	static uint32 GetTag(const String& lowerCaseValue);
	static BlendDataSource GetBlendSource(const String& lowerCaseValue);
	static BlendOp GetBlendOperation(const String& lowerCaseValue);
	static DepthStencilCompare GetDepthStencilFunc(const String& lowerCaseValue);
	static StencilOp GetStencilOp(const String& lowerCaseValue);
	static FillMode GetFillMode(const String& lowerCaseValue);
	static CullMode GetCullMode(const String& lowerCaseValue);
	static AutoParamName GetAutoParam(const String& lowerCaseValue);
	static ParameterContext GetContextFromName(const String& lowerCaseValue);
	static TextureAddressMode GetTextureAddressMode(const String& lowerCaseValue);
	static TextureComparisonMode GetTextureCompareFunc(const String& lowerCaseValue);
	static TextureMinFilter GetMinFilter(const String& val);
	static TextureMagFilter GetMagFilter(const String& val);
	static FilterType GetFilterType(const String& val);
};

}

#endif //COMMONTYPES_H_
