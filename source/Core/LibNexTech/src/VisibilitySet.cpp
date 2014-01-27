/*
 * VisibilitySet.cpp
 *
 *  Created on: Oct 5, 2011
 *      Author: obhi
 */
#include "NexHeaders.h"
#include "VisibilitySet.h"
#include "RadixSort.h"

namespace nextar {
	/******************************
	 * RenderQueue
	 */
	void RenderQueue::Sort() {
		if (visibles.size() > 32)
			RadixSort(visibles.data(), visibles.size(), 0xff000000, 24);
		else
			std::sort(visibles.begin(), visibles.end());
	}

	/******************************
	 * VisibilitySet
	 */
	VisibilitySet::VisibilitySet() : visibleCount(0) {
	}

	VisibilitySet::~VisibilitySet() {
	}

	void VisibilitySet::SortSet() {
		for (auto &q : queues) {
			q.Sort();
		}
	}
} /* namespace nextar */
