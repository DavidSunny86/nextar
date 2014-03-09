/* 
 * File:   BufferManager.cpp
 * Author: obhi
 * 
 * Created on February 6, 2011, 9:56 PM
 */
#include <NexEngine.h>
#include <BufferManager.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <StreamData.h>

namespace nextar {

	NEX_DEFINE_SINGLETON_PTR(BufferManager);

	BufferManager::BufferManager(size_t _memoryBudget, uint32 _releaseFrameThreshold) : 
		memoryBudget(_memoryBudget), memoryInUse(0),
		releaseFrameThreshold(_releaseFrameThreshold) {
	}

	BufferManager::~BufferManager() {
	}

	size_t BufferManager::MapSignatureToSemantics(
			const VertexSemantic* signature, size_t semanticStride, uint32 numSemantics, 
			const VertexElement* elements, size_t elementsStride, size_t numElements,
			uint16* outElements) {
		size_t numMapped = 0;
		for (size_t i = 0; i < numSemantics; ++i) {
			const VertexSemantic& vs = *(reinterpret_cast<const VertexSemantic*> (((const uint8*)signature) + i*semanticStride));
			size_t lastMap = numMapped;
			for (size_t j = 0; j < numElements; ++j) {
				const VertexElement& element = *(reinterpret_cast<const VertexElement*> (((const uint8*)elements) + j*elementsStride));
				if (element.desc.semantic == vs) {
					outElements[numMapped++] = (uint16)j;
					break;
				}
			}
			if (lastMap == numMapped) {
				// an attribute was not mapped
				return 0;
			}
		}
		return numMapped;
	}

	VertexBufferBinding* BufferManager::CreateVertexBufferBinding() {
		return VertexBufferBinding::CreateInstance();
	}

	VertexLayout* BufferManager::CreateVertexLayout(const VertexElement elements[], uint32 numElements, 
		bool usesDynamicBinding) {
		VertexLayout* layout = CreateVertexLayoutImpl(usesDynamicBinding);
		if (layout) {
			VertexLayout::UpdateParam up;
			up.numElements = numElements;
			up.elements = elements;
			layout->NotifyUpdated(&up);
		}
		return layout;
	}
	
}

