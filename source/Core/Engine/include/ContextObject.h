/*
 * ContextObject.h
 *
 *  Created on: 02-Oct-2013
 *      Author: obhi
 */

#ifndef GPUCONTEXTOBJECT_H_
#define GPUCONTEXTOBJECT_H_

#include <NexBase.h>
#include <NexEngine.h>
#include <NexThread.h>

namespace nextar {

	typedef std::ptrdiff_t ContextID;
	class _NexEngineAPI ContextObject {
	public:

		typedef const void* ContextParamPtr;

		enum Type {
			TYPE_TEXTURE,
			TYPE_RENDER_TEXTURE,
			TYPE_MULTI_RENDER_TARGET,
			TYPE_RENDER_BUFFER,
			TYPE_CONSTANT_BUFFER,
			TYPE_INDEX_BUFFER,
			TYPE_VERTEX_BUFFER,
			TYPE_PASS,
		};

		class View {
		public:
			//View(ContextObject* _parent) : parentObject(_parent) {}
			/*
			template <typename T>
			inline const T* GetContextObject() const {
				return static_cast<const T*>(parentObject);
			}*/
			virtual void Create(nextar::RenderContext*) {}
			virtual void Update(nextar::RenderContext*, uint32 msg, ContextParamPtr) {}
			virtual void Destroy(nextar::RenderContext*) {}

			virtual ~View() {}

		protected:
			//ContextObject* parentObject;
		};

		ContextObject(Type type);
		virtual ~ContextObject();

		inline void SetContextID(ContextID id) {
			contextId = id;
		}

		/* Called by the class when the object is updated
		 * todo Extend the functionality to include wait-for or wait-less update.
		 * What the later would do is, RequestUpdate will immediately return after
		 * the call and register a callback to be called when the update is done. */
		virtual void RequestUpdate(uint32 updateMsg, ContextObject::ContextParamPtr);

	protected:
		ContextID contextId;
	};

} /* namespace RenderOpenGL */
#endif /* GPUCONTEXTOBJECT_H_ */
