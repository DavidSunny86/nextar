/*
 * ShaderParam.cpp
 *
 *  Created on: 06-Oct-2013
 *      Author: obhi
 */

#include "ShaderParam.h"

namespace nextar {

	ShaderVariant::ShaderVariant(const ShaderVariant& v) : type(v.type),
			count (v.count) {
	}

	ShaderVariant::~ShaderVariant() {
	}
} /* namespace nextar */
