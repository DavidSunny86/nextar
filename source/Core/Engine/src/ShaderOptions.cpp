/*
 * ShaderOptions.cpp
 *
 *  Created on: 04-Mar-2017
 *      Author: obhi
 */

#include <ShaderOptions.h>

namespace nextar {

void ShaderOptions::Append(const ShaderOptions& other) {
	for (auto& e : other.options) {
		options[e.first] = e.second;
	}
}

void ShaderOptions::Append(const StringUtils::WordList& words) {
	auto it = ConstMultiStringHelper::It(words);
	String nameValue;
	StringPair nameValPair;
	while (it.HasNext(nameValue)) {
		StringUtils::Split(nameValPair, nameValue);
		options[nameValPair.first] = nameValPair.second;
	}
}

void ShaderOptions::FromString(const StringUtils::WordList& words) {
	options.clear();
	Append(words);
}

void ShaderOptions::ToString(StringUtils::WordList& oLongName) const {
	oLongName = "";
	for (auto& e : options) {
		StringUtils::PushBackWord(oLongName, e.first + ":" + e.second);
	}
}

hash_t ShaderOptions::ComputeHash() const {
	// of already in list dont add
	return StringUtils::Hash(ToString());
}

} /* namespace nextar */
