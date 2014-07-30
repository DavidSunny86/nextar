/*
 * ScriptStrings.h
 *
 *  Created on: 19-May-2014
 *      Author: obhi
 */

#ifndef MATERIALSCRIPTSTRINGS_H_
#define MATERIALSCRIPTSTRINGS_H_

#if defined(NEX_MATERIAL_STRINGS_DEFINE)
#define SCRIPT_STRING(str, what)	const char* SCRIPT_##str=what
#else
#define SCRIPT_STRING(str, what)	extern const char* SCRIPT_##str
#endif
#define _SS(str) SCRIPT_##str

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

#endif /* MATERIALSCRIPTSTRINGS_H_ */
