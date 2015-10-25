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
	GLboolean normalize;bool integer;
	int16 elementCount;
	VertexElement element;
	GLenum dttype;
};

struct MappedVertexAttrib {
	uint16 attribIndex;
	uint16 index; /* should be enough */
};

struct MappedVertexAttribList {
	enum {
		HEADER_SIZE = (sizeof(uint16) * 2 + sizeof(GLuint)) / 2
	};
	uint16 passId;
	uint16 numAttributes;
	GLuint layout;
	MappedVertexAttrib attribs[1];
};

struct VertexSemanticGL {
	VertexSemantic semantic;
	GLuint index;

	friend bool operator ==(const VertexSemanticGL& v1,
			const VertexSemanticGL& v2) {
		// types may vary depending upon normalization is used
		return (v1.semantic == v2.semantic && v1.index == v2.index);
	}
};

/** Stored in passes, streams can only be bound
 * to matching vertex signatures.
 */
struct VertexSemanticDataGL {
	uint16 count;
	const VertexSemanticGL* rawArray;
};

struct VertexSemanticID {
	uint32 indexIntoRegisteredList;
	VertexSemanticID() : indexIntoRegisteredList(-1) {}
	VertexSemanticID(uint32 i) : indexIntoRegisteredList(i) {}
};

struct VertexSemanticListElement {
	uint16 indexIntoBlobList;
	uint16 count;
};

typedef vector<VertexSemanticListElement>::type VertexSemanticListElementList;
typedef vector<VertexSemanticGL>::type VertexSemanticListGL;
typedef vector<VertexAttribGL>::type VertexAttribListGL;
typedef std::pair<uint32, GLuint> PassLayoutVertexArrayPair;
typedef vector<PassLayoutVertexArrayPair>::type PassLayoutVertexArrayPairList;
}
#endif	/* GLVERTEXELEMENT_H */

