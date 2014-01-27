/*
 * CommitContext.h
 *
 *  Created on: 17-Nov-2013
 *      Author: obhi
 */

#ifndef COMMITCONTEXT_H_
#define COMMITCONTEXT_H_

#include "NexSys.h"

namespace nextar {

	class CommitContext : public AllocGeneral {
	public:
		CommitContext();
		virtual ~CommitContext();
			
		void _Reset();


		uint32 			frameNumber;
		Size			targetDimension;

		Pass*				pass;
		ShaderAsset*				shader;
		DebugDisplay*		debugDisplay;
		VisibilitySet*		visibiles;
		LightSystem*		lightSystem;
		RenderTarget*		renderTarget;
		Viewport* 			viewport;
		VisiblePrimitive*	primitive;
		MaterialAsset*			material;
	};

} /* namespace nextar */
#endif /* COMMITCONTEXT_H_ */
