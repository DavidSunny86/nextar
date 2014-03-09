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

		inline uint32 GetCustomParamCount() const {
			return customParamCount;
		}

		ShaderParamIterator GetParamIterator();
		/* Must be called before Write is called */
		virtual void BeginWrite(RenderContext* rc) =0;
		/* Must be called after Write operations are done */
		virtual void EndWrite(RenderContext* rc) =0;
		/** This function will fail unless a local copy of the data exists. The local
		 * copy is created if the CPU_READ flag is set. **/
		virtual void Read(RenderContext* rc, void *dest, size_t offset = 0, size_t size = 0) =0;
		/** Write to buffer */
		virtual void Write(RenderContext* rc, const void *src, size_t offset = 0, size_t size = 0) =0;
		/** Copy from another buffer */
		virtual void CopyFrom(RenderContext* rc, BufferPtr&) =0;

	protected:
		// todo If frequency is exclusively PER_MATERIAL, then a copy of this
		// block can be stored in material and updated accordingly.
		uint32 frequency;
		uint32 customParamCount;

		ConstBufferParamDesc* paramDesc;
		uint32 paramStride;
		uint32 paramCount;

		StringRef name;
	};

	typedef vector<ConstantBuffer*>::type ConstantBufferList;
} /* namespace nextar */

#endif /* CONSTANTBUFFER_H_ */
