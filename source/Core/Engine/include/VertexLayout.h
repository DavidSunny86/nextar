/*
 * VertexLayout.h
 *
 *  Created on: 14-Jun-2014
 *      Author: obhi
 */

#ifndef VERTEXLAYOUT_H_
#define VERTEXLAYOUT_H_

#include <NexEngine.h>
#include <VertexElement.h>
#include <ContextObject.h>

namespace nextar {

// common layouts
enum VertexLayoutType : uint16 {
	CUSTOM_LAYOUT = 0xffff,
	POSITION_0 = 0,
	POSITION_NORMAL_0,
	POSITION_NORMAL_UV_0,
	POSITION_0_NORMAL_1,
	POSITION_0_NORMAL_UV_1,
	VERTEX_LAYOUT_COUNT
};

/** @remarks
 * This class represents an input or output layout for vertices.
 */
class _NexEngineAPI VertexLayout: public Referenced<VertexLayout, AllocGeneral>,
	public ContextObject {
public:
	enum Message {
		MSG_CREATE,
	};

	enum Flags {
		// is this layout shared between various vertex buffer bindings?
		SHARED_VERTEX_LAYOUT = 1 << 0,
		// does this layout contain transient buffers
		HAS_TRANSIENT_BUFFERS = 1 << 1,
		// does this layout contain loose buffer binding
		// which means the buffers are switched in and out
		LOOSE_BUFFER_BINDING = 1 << 2,
	};

	struct CreateParam {
		uint32 numElements;
		const VertexElement* elements;
	};

	VertexLayout(uint32 flags = SHARED_VERTEX_LAYOUT);
	virtual ~VertexLayout();

	void Create(uint32 numElements, const VertexElement* elements);

	static void ClearCommonLayouts();
	static const VertexLayoutPtr& GetCommonLayout(VertexLayoutType type);
	static void GetCustomLayoutElements(VertexLayoutType type,
			const VertexElement*& elements, uint32& numElements) {
		elements = commonElementLayout[type].elements;
		numElements = commonElementLayout[type].numElements;
	}

protected:
	uint32 flags;

	struct CommonVertexElement {
		uint32 numElements;
		const VertexElement* elements;
	};

	static CommonVertexElement commonElementLayout[VertexLayoutType::VERTEX_LAYOUT_COUNT];
	static VertexLayoutPtr commonLayouts[VertexLayoutType::VERTEX_LAYOUT_COUNT];

};

} /* namespace nextar */

#endif /* VERTEXLAYOUT_H_ */
