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
			LAST_FLAG = ManagedBuffer::LAST_FLAG << 1,
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

		ShaderParamIterator GetParamIterator();
		/* Must be called before Write is called */
		virtual void BeginUpdate(RenderContext* rc, uint32 updateFlags) =0;
		/* Must be called after Write operations are done */
		virtual void EndUpdate(RenderContext* rc) =0;
		/** This function will fail unless a local copy of the data exists. The local
		 * copy is created if the CPU_READ flag is set. **/
		virtual void Read(RenderContext* rc, void *dest, size_t offset = 0, size_t size = 0) override =0;
		/** Write to buffer */
		virtual void Write(RenderContext* rc, const void *src, size_t offset = 0, size_t size = 0) override =0;
		/** Copy from another buffer */
		virtual void CopyFrom(RenderContext* rc, BufferPtr&) override =0;

	protected:
		// todo If frequency is exclusively PER_MATERIAL or PER_PASS or PER_OBJECT_INSTANCE,
		// and all parameters are custom then a copy of this
		// block can be stored in material and updated accordingly.
		uint32 frequency;

		ConstBufferParamDesc* paramDesc;
		uint32 paramStride;
		uint32 paramCount;

		StringRef name;
	};

	typedef vector<ConstantBufferPtr>::type ConstantBufferList;
} /* namespace nextar */

#endif /* CONSTANTBUFFER_H_ */
