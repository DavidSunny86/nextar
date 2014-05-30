/*
 * GpuProgram.cpp
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */
#include <NexEngine.h>
#include <RenderManager.h>
#include <GpuProgram.h>

namespace nextar {

	GpuProgram::GpuProgram(Type t) :
		type(t), flags(PROG_CREATED) {
	}

	GpuProgram::~GpuProgram() {
	}

	void GpuProgram::View::Create(RenderContext* rc) {
		// todo
	}

	void GpuProgram::View::Update(RenderContext* rc, ContextObject::ContextParamPtr ptr) {
		ContextParam* param = reinterpret_cast<ContextParam*>(ptr);
		if ((flags & PROG_COMPILED)) {
			Decompile(rc);
		}

		if (param->programSource) {
			if(Compile(rc, param->programSource,
					param->compileOptions? *(param->compileOptions) : StringUtils::Null))
					flags |= PROG_COMPILED;
		}
	}

	void GpuProgram::View::Destroy(RenderContext* rc) {
		if ((flags & PROG_COMPILED)) {
			Decompile(rc);
			flags &= ~PROG_COMPILED;
		}
	}

	GpuProgram* GpuProgram::Instance(GpuProgram::Type t) {
		return RenderManager::Instance().CreateProgram(t);
	}
} /* namespace nextar */
