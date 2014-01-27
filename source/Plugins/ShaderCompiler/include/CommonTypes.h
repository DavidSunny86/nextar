
#ifndef COMMONTYPES_H_
#define COMMONTYPES_H_

#include "ScriptParser.h"
#include "DepthStencilState.h"
#include "BlendState.h"
#include "RasterState.h"
#include "TextureUnitState.h"
#include "GpuProgram.h"

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

		inline bool operator () (const EnumNamePair& enp, const String& name) const {
			return name.compare(enp.name) >= 0;
		}

		inline bool operator () (const String& name, const EnumNamePair & enp) const {
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

	struct CommandDelegate {
		enum {
			INVALID_TYPE = -1,
			SHADER_BLOCK,
			DEPTH_STENCIL_BLOCK,
			BLEND_STATE_BLOCK,
			RASTER_STATE_BLOCK,
			TEXTURE_STATE_BLOCK,
		};

		virtual void Execute(int parentType,
				void* parentParam,
				ScriptParser::StatementContext& statement) {};
	};

	struct CommandNamePair {
		const char* name;
		CommandDelegate* command;
	};

	struct CommandNameCompare {
		bool operator () (const CommandNamePair& enp, const String& name) const {
			return name.compare(enp.name) >= 0;
		}

		bool operator () (const String& name, const CommandNamePair & enp) const {
			return name.compare(enp.name) < 0;
		}
	};

	class Helper {
	public:
		static CommandDelegate* FindCommand(CommandNamePair cmdMap[], size_t arraySize, const String& name);
		static ComparisonFunc GetComparisonFunc(const String& val);
		static StencilOpType GetStencilOp(const String& val);
		static DepthStencilCompareType GetDepthStencilFunc(const String& val);
		static ColorMaskType GetColorMask(const String& val);
		static BlendDataSourceType GetBlendSource(const String& val);
		static BlendOpType GetColorOperation(const String& val);
		static FillModeType GetFillMode(const String& val);
		static FillModeType GetCullMode(const String& val);
		static FilterType GetFilterType(const String& val);
		static TextureComparisonModeType GetTextureCompareFunc(const String& val);
		static TextureMinFilterType GetMinFilter(const String& val);
		static TextureMagFilterType GetMagFilter(const String& val);
		static TextureAddressModeType GetTextureAddressMode(const String& val);
	};
}

#endif //COMMONTYPES_H_
