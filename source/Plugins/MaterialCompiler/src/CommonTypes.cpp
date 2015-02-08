/*
 * CommonTypes.cpp
 *
 *  Created on: 26-Jul-2014
 *      Author: obhi
 */

#include <BaseHeaders.h>
#include <CommonTypes.h>

#define NEX_MATERIAL_STRINGS_DEFINE
#include <ScriptStrings.h>
#undef NEX_MATERIAL_STRINGS_DEFINE

namespace MaterialCompiler {

CommandDelegate_Execute Helper::FindCommand(CommandNamePair cmdMap[], size_t arraySize,
		const String& name) {
	CommandNamePair* ptr = std::lower_bound(cmdMap, cmdMap + arraySize, name,
			CommandNameCompare());
	if (ptr != cmdMap + arraySize && !(name.compare(ptr->name) < 0))
		return static_cast<CommandDelegate_Execute>(ptr->command);
	return 0;
}

}


