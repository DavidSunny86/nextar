/*
 * UniformBufferGL.h
 *
 *  Created on: 06-Oct-2013
 *      Author: obhi
 */

#ifndef UNIFORMBUFFERGL_H_
#define UNIFORMBUFFERGL_H_

#include "RenderOpenGL.h"

namespace RenderOpenGL {

	struct UniformGL {
		bool	isRowMajMatrix;
		uint8	updateFrequency;
		uint16	type;
		uint16	typeGl;
		uint16 autoName;
		uint16 offset;
	    uint16 matrixStride; // matrix/array
	    uint16 arrayStride;
	    uint16 arrayCount;
		String name;
	};

	typedef vector<UniformGL>::type UniformList;

	class UniformBufferGL {
	public:
		UniformBufferGL();
		virtual ~UniformBufferGL();

		inline uint16 GetParamCount() const {
			return 0;
		}

		inline size_t GetSize() const {
			return size;
		}

	protected:
		uint8 mask;
		uint16 numUnmappedParams;
		uint32 frameStamp;
		GLuint ubName;

		UniformList uniforms;
		size_t size;
		friend class PassGL;
		friend class RenderContextGL;
	};

	typedef vector<UniformBufferGL*>::type UniformBufferList;

} /* namespace RenderOpenGL */
#endif /* UNIFORMBUFFERGL_H_ */
