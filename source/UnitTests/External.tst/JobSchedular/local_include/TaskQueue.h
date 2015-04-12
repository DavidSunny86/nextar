/*
 * TaskQueue.h
 *
 *  Created on: 05-Apr-2015
 *      Author: obhi
 */

#ifndef UNITTESTS_EXTERNAL_TST_TASKQUEUE_LOCAL_INCLUDE_TASKQUEUE_H_
#define UNITTESTS_EXTERNAL_TST_TASKQUEUE_LOCAL_INCLUDE_TASKQUEUE_H_

#include <NexBase.h>

namespace nextar {

class Task;
class TaskSchedular;

typedef Task TaskItem;

class TaskQueue {
public:
	TaskQueue();
	virtual ~TaskQueue();

	TaskItem* AsyncSteal(int32 attempts = 5);

	void Add(TaskItem* item);
	TaskItem* Retrieve();
	void RetrieveAll(TaskItem**& to, uint32& n);

	uint32 Tail() {
		return tail.load(std::memory_order_relaxed);
	}

	inline bool TryAcquire() {
		if (!taskPoolLocked.test_and_set(std::memory_order_relaxed))
			return true;
		return false;
	}

	inline void Acquire() {
		while (taskPoolLocked.test_and_set(std::memory_order_relaxed))
			std::this_thread::yield();
	}

	inline void Release() {
		taskPoolLocked.clear();
	}

#ifdef NEX_DEBUG
#endif

protected:

	uint32 AddFreeSlots(uint32 n);
	TaskItem** AllocatePool(uint32 to);

	TaskItem** taskPool;
	atomic_flag taskPoolLocked;

	static uint32 minPoolSize;
	uint32 capacity;
	atomic_uint head;
	atomic_uint tail;
};

} /* namespace nextar */

#endif /* UNITTESTS_EXTERNAL_TST_TASKQUEUE_LOCAL_INCLUDE_TASKQUEUE_H_ */
