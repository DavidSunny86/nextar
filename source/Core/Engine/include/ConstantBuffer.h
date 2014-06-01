/*
 * ConstantBuffer.h
 *
 *  Created on: 25-Feb-2014
 *      Author: obhi
 */

#ifndef CONSTANTBUFFER_H_
#define CONSTANTBUFFER_H_

#include <ShaderParam.h>
#include <GpuBuffer.h>

namespace nextar {

	class ConstantBuffer : public GpuBuffer {
	public:

		enum {
			CUSTOM_BUFFER = GpuBuffer::LAST_FLAG << 0,
			CUSTOM_STRUCT = GpuBuffer::LAST_FLAG << 1,
			LAST_FLAG = GpuBuffer::LAST_FLAG << 2,
		};

		class View : public GpuBuffer::View {
		public:
			/* Must be called before Write is called */
			virtual void BeginUpdate(RenderContext* rc, UpdateFrequency updateFlags) =0;
			/* Must be called after Write operations are done */
			virtual void EndUpdate(RenderContext* rc) =0;
			/* Write in one go, does not require a begin and end update */
			virtual void Write(RenderContext* rc, const void *src, size_t offset, size_t size) =0;
		};

		ConstantBuffer(size_t bufferSize, uint32 flags = GpuBuffer::DEFAULT_FLAGS);
		virtual ~ConstantBuffer();

		inline void SetName(const String& name) {
			this->name = name;
		}

		inline const String& GetName() const {
			return name;
		}

		inline UpdateFrequency GetFrequency() const {
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

		inline ShaderParamIterator GetParamIterator() {
			return ShaderParamIterator(&paramDesc->paramDesc, paramCount, paramStride);
		}


	protected:
		// If frequency is exclusively PER_MATERIAL or PER_PASS or PER_OBJECT_INSTANCE,
		// and all parameters are custom then a copy of this
		// block can be stored in material and updated accordingly. This processor
		// will update it accordingly
		AutoParamProcessor* processor;

		UpdateFrequency frequency;

		ConstBufferParamDesc* paramDesc;
		uint32 paramCount;
		static uint32 paramStride;

		String name;
	};

} /* namespace nextar */

#endif /* CONSTANTBUFFER_H_ */
