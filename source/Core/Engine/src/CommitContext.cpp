/*
 * CommitContext.cpp
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <CommitContext.h>

namespace nextar {

CommitContext::CommitContext() :
		frameNumber(0), targetDimension(0, 0), debugDisplay(nullptr), visibiles(
				nullptr), lightSystem(nullptr), renderTarget(nullptr), viewport(
				nullptr), primitive(nullptr), material(nullptr) {
}

CommitContext::~CommitContext() {
	// TODO Auto-generated destructor stub
}

void CommitContext::_Reset() {

	frameNumber = 0;
	targetDimension.combined = 0;
	debugDisplay = nullptr;
	visibiles = nullptr;
	lightSystem = nullptr;
	renderTarget = nullptr;
	viewport = nullptr;
	primitive = nullptr;
	material = nullptr;
}

} /* namespace nextar */
