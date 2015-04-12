/*
 * Task.cpp
 *
 *  Created on: 12-Apr-2015
 *      Author: obhi
 */

#include <Task.h>

namespace nextar {


void Task::Spawn(Task* child) {
#ifdef NEX_DEBUG
	NEX_ASSERT(meta.state == Task::TASK_RUNNING);
#endif
	child->meta.parent = this;
	meta.refCount.fetch_add(1, std::memory_order_relaxed);
	TaskSchedular::Instance().AsyncAddChildTask(child);
}

void Task::WaitForChildren() {
#ifdef NEX_DEBUG
	NEX_ASSERT(meta.state == Task::TASK_RUNNING);
#endif
	uint32 ref = meta.refCount.load(std::memory_order_relaxed);
	if (ref <= 1)
		return;
	TaskSchedular::Instance().AsyncResume(this, 1);
}

void Task::Wait() {
	uint32 ref = meta.refCount.load(std::memory_order_relaxed);
	if (!ref)
		return;
	TaskSchedular::Instance().AsyncResume(this, 0);
}

} /* namespace nextar */
