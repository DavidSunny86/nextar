/*
 * ShaderOptions.h
 *
 *  Created on: 04-Mar-2017
 *      Author: obhi
 */

#ifndef CORE_ENGINE_INCLUDE_SHADEROPTIONS_H_
#define CORE_ENGINE_INCLUDE_SHADEROPTIONS_H_

#include <NexBase.h>
#include <StringUtils.h>
#include <EngineConfig.h>

namespace nextar {

class _NexEngineAPI ShaderOptions : public AllocGeneral {
public:

	inline ShaderOptions() {}
	inline ShaderOptions(const StringUtils::WordList& values) { Append(values); }
	inline ShaderOptions(const ShaderOptions& other) : options(other.options) {}
	inline ShaderOptions(ShaderOptions&& other) : options(std::move(other.options)) {}

	void Append(const ShaderOptions& other);
	void Append(const StringUtils::WordList& other);
	void Append(const String& name, const String& value) {
		options[name] = value;
	}

	void Append(const String& name, bool value) {
		Append(name, (value ? "1" : "0"));
	}

	inline StringUtils::WordList ToString() const {
		StringUtils::WordList oLongName;
		ToString(oLongName);
		return oLongName;
	}

	void ToString(StringUtils::WordList& oName) const;
	void FromString(const StringUtils::WordList& words);

	hash_t ComputeHash() const;

protected:
	NameValueMap options;
};

} /* namespace nextar */

#endif /* CORE_ENGINE_INCLUDE_SHADEROPTIONS_H_ */
