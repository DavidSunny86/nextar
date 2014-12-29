/*
 * ConstBufferGLSL.h
 *
 *  Created on: 26-Oct-2014
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERHLSL_H_
#define PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERHLSL_H_

#include <ConstBufferTranslator.h>

namespace ShaderCompiler {

class ConstBufferHLSL :
		public ConstBufferTranslatorImpl {
public:
	ConstBufferHLSL();
	virtual ~ConstBufferHLSL();

	virtual void BeginBuffer(const String& name);
	virtual void AddParam(ParamDataType dataType, const String& name, uint32 arrayCount);
	virtual void EndBuffer(ShaderScript* _script);

protected:
	String transientBuffer;
};

} /* namespace ShaderCompiler */

#endif /* PLUGINS_SHADERCOMPILER_LOCAL_INCLUDE_CONSTBUFFERHLSL_H_ */
