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

BufferManager ::BufferManager(size_t _memoryBudget, uint32 _releaseFrameThreshold) :
		memoryBudget(_memoryBudget), memoryInUse(0), releaseFrameThreshold(
				_releaseFrameThreshold) {
}

BufferManager::~BufferManager() {
}


VertexBufferBinding* BufferManager::CreateVertexBufferBinding() {
	return VertexBufferBinding::CreateInstance();
}

VertexLayout* BufferManager::CreateVertexLayout(const VertexElement elements[],
		uint32 numElements,
		bool usesDynamicBinding) {
	VertexLayout* layout = CreateVertexLayoutImpl(usesDynamicBinding);
	if (layout) {
		VertexLayout::CreateParam up;
		up.numElements = numElements;
		up.elements = elements;
		layout->RequestUpdate(VertexLayout::MSG_CREATE,
				reinterpret_cast<ContextObject::ContextParamPtr>(&up));
	}
	return layout;
}

}

