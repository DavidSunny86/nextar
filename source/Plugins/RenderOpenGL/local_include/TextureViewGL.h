/*
 * TextureViewGL.h
 *
 *  Created on: 25-May-2014
 *      Author: obhi
 */

#ifndef TEXTUREVIEWGL_H_
#define TEXTUREVIEWGL_H_

#include <TextureBase.h>

namespace RenderOpenGL {

	class TextureViewGL: public nextar::TextureBase::View {
		NEX_LOG_HELPER(TextureViewGL);

	public:

		TextureViewGL();
		virtual ~TextureViewGL();

		virtual void Update(RenderContext*, uint32 msg, ContextObject::ContextParamPtr);

		bool IsCreated() const {
			return texture == 0;
		}
	protected:

		GLenum target;
		GLuint texture;
	};

} /* namespace RenderOpenGL */

#endif /* TEXTUREVIEWGL_H_ */
