/*
 * ManagedBuffer.cpp
 *
 *  Created on: 16-Nov-2013
 *      Author: obhi
 */

#include <ManagedBuffer.h>

namespace nextar {

	ManagedBuffer::ManagedBuffer(size_t bufferSize, uint32 flags) : Buffer(bufferSize, flags) {
		// TODO Auto-generated constructor stub
	}

	ManagedBuffer::~ManagedBuffer() {
		// TODO Auto-generated destructor stub
	}

	void ManagedBuffer::Read(void* dest, size_t offset, size_t amount) {
		UpdateParam p;
		p.operation = OP_READ;
		p.readBox.bufferPtr = dest;
		p.readBox.offset = offset;
		p.readBox.size = amount;
		RequestUpdate(reinterpret_cast<ContextObject::UpdateParamPtr>(&p));
	}

	void ManagedBuffer::Write(const void* dest, size_t offset, size_t amount) {
		UpdateParam p;
		p.operation = OP_READ;
		p.writeBox.bufferPtr = dest;
		p.writeBox.offset = offset;
		p.writeBox.size = amount;
		RequestUpdate(reinterpret_cast<ContextObject::UpdateParamPtr>(&p));
	}

	void ManagedBuffer::Update(RenderContext* context, ContextObject::UpdateParamPtr param) {
		UpdateParam* p = reinterpret_cast<UpdateParam*>(param);
		//@ use a switch for more than 2 operations
		if(p->operation == OP_READ) {
			Read(context, p->readBox.bufferPtr, p->readBox.offset, p->readBox.size);
		} else {
			Write(context, p->writeBox.bufferPtr, p->writeBox.offset, p->writeBox.size);
		}
	}
} /* namespace nextar */
