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
			ubBindingGl = binding;
		}

		inline GLuint GetBinding() const {
			return ubBindingGl;
		}

		/* Must be called before Write is called */
		virtual void BeginUpdate(RenderContext* rc, UpdateFrequency updateFlags) override;
		/* Must be called after Write operations are done */
		virtual void EndUpdate(RenderContext* rc) override;
		/** This function will fail unless a local copy of the data exists. The local
		 * copy is created if the CPU_READ flag is set. **/
		virtual void Read(RenderContext* rc, void *dest, size_t offset = 0, size_t size = 0) override;
		/** Write to buffer */
		virtual void Write(RenderContext* rc, const void *src, size_t offset, size_t size) override;
		/* Write in one go, does not require a begin and end update */
		virtual void Write(RenderContext* rc, const void *src, size_t size) override;
		/** Copy from another buffer */
		virtual void CopyFrom(RenderContext* rc, BufferPtr&) override;

	protected:

		GLuint ubBindingGl;
		GLuint ubNameGl;

		uint8* mappedMem;
		friend class PassGL;
		friend class RenderContextGL;
	};

	typedef std::reference_wrapper<UniformBufferGL> UniformBufferGLRef;

} /* namespace RenderOpenGL */
#endif /* UNIFORMBUFFERGL_H_ */
