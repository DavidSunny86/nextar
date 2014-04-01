/*
 * ConstantBuffer.h
 *
 *  Created on: 25-Feb-2014
 *      Author: obhi
 */

#ifndef CONSTANTBUFFER_H_
#define CONSTANTBUFFER_H_

#include <ShaderParam.h>

namespace nextar {

	class ConstantBuffer : public ManagedBuffer {
	public:

		enum {
			CUSTOM_BUFFER = ManagedBuffer::LAST_FLAG << 0,
			CUSTOM_STRUCT = ManagedBuffer::LAST_FLAG << 1,
			LAST_FLAG = ManagedBuffer::LAST_FLAG << 2,
		};

		ConstantBuffer();
		virtual ~ConstantBuffer();

		inline void SetName(const String& name) {
			this->name = name;
		}

		inline const String& GetName() const {
			return name;
		}

		inline uint32 GetFrequency() const {
			return frequency;
		}

		inline bool IsCustomBuffer() const {
			return (flags & CUSTOM_BUFFER) != 0;
		}

		inline bool IsCustomStruct() const {
			return (flags & CUSTOM_STRUCT) != 0;
		}

		inline AutoParamProcessor* GetProcessor() const {
			return processor;
		}

		ShaderParamIterator GetParamIterator();
		/* Must be called before Write is called */
		virtual void BeginUpdate(RenderContext* rc, uint32 updateFlags) =0;
		/* Must be called after Write operations are done */
		virtual void EndUpdate(RenderContext* rc) =0;
		/* Write in one go, does not require a begin and end update */
		virtual void Write(RenderContext* rc, const void *src, size_t size) =0;
		/** This function will fail unless a local copy of the data exists. The local
		 * copy is created if the CPU_READ flag is set. **/
		virtual void Read(RenderContext* rc, void *dest, size_t offset, size_t size) override =0;
		/** Write to buffer */
		virtual void Write(RenderContext* rc, const void *src, size_t offset, size_t size) override =0;
		/** Copy from another buffer */
		virtual void CopyFrom(RenderContext* rc, BufferPtr&) override =0;

	protected:
		// If frequency is exclusively PER_MATERIAL or PER_PASS or PER_OBJECT_INSTANCE,
		// and all parameters are custom then a copy of this
		// block can be stored in material and updated accordingly. This processor
		// will update it accordingly
		AutoParamProcessor* processor;

		uint32 frequency;

		ConstBufferParamDesc* paramDesc;
		uint32 paramCount;
		static uint32 paramStride;

		StringRef name;
	};

} /* namespace nextar */

#endif /* CONSTANTBUFFER_H_ */
