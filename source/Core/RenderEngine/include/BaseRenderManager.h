/*
 * RenderManager.h
 *
 *  Created on: 15-Aug-2013
 *      Author: obhi
 */

#ifndef BASERENDERMANAGER_H_
#define BASERENDERMANAGER_H_

#include <RenderEngine.h>
#include <RenderManager.h>

namespace nextar {

	// Blank implementation for now.
	class _NexRenderAPI BaseRenderManager : public RenderManager {
		NEX_LOG_HELPER(BaseRenderManager);
	public:

		BaseRenderManager();
		virtual ~BaseRenderManager();

		/* */
		virtual void Configure(const Config&);
		
	protected:
	};
} /* namespace nextar */
#endif /* BASERENDERMANAGER_H_ */
