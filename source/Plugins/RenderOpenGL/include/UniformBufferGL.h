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

	struct UniformGL {
		ConstBufferParamDesc constBufferDesc;
		bool	isRowMajMatrix;
		uint16	typeGl;
	    uint16 matrixStride; // matrix/array
	    uint16 arrayStride;
	};

	typedef vector<UniformGL>::type UniformList;

	class UniformBufferGL : public ConstantBuffer {
	public:
		UniformBufferGL();
		virtual ~UniformBufferGL();

		inline uint16 GetParamCount() const {
			return 0;
		}

		inline void SetBinding(GLuint binding) {
			ubBinding = binding;
		}

		inline GLuint GetBinding() const {
			return ubBinding;
		}

	protected:

		GLuint ubBinding;
		GLuint ubName;

		UniformList uniforms;
		size_t size;
		friend class PassGL;
		friend class RenderContextGL;

	};

	typedef std::reference_wrapper<UniformBufferGL> UniformBufferGLRef;
	typedef vector<UniformBufferGL*>::type UniformBufferList;

} /* namespace RenderOpenGL */
#endif /* UNIFORMBUFFERGL_H_ */
