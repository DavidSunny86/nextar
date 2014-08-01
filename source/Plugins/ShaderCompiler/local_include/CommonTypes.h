#ifndef COMMONTYPES_H_
#define COMMONTYPES_H_

#include <ProjectModelHeaders.h>
#include <ScriptParser.h>
#include <DepthStencilState.h>
#include <BlendState.h>
#include <RasterState.h>
#include <TextureUnitState.h>
#include <ShaderAsset.h>
#include <ShaderTemplate.h>
#include <RenderManager.h>

using namespace nextar;
namespace ShaderCompiler {
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

class CommandDelegate {
public:
	enum {
		INVALID_TYPE = -1,
		SHADER_BLOCK,
		PASS_BLOCK,
		DEPTH_STENCIL_BLOCK,
		BLEND_STATE_BLOCK,
		RASTER_STATE_BLOCK,
		TEXTURE_STATE_BLOCK,
	};

	virtual void Execute(int parentType, void* parentParam,
			ScriptParser::StatementContext& statement) = 0;

};

struct CommandNamePair {
	const char* name;
	CommandDelegate* command;
};

struct CommandNameCompare {
	bool operator ()(const CommandNamePair& enp, const String& name) const {
		return name.compare(enp.name) > 0;
	}

	bool operator ()(const String& name, const CommandNamePair & enp) const {
		return name.compare(enp.name) < 0;
	}
};

class Helper {
public:
	static CommandDelegate* FindCommand(CommandNamePair cmdMap[],
			size_t arraySize, const String& name);
	static ComparisonFunc GetComparisonFunc(const String& val);
	static StencilOp GetStencilOp(const String& val);
	static DepthStencilCompare GetDepthStencilFunc(const String& val);
	static ColorMask GetColorMask(const String& val);
	static BlendDataSource GetBlendSource(const String& val);
	static BlendOp GetColorOperation(const String& val);
	static FillMode GetFillMode(const String& val);
	static CullMode GetCullMode(const String& val);
	static FilterType GetFilterType(const String& val);
	static TextureComparisonMode GetTextureCompareFunc(const String& val);
	static TextureMinFilter GetMinFilter(const String& val);
	static TextureMagFilter GetMagFilter(const String& val);
	static TextureAddressMode GetTextureAddressMode(const String& val);
	static uint32 GetShaderFlag(const String& val);
};
}

#endif //COMMONTYPES_H_
