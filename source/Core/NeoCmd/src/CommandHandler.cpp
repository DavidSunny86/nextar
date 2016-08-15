/*
 * CommandHandler.cpp
 *
 *  Created on: 12-Feb-2016
 *      Author: obhi
 */

#include <NeoCmd.h>
#include <CommandHandler.h>

namespace nextar {

CommandHandler::CommandHandler(const bool isBlock) : _isBlockHandler(isBlock),
		_parent(nullptr) {
}

CommandHandler::~CommandHandler() {
}

RegionHandler::~RegionHandler() {
}

} /* namespace nextar */
