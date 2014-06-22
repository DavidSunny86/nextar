/*
 * VertexLayout.cpp
 *
 *  Created on: 14-Jun-2014
 *      Author: obhi
 */

#include <VertexLayout.h>

namespace nextar {

VertexLayout::VertexLayout(uint32 _flags) : flags(_flags),
		ContextObject(TYPE_VERTEX_LAYOUT, _flags) {
}

VertexLayout::~VertexLayout() {
}

void VertexLayout::Create(uint32 numElements, const VertexElement* elements) {
	CreateParam params;
	params.elements = elements;
	params.numElements = numElements;
	RequestUpdate(MSG_CREATE, reinterpret_cast<ContextParamPtr>(&params));
}

} /* namespace nextar */
