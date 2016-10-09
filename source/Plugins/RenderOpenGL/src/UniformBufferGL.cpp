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

UniformBufferPoolGL::UniformBufferPoolGL() : blockSize(0) {
}

UniformBufferPoolGL::~UniformBufferPoolGL() {
	_CleanAllBlocks();
}

void UniformBufferPoolGL::_DestroyPool(RenderContext_Base_GL* rc) {

	for (auto& e : ubBlocks) {
		rc->DestroyBuffer(e.ubNameGl);
		e.allocatedSize = 0;
	}
	_CleanAllBlocks();
	ubBlocks.clear();
}

void UniformBufferPoolGL::_CleanAllBlocks() {
	for (auto& e : ubBlocks) {
		UniformBufferGL* buffer = e.head;
		while (buffer) {
			UniformBufferGL* next = buffer->ref.nextInList;
			NEX_DELETE(buffer);
			buffer = next;
		}
		e.head = nullptr;
		e.freeSize = e.allocatedSize;
	}
}

UniformBufferGL* UniformBufferPoolGL::_Find(const char* name) {
	for (auto& e : ubBlocks) {
		UniformBufferGL* buffer = e.head;
		while (buffer) {
			if (!std::strcmp(buffer->name, name))
				return buffer;
			buffer = buffer->ref.nextInList;
		}
	}
	return nullptr;
}

void UniformBufferPoolGL::_ModifyName(const char* name, uint32 size, char* toName) {
	const uint32 l = (uint32)(UB_MAX_NAME_LENGTH - 8);
	std::strncpy(toName, name, l);
	toName[l] = 0;
	char number[16];
	std::sprintf(number, "<%x>", size);
	std::strcat(toName, number);
	return;
}

UniformBufferGL* UniformBufferPoolGL::_Alloc(RenderContext_Base_GL* rc, const char* name,
	uint32 size, uint32 allocSize) {
	ubBlocks.push_back(UBObject());
	UBObject& ubo = ubBlocks.back();
	ubo.allocatedSize = allocSize;
	ubo.ubNameGl = rc->CreateBuffer(allocSize, GL_DYNAMIC_DRAW, GL_UNIFORM_BUFFER);
	ubo.freeSize = allocSize - size;
	ubo.head = NEX_NEW(UniformBufferGL(name, UBRef((uint16)ubBlocks.size()-1, 0, size, ubo.ubNameGl)));
	return ubo.head;
}

UniformBufferGL* UniformBufferPoolGL::Acquire(RenderContext_Base_GL* rc, const char* name, uint32 isize) {
	UniformBufferGL* ret;
	char modName[UniformBufferPoolGL::UB_MAX_NAME_LENGTH];
	_ModifyName(name, isize, modName);
	// find
	isize = std::max((uint32)uniformBufferMinSize, isize);
	uint32 size = Math::RoundUp(isize, (uint32)uniformBufferAlignSize);
	UBObjectList::reverse_iterator it, en;

	if ((ret = _Find(modName)))
		goto add_ref_and_ret;
	// prelim check
	if (size > blockSize) {
		ret = _Alloc(rc, modName, size, size);
		goto add_ref_and_ret;
	}
	// free
	it = ubBlocks.rbegin();
	en = ubBlocks.rend();

	for (; it != en; ++it) {
		UBObject& c = (*it);
		if (c.freeSize >= size) {
			break;
		}
	}

	if (it == en) {
		ret = _Alloc(rc, name, size, blockSize);
		goto add_ref_and_ret;
	} else {
		UBObject& c = (*it);
		UniformBufferGL* head = c.head;
		ret = c.head = NEX_NEW(UniformBufferGL(name, UBRef((uint16)(std::distance(std::begin(ubBlocks), it.base()) - 1), 
			c.allocatedSize - c.freeSize, size, c.ubNameGl, head)));
		c.freeSize -= size;
	}

add_ref_and_ret:
	if (ret)
		ret->ref.numOfRef++;
	return ret;
}

void UniformBufferPoolGL::Release(RenderContext_Base_GL* rc, UniformBufferGL* buff) {

	auto en = ubBlocks.end();
	for (auto it = ubBlocks.begin(); it != en; ++it) {
		auto& e = (*it);
		UniformBufferGL** ref = &e.head;
		UniformBufferGL* buffer = e.head;
		while (buffer) {
			if (buffer == buff) {
				UniformBufferGL* next = *ref = buffer->ref.nextInList;
				uint32 offsetMove = buffer->size;
				e.freeSize += offsetMove;
				while (next) {
					next->ref.offset -= offsetMove;
					next->InvalidateContents();
					next = next->ref.nextInList;
				}
				NEX_DELETE(buffer);
				if (e.freeSize == e.allocatedSize) {
					rc->DestroyBuffer(e.ubNameGl);
					ubBlocks.erase(it);
					return;
				}
			}
			ref = &buffer->ref.nextInList;
			buffer = *ref;
		}
	}
}

UniformBufferGL::UniformBufferGL(const char* name, const UBRef& r) :
mappedMem(nullptr), ref(r) {
	// ubBindingGl = 0;
	std::strcpy(this->name, name);
}

UniformBufferGL::~UniformBufferGL() {
	if (parameter) {
		UniformGL* parameters = static_cast<UniformGL*>(parameter);
		NEX_DELETE_ARRAY(parameters);
	}

}

void* UniformBufferGL::Map(RenderContext* rc) {
	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	gl->SetCurrentUBO(ref.ubNameGl);
	// map unmap if necessary
	return (mappedMem = static_cast<uint8*>(gl->MapRange(GL_UNIFORM_BUFFER, ref.offset,
		size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT)));
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
		gl->SetCurrentUBO(ref.ubNameGl);
		gl->WriteSubData(GL_UNIFORM_BUFFER, src, offset + ref.offset, size);
	}
}

void UniformBufferGL::SetRawBuffer(RenderContext* rc, const ConstantParameter& desc,
		const void* data) {

	RenderContext_Base_GL* gl = static_cast<RenderContext_Base_GL*>(rc);
	if (mappedMem) {
		std::memcpy(mappedMem + desc.bufferOffset, data, desc.size);
	} else {
		gl->SetCurrentUBO(ref.ubNameGl);
		gl->WriteSubData(GL_UNIFORM_BUFFER, data, desc.bufferOffset + ref.offset, desc.size);
	}
}

} /* namespace RenderOpenGL */
