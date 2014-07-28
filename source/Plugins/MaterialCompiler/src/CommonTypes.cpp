/*
 * CommonTypes.cpp
 *
 *  Created on: 26-Jul-2014
 *      Author: obhi
 */
#define NEX_SHADER_STRINGS_DEFINE
#include <BaseHeaders.h>
#include <CommonTypes.h>
#include <ScriptStrings.h>

namespace ShaderCompiler {

CommandDelegate* Helper::FindCommand(CommandNamePair cmdMap[], size_t arraySize,
		const String& name) {
	CommandNamePair* ptr = std::lower_bound(cmdMap, cmdMap + arraySize, name,
			CommandNameCompare());
	if (ptr != cmdMap + arraySize)
		return static_cast<CommandDelegate*>(ptr->command);
	return 0;
}

}

