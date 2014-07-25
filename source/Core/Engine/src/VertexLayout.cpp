/*
 * VertexLayout.cpp
 *
 *  Created on: 14-Jun-2014
 *      Author: obhi
 */

#include <VertexLayout.h>
#include <CommonMeshLayouts.h>

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

const VertexLayoutPtr& VertexLayout::GetCommonLayout(VertexLayoutType layoutType) {
	if (!commonLayouts[layoutType]) {
		commonLayouts[layoutType] = Assign( NEX_NEW(VertexLayout()) );
		commonLayouts[layoutType]->Create(
				commonElementLayout[layoutType].numElements,
				commonElementLayout[layoutType].elements);
	}
	return commonLayouts[layoutType];
}

void VertexLayout::ClearCommonLayouts() {
	for(uint32 i = 0; i < VertexLayoutType::VERTEX_LAYOUT_COUNT; ++i)
		commonLayouts[i].Clear();
}

} /* namespace nextar */
