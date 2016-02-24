/*
 * CommandContext.cpp
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */
#include <NeoCmd.h>
#include <CommandContext.h>
#include <RootBlockCommandHandler.h>

namespace nextar {

CommandContext::CommandContext() {
	// TODO Auto-generated constructor stub

}

CommandContext::~CommandContext() {
	// TODO Auto-generated destructor stub
}

void CommandContext::SetDictionary(CommandDictionary* dic) {
	_dictionary = dic;
	_activeHandler = dic->GetRoot();
}

} /* namespace nextar */
