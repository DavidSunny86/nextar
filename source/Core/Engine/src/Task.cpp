/*
 * Task.cpp
 *
 *  Created on: 12-Apr-2015
 *      Author: obhi
 */

#include <Task.h>

namespace nextar {


void Task::Spawn(Task* child) {
#ifdef NEX_TASK_SCHEDULAR_CHECKS
	NEX_ASSERT(meta.state == Task::TASK_RUNNING);
#endif
	child->meta.parent = this;
	meta.refCount.fetch_add(1, std::memory_order_relaxed);
	TaskSchedular::Instance().AsyncAddChildTask(child);
}

void Task::WaitForChildren() {
#ifdef NEX_TASK_SCHEDULAR_CHECKS
	NEX_ASSERT(meta.state == Task::TASK_RUNNING);
	std::thread::id id = std::this_thread::get_id();
	NEX_ASSERT(!meta.waitingForChildren.load() ||
				meta.waitingForChildrenId != id);
	meta.waitingForChildrenId = id;
	meta.waitingForChildren.store(true);
	NEX_ASSERT(!meta.waiting.load() ||
			meta.waitingForChildrenId != meta.waitingId);
#endif
	uint32 ref = meta.refCount.load(std::memory_order_relaxed);
	if (ref <= 1)
		return;
	TaskSchedular::Instance().AsyncResume(this, 1);
#ifdef NEX_TASK_SCHEDULAR_CHECKS
	meta.waitingForChildren.store(false);
#endif
}

void Task::Wait() {
#ifdef NEX_TASK_SCHEDULAR_CHECKS
	std::thread::id id = std::this_thread::get_id();
	NEX_ASSERT(!meta.waiting.load() ||
			meta.waitingId != id);
	meta.waitingId = id;
	meta.waiting.store(true);
	NEX_ASSERT(!meta.waitingForChildren.load() ||
				meta.waitingForChildrenId != meta.waitingId);
#endif
	uint32 ref = meta.refCount.load(std::memory_order_relaxed);
	if (!ref)
		return;
	TaskSchedular::Instance().AsyncResume(this, 0);
#ifdef NEX_TASK_SCHEDULAR_CHECKS
	meta.waiting.store(false);
#endif
}

} /* namespace nextar */
