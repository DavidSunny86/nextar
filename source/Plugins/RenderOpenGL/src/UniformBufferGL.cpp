/*
 * UniformBufferGL.cpp
 *
 *  Created on: 06-Oct-2013
 *      Author: obhi
 */
#include <RenderOpenGL.h>
#include <RenderContext_Base_GL.h>
#include <UniformBufferGL.h>

namespace RenderOpenGL {

UniformBufferGL::UniformBufferGL() :
		mappedMem(nullptr), ubNameGl(0) {
	// ubBindingGl = 0;
}

UniformBufferGL::~UniformBufferGL() {
	if (parameter) {
		UniformGL* parameters = static_cast<UniformGL*>(parameter);
		NEX_DELETE_ARRAY(parameters);
	}

}

void UniformBufferGL::Destroy(RenderContext_Base_GL* rc) {
	rc->DestroyBuffer(ubNameGl);
}

void* UniformBufferGL::Map(RenderContext* rc) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	gl->Bind(GL_UNIFORM_BUFFER, ubNameGl);
	// map unmap if necessary
	return (mappedMem = static_cast<uint8*>(gl->MapRange(GL_UNIFORM_BUFFER, 0,
				size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT)));
}

void UniformBufferGL::Unmap(RenderContext* rc) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	gl->Unmap(GL_UNIFORM_BUFFER);
	mappedMem = nullptr;
}

void UniformBufferGL::WriteRawData(RenderContext* rc, const void *src, size_t offset,
		size_t size) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	if (mappedMem) {
		std::memcpy(mappedMem + offset, src, size);
	} else {
		gl->WriteSubData(GL_UNIFORM_BUFFER, src, offset, size);
	}
}

void UniformBufferGL::SetRawBuffer(RenderContext* rc, const ConstantParameter& desc,
		const void* data) {

	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	if (mappedMem) {
		std::memcpy(mappedMem + desc.bufferOffset, data, desc.size);
	} else {
		gl->WriteSubData(GL_UNIFORM_BUFFER, data, desc.bufferOffset, desc.size);
	}
}

} /* namespace RenderOpenGL */
