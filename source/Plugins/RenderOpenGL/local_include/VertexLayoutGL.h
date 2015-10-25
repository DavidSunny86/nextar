/*
 * VertexLayoutGL.h
 *
 *  Created on: 14-Jun-2014
 *      Author: obhi
 */

#ifndef VERTEXLAYOUTGL_H_
#define VERTEXLAYOUTGL_H_

#include <RenderConstants.h>
#include <RenderOpenGL.h>
#include <VertexElementGL.h>

namespace RenderOpenGL {

class PassViewGL;
class GpuBufferViewGL;
/** todo We can have more efficient structures here, but if this works out,
 then no worry */
class VertexLayoutGL: public VertexLayout::View {
public:
	VertexLayoutGL();
	~VertexLayoutGL() {
	}

	virtual void Update(nextar::RenderContext*, uint32 msg,
			ContextObject::ContextParamPtr);
	virtual void Enable(VertexBufferBinding& binding, PassViewGL* pass,
			RenderContext_Base_GL* rc) = 0;
	virtual void Disable(RenderContext_Base_GL* rc) = 0;

	static uint16 GetFromGlType(GLenum);
	static void Populate(VertexAttribGL& a);
protected:
	VertexAttribListGL attributes;
};

class VertexLayoutStaticGL: public VertexLayoutGL {
	uint16 bindingNumber;
	uint16 cachedIndex;
	PassLayoutVertexArrayPairList passVertexArrayPairs;

	NEX_LOG_HELPER(VertexLayoutStaticGL)
	;
public:

	VertexLayoutStaticGL();
	~VertexLayoutStaticGL();

	virtual void Update(nextar::RenderContext*, uint32 msg,
			ContextObject::ContextParamPtr);
	virtual void Destroy(nextar::RenderContext*);

	virtual void Enable(VertexBufferBinding& binding, PassViewGL* pass,
			RenderContext_Base_GL* rc);
	virtual void Disable(RenderContext_Base_GL* rc);

	void ClearAllVAO(RenderContext_Base_GL*);
	GLuint CreateLayout(RenderContext_Base_GL*, VertexBufferBinding& binding, VertexSemanticID semantics);
};

// implement VAO + loose binding
class VertexLayoutFlexibleGL : public VertexLayoutGL {
	uint16 cachedIndex;
	uint16 numSyncRequired;
	PassLayoutVertexArrayPairList passVertexArrayPairs;
	GpuBufferViewGL* transients[(size_t)RenderConstants::MAX_VERTEX_STREAMS];
public:
	VertexLayoutFlexibleGL();
	~VertexLayoutFlexibleGL();

	virtual void Update(nextar::RenderContext*, uint32 msg,
			ContextObject::ContextParamPtr);
	virtual void Destroy(nextar::RenderContext*);

	virtual void Enable(VertexBufferBinding& binding, PassViewGL* pass,
			RenderContext_Base_GL* rc);
	virtual void Disable(RenderContext_Base_GL* rc);

	void ClearAllVAO(RenderContext_Base_GL*);

protected:
	GLuint CreateLayout(RenderContext_Base_GL*, VertexSemanticID semantics);
};

class VertexLayoutDynamicGL: public VertexLayoutGL {
	uint16 numSyncRequired;
	uint16 numberOfStoredPassMapping;
	MappedVertexAttribList* cached;
	UShortArray indices;
	GpuBufferViewGL* transients[(size_t)RenderConstants::MAX_VERTEX_STREAMS];


	NEX_LOG_HELPER(VertexLayoutDynamicGL)
	;
public:

	VertexLayoutDynamicGL();
	~VertexLayoutDynamicGL();

	virtual void Enable(VertexBufferBinding& binding, PassViewGL* pass,
			RenderContext_Base_GL* rc);
	virtual void Disable(RenderContext_Base_GL* rc);

	// const PassLayoutVertexArrayPair& GetVertexLayoutForPass(nextar::Pass* pass, RenderContext_Base_GL* rc);
};

} /* namespace RenderOpenGL */

#endif /* VERTEXLAYOUTGL_H_ */
