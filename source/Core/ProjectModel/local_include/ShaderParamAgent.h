/*
 * ShaderParamAgent.h
 *
 *  Created on: 27-Jul-2014
 *      Author: obhi
 */

#ifndef SHADERPARAMAGENT_H_
#define SHADERPARAMAGENT_H_

#include <ShaderParam.h>
#include <ParameterBuffer.h>

namespace nextar {

/*
 *
 */
class ShaderParamAgent {
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

class ShaderParamScalar : public ShaderParamAgent {
public:
	static ShaderParamScalar agent;
	virtual void SetParamValueImpl(size_t offset, ParameterBuffer* buffer,
				const ParamEntry& pe, const String& value);

};

class ShaderParamVector : public ShaderParamAgent {
public:
	static ShaderParamVector agent;
	virtual void SetParamValueImpl(size_t offset, ParameterBuffer* buffer,
				const ParamEntry& pe, const String& value);

};

class ShaderParamMatrix : public ShaderParamAgent {
public:
	static ShaderParamMatrix agent;
	virtual void SetParamValueImpl(size_t offset, ParameterBuffer* buffer,
				const ParamEntry& pe, const String& value);

};

class ShaderParamTexture : public ShaderParamAgent {
public:
	static ShaderParamTexture agent;
	virtual void SetParamValueImpl(size_t offset, ParameterBuffer* buffer,
				const ParamEntry& pe, const String& value);

};

} /* namespace MaterialCompiler */

#endif /* SHADERPARAMAGENT_H_ */
