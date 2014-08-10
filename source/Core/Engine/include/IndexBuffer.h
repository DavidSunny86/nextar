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

class _NexEngineAPI IndexBuffer: public GpuBuffer {
public:

	enum Type
		: uint8 {
			TYPE_32BIT, TYPE_16BIT
	};

	IndexBuffer() : indexType(TYPE_16BIT), policy(RelocationPolicy::INVALID_POLICY) {}
	IndexBuffer(IndexBuffer&& other);
	IndexBuffer(RelocationPolicy _policy) :
			GpuBuffer(TYPE_INDEX_BUFFER, _policy), indexType(TYPE_16BIT), policy(_policy) {
	}

	Type GetIndexType() const {
		return indexType;
	}

	inline IndexBuffer& operator = (IndexBuffer&& other) {
		GpuBuffer::operator =(std::move(other));
		indexType = other.indexType;
		policy = other.policy;
		return *this;
	}

	virtual void CreateBuffer(size_t bufferSize, Type type, const uint8* dataPtr);

private:

	RelocationPolicy policy;
	Type indexType;
};

}

#endif	/* INDEXBUFFER_H */

