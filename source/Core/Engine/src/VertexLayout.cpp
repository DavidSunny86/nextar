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
	NEX_ASSERT(layoutType >= 0 && layoutType < VertexLayoutType::VERTEX_LAYOUT_COUNT);
	CommonVertexLayoutData& data = commonLayouts[layoutType];
	if (!data.layout) {
		data.layout = Assign(NEX_NEW(VertexLayout(data.defaultFlags)));
		data.layout->Create(
			data.numElements,
			data.elements);
	}
	return data.layout;
}

void VertexLayout::ClearCommonLayouts() {
	for(uint32 i = 0; i < VertexLayoutType::VERTEX_LAYOUT_COUNT; ++i)
		commonLayouts[i].layout.Clear();
}

} /* namespace nextar */
