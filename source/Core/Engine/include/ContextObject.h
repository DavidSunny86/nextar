/*
 * ContextObject.h
 *
 *  Created on: 02-Oct-2013
 *      Author: obhi
 */

#ifndef GPUCONTEXTOBJECT_H_
#define GPUCONTEXTOBJECT_H_

#include <NexBase.h>
#include <NexBase.h>
#include <NexThread.h>

namespace nextar {


	class ContextObject {
	public:
		typedef std::ptrdiff_t UpdateParamPtr;

		ContextObject();
		virtual ~ContextObject();

		virtual void Create(nextar::RenderContext*) {}
		virtual void Update(nextar::RenderContext*, UpdateParamPtr) {}
		virtual void Destroy(nextar::RenderContext*) {}
		/* Called by the class who is responsible for its creation
		 * and destruction */
		virtual void NotifyCreated();
		/* Called by the class who is responsible for its creation
		 * and destruction */
		virtual void NotifyDestroyed();
		/* Called by the class when the object is updated */
		virtual void NotifyUpdated(ContextObject::UpdateParamPtr);
	};

} /* namespace RenderOpenGL */
#endif /* GPUCONTEXTOBJECT_H_ */
