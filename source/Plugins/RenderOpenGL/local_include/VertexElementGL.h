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
	uint16 passId;
	uint16 numAttributes;
	MappedVertexAttrib attribs[1];
};

typedef std::vector<VertexAttribGL> VertexAttribListGL;
typedef std::pair<uint32, GLuint> PassLayoutVertexArrayPair;
typedef std::vector<PassLayoutVertexArrayPair> PassLayoutVertexArrayPairList;
}
#endif	/* GLVERTEXELEMENT_H */

