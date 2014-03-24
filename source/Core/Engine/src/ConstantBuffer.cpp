/*
 * ConstantBuffer.cpp
 *
 *  Created on: 25-Feb-2014
 *      Author: obhi
 */

#include <ConstantBuffer.h>

namespace nextar {

	ConstantBuffer::ConstantBuffer() : frequency(0)
	,paramDesc(nullptr)
	,paramStride(0)
	,paramCount(0)
	,name(StringUtils::NullRef) {
	}

	ConstantBuffer::~ConstantBuffer() {
		// TODO Auto-generated destructor stub
	}

} /* namespace nextar */
