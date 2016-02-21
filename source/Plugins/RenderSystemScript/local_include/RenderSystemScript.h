/*
 * RenderSystemScript.h
 *
 *  Created on: 13-Feb-2016
 *      Author: obhi
 */

#ifndef PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_RENDERSYSTEMSCRIPT_H_
#define PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_RENDERSYSTEMSCRIPT_H_

#include <EngineHeaders.h>
#include <NeoCmdHeaders.h>
#include <DeferredRenderPass.h>
#include <ForwardRenderPass.h>
#include <CompositorRenderPass.h>
#include <DebugRenderPass.h>

#define NEX_SINGLE_INSTANCE(Class) \
	static inline Class& Instance() { \
		static Class _instance;\
		return _instance; \
	} \
	static inline Class* InstancePtr() { \
		return &Instance(); \
	} \


using namespace nextar;
namespace RenderSystemScript {


}


#endif /* PLUGINS_RENDERSYSTEMSCRIPT_LOCAL_INCLUDE_RENDERSYSTEMSCRIPT_H_ */
