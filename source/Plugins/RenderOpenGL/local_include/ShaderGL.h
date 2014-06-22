/*
 * GLProgram.h
 *
 *  Created on: 02-Oct-2013
 *      Author: obhi
 */

#ifndef CTXGLPROGRAM_H_
#define CTXGLPROGRAM_H_

#include <Shader.h>

namespace RenderOpenGL {

class ShaderGL: public Shader {
	NEX_LOG_HELPER (GpuShaderGL);

public:
	ShaderGL(nextar::AssetManager* m, const String&);
	virtual ~ShaderGL();

protected:
	virtual bool Compile(RenderContext*);
	virtual void Decompile(nextar::RenderContext*);

};

} /* namespace RenderOpenGL */
#endif /* CTXGLPROGRAM_H_ */
