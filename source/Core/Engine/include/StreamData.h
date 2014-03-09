/* 
 * File:   StreamData.h
 * Author: obhi
 *
 * Created on February 7, 2011, 8:39 PM
 */

#ifndef StreamData_H
#define	StreamData_H

#include <RenderConstants.h>
#include <VertexBuffer.h>
#include <VertexElement.h>
#include <IndexBuffer.h>

namespace nextar {

	/** @remarks
	 * This class represents an input or output layout for vertices. This specifically
	 * is meant for the shader mapping, so is dependent on the shader chosen for rendering.
	 * This makes sure the layout is modified if the shader is changed.
	 */
	class VertexLayout: public ContextObject,
		public AllocGraphics {
	public:
		struct UpdateParam {
			uint32 numElements;
			const VertexElement* elements;
		};
		/* Dtor */
		virtual ~VertexLayout() {}
		/* Get ith element */
		virtual VertexElement GetVertexElement(uint32 i) = 0;
		/* Element count */
		virtual uint32 GetNumElements() const = 0;
	};

	class _NexEngineExport VertexBufferBinding: public AllocGraphics {
	public:
		VertexBufferBinding();
		~VertexBufferBinding() {}

		inline uint16 GetBindingNumber() const {
			return bindingNumber;
		}

		/** @remarks Returns the count of vertex buffers */
		inline size_t GetBufferCount() {
			return bufferCount;
		}
		/** @remarks Returns the buffer stored at a specified index */
		VertexBufferPtr& GetBuffer(size_t i) {
			NEX_ASSERT(bufferCount > i);
			return bufferList[i];
		}
		/** @remarks Set buffer count, this must be called before binding
		 * Use VertexLayout::GetBufferCount to find the number of seperate
		 * buffers.
		 **/
		void SetBufferCount(size_t num) {
			bufferCount = (uint16)num;
		}

		/** @remarks Binds the buffer to the specified index */
		void BindBuffer(size_t index, VertexBufferPtr vb) {
			bufferList[index] = vb;
			bindingNumber++;
		}

		bool HasOnlyStaticBuffers();

		static VertexBufferBinding* CreateInstance();

	protected:
		typedef array<VertexBufferPtr, (const size_t)RenderConstants::MAX_VERTEX_STREAMS>::type VertexBufferArray;
		uint16 bufferCount;
		uint16 bindingNumber;
		VertexBufferArray bufferList;
	};

	enum PrimitiveTypeTag {
		/* Point list primitive type.  */
		PT_POINT_LIST,
		/* Line list primitive type.  */
		PT_LINE_LIST,
		/* Triangle list primitive type.  */
		PT_TRI_LIST,
		/* Triangle strip primitive type.  */
		PT_TRI_STRIP
	};

	typedef uint8 PrimitiveType;

	struct VertexData: public AllocGraphics {

		size_t start;
		size_t count;

		VertexLayout* layout;
		VertexBufferBinding* binding;

		VertexData() :
			layout(0), binding(0), start(0), count(0) {

		}
	};

	struct IndexData {

		size_t start;
		size_t count;

		IndexBufferPtr indices;

		IndexData() :
				start(0), count(0) {

		}

		IndexData(size_t indexStart, size_t indexCount,
				IndexBufferPtr indexBuffer) :
				start(indexStart), count(indexCount), indices(indexBuffer) {
		}
	};

	/**
	 * This class represents stream data for a single pass.
	 * A lot of class expecting single pass rendering can
	 * simply use an instance of this class rather than
	 * the stream pass map (StreamData)
	 */
	struct _NexEngineExport StreamData: public AllocGeneral {
			/* This is a hint to the renderer for optimization saying
		 * it doesn't use any dynamic vertex buffer */
		bool usesOnlyStaticVb;
		/* Use indices */
		bool useIndices;
		/* Indicates if GraphicsStreamData was created */
		bool isGsdataValid;
		/**/
		PrimitiveType type;
		/* instance count is usually 1 */
		uint16 instanceCount;
		/* Vertex data */
		VertexData vertices;
		/* Indices are not used if invalid */
		IndexData indices;
		/* Graphics data index created by graphics system */
		uint32 gsDataIndex;

		StreamData();
		~StreamData();
	};

}
#endif	/* StreamData_H */

