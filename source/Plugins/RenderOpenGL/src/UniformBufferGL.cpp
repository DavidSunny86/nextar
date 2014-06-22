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
		mappedMem(nullptr), ubBindingGl(0), ubNameGl(0) {
	paramStride = sizeof(UniformGL);
}

UniformBufferGL::~UniformBufferGL() {
}

void UniformBufferGL::BeginUpdate(RenderContext* rc, uint32 updateFlags) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	gl->Bind(GL_UNIFORM_BUFFER, ubNameGl);
	// map unmap if necessary
	if (!IsCustomStruct()) {
		mappedMem = static_cast<uint8*>(gl->MapRange(GL_UNIFORM_BUFFER, 0,
				GetSize(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	}
}

void UniformBufferGL::EndUpdate(RenderContext* rc) {
	if (!IsCustomStruct()) {
		RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
		gl->Unmap(GL_UNIFORM_BUFFER);
		mappedMem = nullptr;
	}
}

void UniformBufferGL::Read(RenderContext* rc, void *dest, size_t offset = 0,
		size_t size = 0) {
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
}

void UniformBufferGL::Write(RenderContext* rc, const void *src, size_t offset,
		size_t size) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	if (mappedMem) {
		std::memcpy(mappedMem + offset, src, size);
	} else {
		gl->WriteSubData(GL_UNIFORM_BUFFER, src, offset, size);
	}
}

void UniformBufferGL::Write(RenderContext* rc, const void *src, size_t size) {
	RenderContextGL* gl = static_cast<RenderContextGL*>(rc);
	if (mappedMem) {
		std::memcpy(mappedMem, src, size);
	} else {
		gl->WriteSubData(GL_UNIFORM_BUFFER, src, 0, size);
	}
}

void UniformBufferGL::CopyFrom(RenderContext* rc, BufferPtr&) {
	NEX_THROW_FatalError(EXCEPT_NOT_IMPLEMENTED);
}
} /* namespace RenderOpenGL */
