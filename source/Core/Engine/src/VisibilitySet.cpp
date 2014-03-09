/*
 * VisibilitySet.cpp
 *
 *  Created on: Oct 5, 2011
 *      Author: obhi
 */
#include "NexEngine.h"
#include "VisibilitySet.h"
#include "RenderManager.h"

namespace nextar {
	/******************************
	 * RenderQueue
	 */
	RenderQueue::RenderQueue() : descriptor(0), flags(DEFAULT_FLAGS) {
	}

	void RenderQueue::Sort() {
	}

	/******************************
	 * VisibilitySet
	 */
	VisibilitySet::VisibilitySet() : visibleCount(0) {
		/* Create basic queues */
		const RenderQueueDescList& rqdl = RenderManager::Instance().GetRenderQueueInfo();
		queues.resize(rqdl.size());
		RenderQueueList::iterator it = queues.begin();
		RenderQueueList::iterator end = queues.end();

		RenderQueueDescList::const_iterator descIt = rqdl.begin();
		RenderQueueDescList::const_iterator descEnd = rqdl.end();
		for (; it != end; ++it, ++descIt) {
			(*it).SetDesc((*descIt));
		}
	}

	VisibilitySet::~VisibilitySet() {
	}

	void VisibilitySet::SortSet() {
	}
} /* namespace nextar */
