/*
 * ConstantBuffer.cpp
 *
 *  Created on: 25-Feb-2014
 *      Author: obhi
 */

#include <ConstantBuffer.h>

namespace nextar {

	uint32 ConstantBuffer::paramStride(0);
	ConstantBuffer::ConstantBuffer(size_t bufferSize, uint32 flags) : 
	GpuBuffer(ContextObject::TYPE_CONSTANT_BUFFER, bufferSize, flags)
	,frequency(UpdateFrequency::NEVER)
	,paramDesc(nullptr)
	,paramCount(0)
	,processor(nullptr)
	,name(StringUtils::NullRef) {
	}

	ConstantBuffer::~ConstantBuffer() {
	}

} /* namespace nextar */
