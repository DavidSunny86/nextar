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
		ReadParams p;
		p.bufferPtr = dest;
		p.offset = offset;
		p.size = amount;
		RequestUpdate(MSG_OP_READ, reinterpret_cast<ContextObject::ContextParamPtr>(&p));
	}

	void GpuBuffer::Write(const void* dest, size_t offset, size_t amount) {
		WriteParams p;
		p.bufferPtr = dest;
		p.offset = offset;
		p.size = amount;
		RequestUpdate(MSG_OP_WRITE, reinterpret_cast<ContextObject::ContextParamPtr>(&p));
	}

	void GpuBuffer::CopyFrom(GpuBufferPtr& ptr) {
		RequestUpdate(MSG_OP_COPYBUFFER, reinterpret_cast<ContextObject::ContextParamPtr>(ptr.GetPtr()));
	}

	void GpuBuffer::View::Update(RenderContext* context, uint32 msg, ContextObject::ContextParamPtr param) {
		switch(msg) {
		case MSG_OP_READ:
			{
				ReadParams* p = reinterpret_cast<ReadParams*>(param);
				Read(context, p->bufferPtr, p->offset, p->size);
			}
			break;
		case MSG_OP_WRITE:
			{
				WriteParams* p = reinterpret_cast<ReadParams*>(param);
				Write(context, p->bufferPtr, p->offset, p->size);
			}
			break;
		case MSG_OP_COPYBUFFER:
			{
				GpuBuffer::View* view = static_cast<GpuBuffer::View*>(
						context->GetView(reinterpret_cast<GpuBuffer*>(param)));
				CopyFrom(context, view);
			}
			break;
		}
	}

} /* namespace nextar */
