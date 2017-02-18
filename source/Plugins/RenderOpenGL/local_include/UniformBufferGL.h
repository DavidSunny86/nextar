/*
 * UniformBufferGL.h
 *
 *  Created on: 06-Oct-2013
 *      Author: obhi
 */

#ifndef UNIFORMBUFFERGL_H_
#define UNIFORMBUFFERGL_H_

#include <RenderOpenGL.h>

namespace RenderOpenGL {

struct UniformGL: public ConstantParameter {
public:
	bool isRowMajMatrix;
	GLint typeGl;
	uint16 matrixStride; // matrix/array
	uint16 arrayStride;
	String name; // valid only for custom parameters
};

typedef vector<UniformGL>::type UniformList;

class UniformBufferPoolGL;
class UniformBufferGL;


struct UBObject {
	GLuint ubNameGl;
	uint32 allocatedSize;
	uint32 freeSize;
	UniformBufferGL* head;
};

struct UBRef {
	uint16 objectIndex;
	uint16 numOfRef;
	uint32 offset;
	uint32 size;
	GLuint ubNameGl;
	UniformBufferGL* nextInList;

	UBRef(uint16 index, uint32 offsetInBuff, uint32 isize, GLuint name, UniformBufferGL* next = nullptr) : objectIndex(index),
		numOfRef(0), offset(offsetInBuff), size(isize), ubNameGl(name), nextInList(next) {}
};

typedef vector<UBObject>::type UBObjectList;

class UniformBufferPoolGL : public AllocGeneral {
public:
	enum {
		UB_MAX_NAME_LENGTH = 48,
		UB_PREFERRED_BLOCK_SIZE = 1024
	};

	UniformBufferPoolGL();
	~UniformBufferPoolGL();
	UniformBufferGL* Acquire(RenderContext_Base_GL* rc, const char* name, uint32 size);
	void Release(RenderContext_Base_GL* rc, UniformBufferGL*);
	void Destroy(RenderContext_Base_GL* rc) { _DestroyPool(rc); }

	inline void SetAlignmentAndMinSize(GLint algn, GLint minSize) {
		uniformBufferAlignSize = algn;
		uniformBufferMinSize = minSize;
		if (!uniformBufferAlignSize)
			blockSize = UB_PREFERRED_BLOCK_SIZE;
		else
			blockSize = (UB_PREFERRED_BLOCK_SIZE / uniformBufferAlignSize) * uniformBufferAlignSize;
	}

protected:

	static void _ModifyName(const char* name, uint32 size, char* store);
	UniformBufferGL* _Alloc(RenderContext_Base_GL* rc, const char* name, uint32 size, uint32 allocSize);
	UniformBufferGL* _Find(const char* name);
	void _DestroyPool(RenderContext_Base_GL* rc);
	void _CleanAllBlocks();

	GLint uniformBufferAlignSize;
	GLint uniformBufferMinSize;
	UBObjectList ubBlocks;
	uint32 blockSize;
};

class UniformBufferGL: public ParameterGroup {
public:
	UniformBufferGL(const char* name, const UBRef& r);
	virtual ~UniformBufferGL();
	
	virtual void* Map(RenderContext* rc);
	virtual void SetRawBuffer(RenderContext* rc, const ConstantParameter& desc,
			const void* data);
	virtual void Unmap(RenderContext* rc);

	virtual void WriteRawData(RenderContext* rc, const void* data,
			size_t offset = 0, size_t size = 0);

protected:
	char name[UniformBufferPoolGL::UB_MAX_NAME_LENGTH];
	UBRef ref;
	// block size may be different from the ParameterGroup::size,
	// because of padding (even in std140).
	uint32 blockSize;
	//GLuint ubBindingGl;
	//GLuint ubNameGl;

	uint8* mappedMem;
	friend class UniformBufferPoolGL;
	friend class PassViewGL;
	friend class RenderContext_Base_GL;
};

typedef std::reference_wrapper<UniformBufferGL> UniformBufferGLRef;

} /* namespace RenderOpenGL */
#endif /* UNIFORMBUFFERGL_H_ */
