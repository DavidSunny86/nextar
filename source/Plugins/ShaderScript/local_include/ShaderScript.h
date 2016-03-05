/*
 * ShaderScript.h
 *
 *  Created on: 05-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_SHADERSCRIPT_LOCAL_INCLUDE_SHADERSCRIPT_H_
#define PLUGINS_SHADERSCRIPT_LOCAL_INCLUDE_SHADERSCRIPT_H_



#include <ProjectModelHeaders.h>
#include <DepthStencilState.h>
#include <BlendState.h>
#include <RasterState.h>
#include <TextureUnitState.h>
#include <ShaderAsset.h>
#include <MaterialTemplate.h>
#include <NeoCmdHeaders.h>

#define NEX_SINGLE_INSTANCE(Class) \
	static inline Class& Instance() { \
		static Class _instance;\
		return _instance; \
	} \
	static inline Class* InstancePtr() { \
		return &Instance(); \
	} \

using namespace nextar;

namespace ShaderScript {

}


#endif /* PLUGINS_SHADERSCRIPT_LOCAL_INCLUDE_SHADERSCRIPT_H_ */
