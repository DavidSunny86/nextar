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

	GpuProgram::GpuProgram(GpuProgram::ProgramType t) :
		ContextObject((ContextObject::Type)t),
		type(t) {
	}

	GpuProgram::~GpuProgram() {
	}

	void GpuProgram::View::Update(RenderContext* rc, uint32 msg, ContextObject::ContextParamPtr ptr) {
		if (msg == MSG_COMPILE) {
			const CompileParam* param = reinterpret_cast<const CompileParam*>(ptr);
			if (param->programSource) {
				Compile(rc, param->programSource,
						param->compileOptions? *(param->compileOptions) : StringUtils::Null);
			}
		}
	}
} /* namespace nextar */
