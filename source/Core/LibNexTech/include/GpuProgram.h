/*
 * GpuProgram.h
 *
 *  Created on: 22-Sep-2013
 *      Author: obhi
 */

#ifndef GPUPROGRAM_H_
#define GPUPROGRAM_H_

#include "ContextObject.h"
#include "StringUtils.h"

namespace nextar {

	class GpuProgram :
			public ContextObject,
			public AllocGraphics {
	public:

		struct UpdateParam {
			const char* programSource;
			const StringUtils::WordList* compileOptions;
		};

		enum {
			PROG_CREATED = 1 << 0,
			PROG_COMPILED = 1 << 1,
		};

		enum Type {
			TYPE_VERTEX,
			TYPE_HULL,
			TYPE_DOMAIN,
			TYPE_GEOMETRY,
			TYPE_FRAGMENT,
			TYPE_COMPUTE,
		};

		GpuProgram(Type type);
		virtual ~GpuProgram();

		static GpuProgram* Instance(Type t);

		inline int16 GetType() {
			return type;
		}


		virtual void Create(nextar::RenderContext*);
		virtual void Update(nextar::RenderContext*, ContextObject::UpdateParamPtr);
		virtual void Destroy(nextar::RenderContext*);

	protected:

		virtual bool Compile(nextar::RenderContext* ctx, const char* src, const nextar::StringUtils::WordList& macroList) = 0;
		virtual void Decompile(nextar::RenderContext* ctx) = 0;

		uint16 flags;
		int16 type;
	};

} /* namespace nextar */
#endif /* GPUPROGRAM_H_ */
