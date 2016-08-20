/*
 * CommandContext.cpp
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */
#include <NeoCmd.h>
#include <CommandContext.h>
#include <BlockCommandHandler.h>

namespace nextar {

CommandContext::CommandContext() : _activeHandler(nullptr), _activeRegionHandler(nullptr), 
_dictionary(nullptr), _document(nullptr) {
	// TODO Auto-generated constructor stub

}

CommandContext::~CommandContext() {
	// TODO Auto-generated destructor stub
}

void CommandContext::SetDictionary(CommandDictionary* dic) {
	_dictionary = dic;
	const RegionHandler* h = dic->GetRegionHandler(StringUtils::Null);
	_activeHandler = h ? h->GetRoot() : nullptr;
}

} /* namespace nextar */
