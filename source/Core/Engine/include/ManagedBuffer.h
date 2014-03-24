/*
 * ManagedBuffer.h
 *
 *  Created on: 16-Nov-2013
 *      Author: obhi
 */

#ifndef MANAGEDBUFFER_H_
#define MANAGEDBUFFER_H_

#include <ContextObject.h>
#include <Buffer.h>


namespace nextar {

	class _NexEngineAPI ManagedBuffer:
			public nextar::ContextObject,
			public nextar::Buffer {
	public:

		ManagedBuffer(size_t bufferSize, uint32 flags = Buffer::DEFAULT_FLAGS);
		virtual ~ManagedBuffer();

		virtual void Update(nextar::RenderContext*, UpdateParamPtr);

		/** This function will fail unless a local copy of the data exists. The local
		 * copy is created if the CPU_READ flag is set. **/
		virtual void Read(void *dest, size_t offset = 0, size_t size = 0);
		/** Write to buffer */
		virtual void Write(const void *src, size_t offset = 0, size_t size = 0);
		
		/** Implementation */
		virtual void Read(RenderContext* rc, void *dest, size_t offset = 0, size_t size = 0) override =0;
		virtual void Write(RenderContext* rc, const void *src, size_t offset = 0, size_t size = 0) override =0;
		virtual void CopyFrom(RenderContext* rc, BufferPtr&) override =0;

	protected:

		enum Op {
			OP_READ,
			OP_WRITE,
		};

		struct ReadStruct {
			void* bufferPtr;
			size_t offset;
			size_t size;
		};

		struct WriteStruct {
			const void* bufferPtr;
			size_t offset;
			size_t size;
		};

		struct UpdateParam {

			uint8 operation;
			union {
				ReadStruct readBox;
				WriteStruct writeBox;
			};
		};

	};
} /* namespace nextar */
#endif /* MANAGEDBUFFER_H_ */
