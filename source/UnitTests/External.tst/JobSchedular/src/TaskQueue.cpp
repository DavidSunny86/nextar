/**\n * Created on: Sun Apr  5 18:02:35 IST 2015\n * @author: obhi\n*/

#include <TaskQueue.h>

namespace nextar {

uint32 TaskQueue::minPoolSize = 32;
TaskQueue::TaskQueue() : head(0), tail(0), capacity(0), taskPool(0)
		  {
	taskPoolLocked.clear();
}

TaskQueue::~TaskQueue() {
	if (taskPool)
		NEX_FREE(taskPool, MEMCAT_GENERAL);
}

uint32 TaskQueue::AddFreeSlots(uint32 n) {
	uint32 utail = tail.load(std::memory_order_relaxed);
	if (utail + n < capacity)
		return utail;
	Acquire();
	uint32 uhead = head.load(std::memory_order_relaxed);
	utail -= uhead;
	if (!taskPool) {
		taskPool = AllocatePool(std::max(utail + n, minPoolSize));
	} else if (utail + n < (capacity*2)/3) {
		// move the block and return
		std::memmove(taskPool, taskPool + uhead, utail * sizeof(TaskItem*));
		head.store(0, std::memory_order_relaxed);
		tail.store(utail, std::memory_order_relaxed);
	} else {
		uint32 newPoolSize = utail + n;
		if (newPoolSize < capacity * 2)
			newPoolSize = capacity * 2;
		TaskItem** newPool = AllocatePool(std::max(newPoolSize, minPoolSize));
		std::memcpy(newPool, taskPool+uhead, utail * sizeof(TaskItem*));
		taskPool = newPool;
		head.store(0, std::memory_order_relaxed);
		tail.store(utail, std::memory_order_relaxed);
	}
	Release();
	return utail;
}

TaskItem** TaskQueue::AllocatePool(uint32 to) {
	TaskItem** howMuch = (TaskItem**)NEX_ALLOC(to*sizeof(TaskItem*), MEMCAT_GENERAL);
#ifdef NEX_DEBUG
	std::memset(howMuch, 0xff, to * sizeof(TaskItem*));
#endif
	capacity = to;
	return howMuch;
}

void TaskQueue::Add(TaskItem* t) {
	uint32 l = AddFreeSlots(1);
	taskPool[l] = t;
	NEX_ASSERT(l+1 <= capacity);
	tail.store(l+1, std::memory_order_release);
}

TaskItem* TaskQueue::Retrieve() {
	uint32 H = head.fetch_add(1, std::memory_order_acq_rel);
	uint32 T = tail.load(std::memory_order_acquire);
	if (H >= T) {
		head.store(T, std::memory_order_release);
		return nullptr;
	}
	TaskItem* ret = taskPool[H];
#ifdef NEX_DEBUG
	std::memset(taskPool + H, 0xee, sizeof(TaskItem*));
#endif
	return ret;
}

void TaskQueue::RetrieveAll(TaskItem**& to, uint32& n) {
	uint32 H = head.load(std::memory_order_acquire);
	uint32 T = tail.load(std::memory_order_acquire);
	head.store(T, std::memory_order_release);
	if (H >= T) {
		to = nullptr;
		n = 0;
		return;
	}
	to = taskPool + H;
	n = T - H;
}

TaskItem* TaskQueue::AsyncSteal(int32 attempts) {
	TaskItem* ret = nullptr;
	bool acquired = false;
	while(1) {
		if((acquired = TryAcquire()) || (--attempts) <= 0)
			break;
		std::this_thread::yield();
	};

	if (!acquired)
		return nullptr;
	ret = Retrieve();
	Release();
	return ret;
}

} /* namespace nextar */
