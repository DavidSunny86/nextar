/*
 * ShaderParamAgent.h
 *
 *  Created on: 27-Jul-2014
 *      Author: obhi
 */

#ifndef SHADERPARAMAGENT_H_
#define SHADERPARAMAGENT_H_

#include <NexProjectModel.h>
#include <ShaderParam.h>
#include <ParameterBuffer.h>

namespace nextar {

/*
 *
 */
class _NexProjectAPI ShaderParamAgent {
public:
	typedef map<ParamDataType, ShaderParamAgent*>::type ShaderParamAgentMap;

	static ShaderParamAgent* GetAgent(ParamDataType type);

	virtual void SetParamValue(size_t offset, ParameterBuffer* buffer,
			const ParamEntry& pe, const String& value);

protected:
	virtual void SetParamValueImpl(size_t offset, ParameterBuffer* buffer,
			const ParamEntry& pe, const String& value) = 0;

	static ShaderParamAgentMap agents;
	~ShaderParamAgent() {}
};

} /* namespace MaterialCompiler */

#endif /* SHADERPARAMAGENT_H_ */
