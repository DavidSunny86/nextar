/*
 * GpuBuffer.cpp
 *
 *  Created on: 16-Nov-2013
 *      Author: obhi
 */

#include <NexEngine.h>
#include <GpuBuffer.h>
#include <RenderContext.h>

namespace nextar {

GpuBuffer::GpuBuffer(GpuBuffer&& other) : ContextObject(other),
		size(other.size) {
	other.size = 0;
}

void GpuBuffer::Read(void* dest, size_t offset, size_t amount) {
	ReadParams p;
	p.bufferPtr = dest;
	p.offset = offset;
	p.size = amount;
	RequestUpdate(MSG_OP_READ,
			reinterpret_cast<ContextObject::ContextParamPtr>(&p));
}

void GpuBuffer::Write(const void* dest, size_t offset, size_t amount) {
	WriteParams p;
	p.bufferPtr = dest;
	p.offset = offset;
	p.size = amount;
	RequestUpdate(MSG_OP_WRITE,
			reinterpret_cast<ContextObject::ContextParamPtr>(&p));
}

void GpuBuffer::View::Update(RenderContext* context, uint32 msg,
		ContextObject::ContextParamPtr param) {
	switch (msg) {
	case MSG_OP_READ: {
		const ReadParams* p = reinterpret_cast<const ReadParams*>(param);
		Read(context, p->bufferPtr, p->offset, p->size);
	}
		break;
	case MSG_OP_WRITE: {
		const WriteParams* p = reinterpret_cast<const WriteParams*>(param);
		Write(context, p->bufferPtr, p->offset, p->size);
	}
		break;
	case MSG_CREATE: {
		const CreateParams* p = reinterpret_cast<const CreateParams*>(param);
		Create(context, p->size, p->elementStride, p->sourceData, p->policy);
	}
		break;
	}
}

} /* namespace nextar */
