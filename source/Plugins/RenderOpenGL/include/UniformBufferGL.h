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
	    uint16 sizeInBytes;
		String name;

		friend bool operator < (const UniformGL& first, const UniformGL& second) {
			if (first.autoName == second.autoName) {
				NEX_ASSERT(first.autoName == AutoParamName::AUTO_CUSTOM);
				return first.name < second.name;
			}
			return (first.autoName < second.autoName) != 0;
		}
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

		inline void SetName(const String& name) {
			this->name = name;
		}

		inline const String& GetName() const {
			return name;
		}

		inline void SetBinding(GLuint binding) {
			ubBinding = binding;
		}

		inline GLuint GetBinding() const {
			return ubBinding;
		}

		inline uint8 GetUpdateFrequency() const {
			return updateFrequency;
		}

		friend bool operator < (UniformBufferGL*)
	protected:

		StringRef name;

		uint8 updateFrequency;
		uint16 numUnmappedParams;

		uint32 frameStamp;

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
