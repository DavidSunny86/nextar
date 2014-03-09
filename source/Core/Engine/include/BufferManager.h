/* 
 * File:   BufferManager.h
 * Author: obhi
 *
 * Created on February 6, 2011, 9:56 PM
 */

#ifndef BUFFERMANAGER_H
#define	BUFFERMANAGER_H

#include "NexSys.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexElement.h"

namespace nextar {

	/**
	 * @remarks
	 * A buffer is marked as being used in the current frame if:
	 * 1. User calls Read/Write. <br/>
	 * 2. The buffer is bound to the context for rendering. <br/>
	 **/
	class _NexEngineExport BufferManager: public Singleton<BufferManager>,
		public AllocGraphics {

	public:
		BufferManager(size_t memoryBudget, uint32 releaseFrameThreshold);
		virtual ~BufferManager();

		/** Create a vertex binding */
		virtual VertexBufferBinding* CreateVertexBufferBinding();

		/* Create a vertex layout, set useDynamicBuffers to true if buffers have dynamic release policy	*/
		virtual VertexLayout* CreateVertexLayout(const VertexElement elements[], uint32 numElements, bool usesDynamicBinding = false);

		/**
		 * @remarks Create a vertex buffer. 
		 * @param bufferSize The size of the buffer in bytes
		 * @param accessFlags Buffer access flags
		 * @param policy Determines when the buffer is lost. This only applies
		 * for dynamic buffers which needs CPU_WRITE or GPU_WRITE access.
		 **/
		virtual VertexBufferPtr CreateVertexBuffer(size_t bufferSize,
				uint32 accessFlags, RelocationPolicy policy) = 0;

		/**
		 * @remarks Create a index buffer. GPU Read only buffers can
		 *          only be mapped using MAP_INIT
		 * @param bufferSize The size of the buffer in bytes
		 * @param accessFlags Buffer access flags
		 * @param type Index Buffer type.
		 **/
		virtual IndexBufferPtr CreateIndexBuffer(size_t bufferSize,
				uint32 accessFlags, IndexBuffer::Type type =
						IndexBuffer::TYPE_16BIT) = 0;

		/* Set threshold at which buffers are released */
		void SetReleaseFrameThreshold(uint16 releaseFrameThreshold) {
			this->releaseFrameThreshold = releaseFrameThreshold;
		}

		/* Get threshold at which buffers are released */
		uint16 GetReleaseFrameThreshold() const {
			return releaseFrameThreshold;
		}
			
		
		/**
		 * Called internally to verify if this signature
		 * can accept the vertex elements. Outputs the mapped vertex
		 * elements. If any attribute/vertex semantic fails to map the
		 * operation returns faliure.
		 * @returns 0 if the verification fails, else the mapped count
		 */
		static size_t MapSignatureToSemantics(const VertexSemantic* signature,
				size_t semanticStride, uint32 numSemantics, 
				const VertexElement* elements, size_t elementsStride,
				size_t numElements, uint16* outElements);

	protected:

		virtual VertexLayout* CreateVertexLayoutImpl(bool usesDynamicBinding = false) = 0;

		uint16 releaseFrameThreshold;

		size_t memoryBudget;
		size_t memoryInUse;
	};

}
#endif	/* BUFFERMANAGER_H */

