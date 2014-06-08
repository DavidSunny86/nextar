/*
 * GpuProgram.h
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#ifndef GPUPROGRAM_H_
#define GPUPROGRAM_H_

#include <ContextObject.h>
#include <StringUtils.h>

namespace nextar {

	class GpuProgram :
			public ContextObject,
			public AllocGraphics {
	public:

		enum {
			MSG_COMPILE,
		};

		enum ProgramType : uint16 {
			TYPE_VERTEX = ContextObject::TYPE_GPU_VERTEX_PROGRAM,
			TYPE_HULL = ContextObject::TYPE_GPU_HULL_PROGRAM,
			TYPE_DOMAIN = ContextObject::TYPE_GPU_DOMAIN_PROGRAM,
			TYPE_GEOMETRY = ContextObject::TYPE_GPU_GEOMETRY_PROGRAM,
			TYPE_FRAGMENT = ContextObject::TYPE_GPU_FRAGMENT_PROGRAM,
			TYPE_COMPUTE = ContextObject::TYPE_GPU_COMPUTE_PROGRAM,
		};

		struct CompileParam {
			const char* programSource;
			const StringUtils::WordList* compileOptions;
		};

		class View : public ContextObject::View {
		public:

			virtual void Update(nextar::RenderContext*, uint32 msg, ContextObject::ContextParamPtr);
			virtual bool Compile(nextar::RenderContext* ctx, const char* src, const nextar::StringUtils::WordList& macroList) = 0;
		};

		GpuProgram(ProgramType type);
		virtual ~GpuProgram();

		inline static GpuProgram* Instance(ProgramType t) {
			return NEX_NEW(GpuProgram(t));
		}

		inline ProgramType GetProgramType() {
			return type;
		}

	protected:

		ProgramType type;
	};

} /* namespace nextar */
#endif /* GPUPROGRAM_H_ */
