/*
 * UniformBufferGL.cpp
 *
 *  Created on: 06-Oct-2013
 *      Author: obhi
 */
#include <RenderOpenGL.h>
#include <RenderContextGL.h>
#include <UniformBufferGL.h>

namespace RenderOpenGL {

UniformBufferGL::UniformBufferGL() :
		mappedMem(nullptr), ubNameGl(0) {
	// ubBindingGl = 0;
}

UniformBufferGL::~UniformBufferGL() {
}

void UniformBufferGL::Destroy(RenderContextGL* rc) {
	rc->DestroyBuffer(ubNameGl);
}

void* UniformBufferGL::Map(RenderContext* rc) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	gl->Bind(GL_UNIFORM_BUFFER, ubNameGl);
	// map unmap if necessary
	return (mappedMem = static_cast<uint8*>(gl->MapRange(GL_UNIFORM_BUFFER, 0,
				size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT)));
}

void UniformBufferGL::Unmap(RenderContext* rc) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	gl->Unmap(GL_UNIFORM_BUFFER);
	mappedMem = nullptr;
}

void UniformBufferGL::WriteRawData(RenderContext* rc, const void *src, size_t offset,
		size_t size) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	if (mappedMem) {
		std::memcpy(mappedMem + offset, src, size);
	} else {
		gl->WriteSubData(GL_UNIFORM_BUFFER, src, offset, size);
	}
}

void UniformBufferGL::SetRawBuffer(RenderContext* rc, const ConstantParameter& desc,
		const void* data) {

	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	if (mappedMem) {
		std::memcpy(mappedMem + desc.bufferOffset, data, desc.size);
	} else {
		gl->WriteSubData(GL_UNIFORM_BUFFER, data, desc.bufferOffset, desc.size);
	}
}

} /* namespace RenderOpenGL */
