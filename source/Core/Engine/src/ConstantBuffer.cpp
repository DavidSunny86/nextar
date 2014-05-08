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
	ManagedBuffer(bufferSize, flags)
	,frequency(UpdateFrequency::NEVER)
	,paramDesc(nullptr)
	,paramCount(0)
	,processor(nullptr)
	,name(StringUtils::NullRef) {
	}

	ConstantBuffer::~ConstantBuffer() {
		// TODO Auto-generated destructor stub
	}
	

} /* namespace nextar */
