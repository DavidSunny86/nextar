/* 
* File:   GlVertexElement.h
* Author: obhi
*
* Created on March 7, 2011, 3:28 AM
*/

#ifndef GLVERTEXELEMENT_H
#define	GLVERTEXELEMENT_H

#include <RenderConstants.h>
#include <RenderOpenGL.h>
#include <Pass.h>

namespace RenderOpenGL {

	class PassViewGL;
	struct VertexAttribGL {
		/* assumed true?? */
		GLboolean normalize;
		bool integer;
		int16 elementCount;
		VertexElement element;
		GLenum dttype;
	};

	struct MappedVertexAttrib {
		uint16 attribIndex;
		uint16 index; /* should be enough */
	};

	struct MappedVertexAttribList {
		uint16 passId;
		uint16 numAttributes;
		MappedVertexAttrib attribs[1];
	};

	typedef std::vector<VertexAttribGL> VertexAttribListGL;
	typedef std::pair<uint32, GLuint> PassLayoutVertexArrayPair;
	typedef std::vector<PassLayoutVertexArrayPair> PassLayoutVertexArrayPairList;

	/** todo We can have more efficient structures here, but if this works out,
	 then no worry */
	class VertexLayoutGL : public VertexLayout::View {
	public:
		VertexLayoutGL();
		~VertexLayoutGL() {}

		virtual void Update(nextar::RenderContext*, uint32 msg, ContextObject::ContextParamPtr);
		virtual void Enable(VertexBufferBinding& binding, PassViewGL* pass, RenderContextGL* rc) = 0;
		virtual void Disable(RenderContextGL* rc) = 0;

		static uint16 GetFromGlType(GLenum);
		static void Populate(VertexAttribGL& a);
	protected:
		VertexAttribListGL attributes;
	};

	class VertexLayoutStaticGL : public VertexLayoutGL {
		uint16 bindingNumber;
		uint16 cachedIndex;
		PassLayoutVertexArrayPairList passVertexArrayPairs;
		
		
		NEX_LOG_HELPER(VertexLayoutGL);
	public:

		VertexLayoutStaticGL();
		~VertexLayoutStaticGL();

		virtual void Update(nextar::RenderContext*, uint32 msg, ContextObject::ContextParamPtr);
		virtual void Destroy(nextar::RenderContext*);

		virtual void Enable(VertexBufferBinding& binding, PassViewGL* pass, RenderContextGL* rc);
		virtual void Disable(RenderContextGL* rc);

		void ClearAllVAO(RenderContextGL*);
	};

	class VertexLayoutDynamicGL : public VertexLayoutGL {
		uint16 numberOfStoredPassMapping;
		MappedVertexAttribList* cached;
		UShortArray indices;
		
		NEX_LOG_HELPER(VertexLayoutGL);
	public:

		VertexLayoutDynamicGL();
		~VertexLayoutDynamicGL();

		virtual void Update(nextar::RenderContext*, uint32 msg, ContextObject::ContextParamPtr);
		virtual void Enable(VertexBufferBinding& binding, PassViewGL* pass, RenderContextGL* rc);
		virtual void Disable(RenderContextGL* rc);

		const PassLayoutVertexArrayPair& GetVertexLayoutForPass(nextar::Pass* pass, RenderContextGL* rc);
	};
}
#endif	/* GLVERTEXELEMENT_H */



