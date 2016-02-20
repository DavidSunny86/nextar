/*
 * File:   StreamData.h
 * Author: obhi
 *
 * Created on February 7, 2011, 8:39 PM
 */

#ifndef StreamData_H
#define	StreamData_H

#include <RenderConstants.h>
#include <VertexBufferBinding.h>
#include <IndexBuffer.h>
#include <VertexLayout.h>

namespace nextar {

enum PrimitiveType : uint8 {
	/* Point list primitive type.  */
	PT_POINT_LIST,
	/* Line list primitive type.  */
	PT_LINE_LIST,
	/* Triangle list primitive type.  */
	PT_TRI_LIST,
	/* Triangle strip primitive type.  */
	PT_TRI_STRIP,
	/* Triangle fan primitive type.  */
	PT_TRI_FAN
};

class VertexData: public AllocGraphics {
public:
	uint32 start;
	uint32 count;

	VertexLayout* layout;
	VertexBufferBinding* binding;

	VertexData() :
			layout(0), binding(0), start(0), count(0) {

	}
};

class IndexData {
public:
	uint32 start;
	uint32 count;

	IndexBufferPtr indices;

	IndexData() :
			start(0), count(0) {
	}

	IndexData(size_t indexStart, size_t indexCount, IndexBufferPtr indexBuffer) :
		start((uint32)indexStart), count((uint32)indexCount), indices(indexBuffer) {
	}

	void Clear() {
		count = start = 0;
		indices.Clear();
	}
};

/**
 * This class represents stream data for a single pass.
 * A lot of class expecting single pass rendering can
 * simply use an instance of this class rather than
 * the stream pass map (StreamData)
 */
class _NexEngineAPI StreamData: public AllocGeneral {
public:
	enum Flags {
		DELETE_BINDING = 1 << 0,
		DELETE_LAYOUT = 1 << 1,
	};

	uint8 flags;
	/* Primitive type */
	/**/
	PrimitiveType type;
	/* instance count is usually 1 */
	uint16 instanceCount;
	/* Vertex data */
	VertexData vertices;
	/* Indices are not used if invalid */
	IndexData indices;

	StreamData();
	~StreamData();

	void Clear();
};

}
#endif	/* StreamData_H */
