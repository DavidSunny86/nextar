/*
 * NeoCommandInterpreter.h
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

#ifndef CORE_NEOCMD_INCLUDE_NEOCOMMANDINTERPRETER_H_
#define CORE_NEOCMD_INCLUDE_NEOCOMMANDINTERPRETER_H_

#include <NeoCmd.h>

namespace nextar {

class _NexNeoCmdAPI NeoCommandInterpreter {
	NEX_LOG_HELPER(NeoCommandInterpreter);

public:

	static void Execute(const String& dictionaryName, InputStreamPtr& stream, CommandContext* context);

protected:

};

} /* namespace nextar */

#endif /* CORE_NEOCMD_INCLUDE_NEOCOMMANDINTERPRETER_H_ */
