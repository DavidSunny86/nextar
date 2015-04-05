/*
 * JobSchedular.h
 *
 *  Created on: 22-Mar-2015
 *      Author: obhi
 */

#ifndef UNITTESTS_EXTERNAL_TST_JOBSCHEDULAR_LOCAL_INCLUDE_JOBSCHEDULAR_H_
#define UNITTESTS_EXTERNAL_TST_JOBSCHEDULAR_LOCAL_INCLUDE_JOBSCHEDULAR_H_

#include <NexBase.h>
#include <NexThread.h>
#include <Singleton.h>
#include <Logger.h>

namespace nextar {

class Task;
class Runner;

typedef deque<Task*, AllocatorGeneral, STDPoolAllocator< Task*, 32, MEMCAT_GENERAL> >::type RunnableQueue;
typedef vector<Runner*>::type RunnerList;

class Task : public AllocGeneral {
public:

	NEX_LOG_HELPER(Task);

#ifdef NEX_DEBUG
	String _name;
	Task(const String& name) : Task() {
		_name = name;
	}
#else
	Task(const String& name) : Task() {}
#endif
	inline Task() : parent(nullptr) {
		refCount.store(1, std::memory_order_relaxed);
	}

	// entry point
	virtual Task* Run() = 0;
	// spawn a child job
	void Spawn(Task* child);
	// wait for children to finish
	void Wait();

protected:

	~Task() {}

private:

	friend class TaskSchedular;
	friend class Runner;

	Task* parent;
	atomic_int refCount;
};

class Runner : public AllocGeneral {
public:
	NEX_LOG_HELPER(Runner);

	typedef std::mutex mutex_type;

	Runner(uint32 startIndex);

	void Start();
	void DoRun();
	void MakeMain();

protected:

	atomic_int taskCount;
	static Runner*& AsyncGetRunner();
	Task* AsyncPopTask();
	friend class TaskSchedular;

	bool quitThread;
	uint32 laststeal;
	mutex_type queueLock;
	RunnableQueue requestQueue;
	std::thread::id id;
	std::thread runner;
};

class TaskSchedular :
		public Singleton<TaskSchedular>,
		public AllocGeneral {
public:
	NEX_LOG_HELPER(TaskSchedular);

	TaskSchedular();
	virtual ~TaskSchedular();

	void AsyncResume(Task* waitingOn);
	void AsyncAddChildTask(Task*);
	Task* AsyncWaitForWork(Runner*);

protected:
		
	std::condition_variable requestQueueVar;
	RunnerList runners;
	Runner* mainThread;
};

} /* namespace nextar */

#endif /* UNITTESTS_EXTERNAL_TST_JOBSCHEDULAR_LOCAL_INCLUDE_JOBSCHEDULAR_H_ */
