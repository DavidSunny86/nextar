/*
 * MaterialScript.h
 *
 *  Created on: 03-Mar-2016
 *      Author: obhi
 */

#ifndef PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_MATERIALSCRIPT_H_
#define PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_MATERIALSCRIPT_H_


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

namespace MaterialScript {



}



#endif /* PLUGINS_MATERIALSCRIPT_LOCAL_INCLUDE_MATERIALSCRIPT_H_ */
