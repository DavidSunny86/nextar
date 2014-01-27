/*
 * GpuProgram.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#include "GpuProgram.h"

namespace nextar {

	GpuProgram::GpuProgram(Type t) :
		type(t), flags(PROG_CREATED) {
	}

	GpuProgram::~GpuProgram() {
	}

	void GpuProgram::Create(RenderContext* rc) {
		// @todo
	}

	void GpuProgram::Update(RenderContext* rc, ContextObject::UpdateParamPtr ptr) {
		UpdateParam* param = static_cast<UpdateParam*>(ptr);
		if ((flags & PROG_COMPILED)) {
			Decompile(rc);
		}

		if (param->programSource) {
			if(Compile(rc, param->programSource,
					param->compileOptions? *(param->compileOptions) : StringUtils::Null))
					flags |= PROG_COMPILED;
		}
	}

	void GpuProgram::Destroy(RenderContext* rc) {
		if ((flags & PROG_COMPILED)) {
			Decompile(rc);
			flags &= ~PROG_COMPILED;
		}
	}

	GpuProgram* GpuProgram::Instance(GpuProgram::Type t) {
		return RenderManager::Instance().CreateProgram(t);
	}
} /* namespace nextar */
