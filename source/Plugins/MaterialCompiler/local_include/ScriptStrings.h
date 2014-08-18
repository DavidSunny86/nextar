/*
 * ScriptStrings.h
 *
 *  Created on: 19-May-2014
 *      Author: obhi
 */

#ifdef SCRIPT_STRING
#	undef SCRIPT_STRING
#endif

#if defined(NEX_MATERIAL_STRINGS_DEFINE)
#define SCRIPT_STRING(str, what)	const char* SCRIPT_##str=what
#else
#define SCRIPT_STRING(str, what)	extern const char* SCRIPT_##str
#endif

#ifndef _SS
#	define _SS(str) SCRIPT_##str
#endif

namespace MaterialCompiler {

SCRIPT_STRING(CMD_MATERIAL, "material");
SCRIPT_STRING(CMD_SHADER, "shader");
SCRIPT_STRING(CMD_LAYER, "layer");
SCRIPT_STRING(CMD_ENABLE_OPTIONS, "enable_options");
SCRIPT_STRING(CMD_PARAM_VALUE, "param_value");
SCRIPT_STRING(ARG_NORMAL, "normal");
SCRIPT_STRING(ARG_HIGHEST, "highest");
SCRIPT_STRING(ARG_LOWEST, "lowest");

}
