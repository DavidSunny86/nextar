/*
 * CommitContext.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef COMMITCONTEXT_H_
#define COMMITCONTEXT_H_

#include <NexBase.h>
#include <ShaderParam.h>
#include <ParameterBuffer.h>

namespace nextar {

	class CommitContext : public AllocGeneral {
	public:
		enum {
			PASS_PARAM_CONTEXT = 0,
			MATERIAL_PARAM_CONTEXT,
			OBJECT_PARAM_CONTEXT,
			PARAM_CONTEXT_COUNT,
		};

		typedef std::pair<uint32, ParameterBuffer*> ParamContext;

		CommitContext();
		virtual ~CommitContext();
			
		void _Reset();


		uint32 			frameNumber;
		uint32			viewNumber;
		Size			targetDimension;

		ParamContext	shaderParamContext[PARAM_CONTEXT_COUNT];
		
		ConstantBuffer*		cbuffer;

		Pass*				pass;
		ShaderAsset*		shader;
		DebugDisplay*		debugDisplay;
		VisibilitySet*		visibiles;
		LightSystem*		lightSystem;
		RenderTarget*		renderTarget;
		Viewport* 			viewport;
		VisiblePrimitive*	primitive;
		MaterialAsset*		material;

	};

} /* namespace nextar */
#endif /* COMMITCONTEXT_H_ */
