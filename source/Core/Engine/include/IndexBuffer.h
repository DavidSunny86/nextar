/* 
* File:   IndexBuffer.h
* Author: obhi
*
* Created on February 6, 2011, 10:23 PM
*/

#ifndef INDEXBUFFER_H
#define	INDEXBUFFER_H

#include <GpuBuffer.h>

namespace nextar {

	class IndexBuffer : public GpuBuffer {
	public:

		enum Type : uint8 {
			TYPE_32BIT,
			TYPE_16BIT
		};
		
		inline IndexBuffer(size_t size, uint32 flags, Type type, RelocationPolicy policy = NEVER_RELEASED) :
		GpuBuffer(TYPE_INDEX_BUFFER, size, flags, policy), indexType(type) {
		}
		virtual ~IndexBuffer();

		Type GetIndexType() const {
			return static_cast<Type>(indexType);
		}

	private:

		Type indexType;
	};
}

#endif	/* INDEXBUFFER_H */


