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

	struct UniformGL : public ConstantParameter {
	public:
		bool	isRowMajMatrix;
		uint16	typeGl;
	    uint16 matrixStride; // matrix/array
	    uint16 arrayStride;
	};

	typedef vector<UniformGL>::type UniformList;

	class UniformBufferGL : public ParameterGroup {
	public:
		UniformBufferGL();
		virtual ~UniformBufferGL();


		inline void SetBinding(GLuint binding) {
			ubBindingGl = binding;
		}

		inline GLuint GetBinding() const {
			return ubBindingGl;
		}

		virtual void Map(RenderContext* rc);
		virtual void SetRawBuffer(RenderContext* rc, const ConstantParameter& desc, const void* data);
		virtual void Unmap(RenderContext* rc);

		virtual void WriteRawData(RenderContext* rc, const void* data, size_t offset = 0, size_t size = 0);

	protected:

		GLuint ubBindingGl;
		GLuint ubNameGl;

		uint8* mappedMem;
		friend class PassViewGL;
		friend class RenderContextGL;
	};

	typedef std::reference_wrapper<UniformBufferGL> UniformBufferGLRef;

} /* namespace RenderOpenGL */
#endif /* UNIFORMBUFFERGL_H_ */
