/*
 * GpuBuffer.cpp
 *
 *  Created on: 16-Nov-2013
 *      Author: obhi
 */

#include <NexEngine.h>
#include <GpuBuffer.h>

namespace nextar {


	void GpuBuffer::Read(void* dest, size_t offset, size_t amount) {
		ContextParam p;
		p.operation = OP_READ;
		p.readBox.bufferPtr = dest;
		p.readBox.offset = offset;
		p.readBox.size = amount;
		RequestUpdate(reinterpret_cast<ContextObject::ContextParamPtr>(&p));
	}

	void GpuBuffer::Write(const void* dest, size_t offset, size_t amount) {
		ContextParam p;
		p.operation = OP_READ;
		p.writeBox.bufferPtr = dest;
		p.writeBox.offset = offset;
		p.writeBox.size = amount;
		RequestUpdate(reinterpret_cast<ContextObject::ContextParamPtr>(&p));
	}

	void GpuBuffer::View::Update(RenderContext* context, ContextObject::ContextParamPtr param) {
		ContextParam* p = reinterpret_cast<ContextParam*>(param);
		//@ use a switch for more than 2 operations
		if(p->operation == OP_READ) {
			Read(context, p->readBox.bufferPtr, p->readBox.offset, p->readBox.size);
		} else {
			Write(context, p->writeBox.bufferPtr, p->writeBox.offset, p->writeBox.size);
		}
	}
} /* namespace nextar */
