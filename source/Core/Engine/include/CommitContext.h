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

namespace nextar {

	class CommitContext : public AllocGeneral {
	public:
		CommitContext();
		virtual ~CommitContext();
			
		void _Reset();


		uint32 			frameNumber;
		uint32			viewNumber;
		Size			targetDimension;

		uint32 passParamOffset;
		uint32 materialParamOffset;
		uint32 objectParamOffset;

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

		ParameterBuffer*		passParameters;
		ParameterBuffer*		materialParameters;
		ParameterBuffer*		objectParameters;
	};

} /* namespace nextar */
#endif /* COMMITCONTEXT_H_ */
