/*
 * Runner.h
 *
 *  Created on: 12-Apr-2015
 *      Author: obhi
 */

#ifndef UNITTESTS_EXTERNAL_TST_JOBSCHEDULAR_LOCAL_INCLUDE_RUNNER_H_
#define UNITTESTS_EXTERNAL_TST_JOBSCHEDULAR_LOCAL_INCLUDE_RUNNER_H_

#include <NexEngine.h>
#include <NexThread.h>
#include <Logger.h>
#include <Task.h>
#include <TaskQueue.h>

namespace nextar {

class TaskSchedular;
class TaskRunner : public AllocGeneral {
public:
	//NEX_LOG_HELPER(Runner);

	TaskRunner(uint32 startIndex) :
			laststeal(startIndex), taskCount(0) {
	}
	~TaskRunner();

	void Start(TaskSchedular* schedular);
	void Relinquish(TaskSchedular* to);

	inline void MakeMain() {
		Task* toExec = nullptr;
		AsyncGetRunner() = this;
		id = std::this_thread::get_id();
	}

	inline void Join() {
		runner.join();
	}

	inline Task* RetrieveTask() {
		return requestQueue.Retrieve();
	}

	inline void AddTask(Task* t) {
		requestQueue.Add(t);
	}

	static inline Task* Execute(Task* toExec) {
#ifdef NEX_TASK_SCHEDULAR_CHECKS
		NEX_ASSERT(toExec->meta.state == Task::TASK_QUEUED ||
				toExec->meta.state == Task::TASK_INIT);
		toExec->meta.state = Task::TASK_RUNNING;
		toExec->meta.runId = std::this_thread::get_id();
#endif
		Task* nextToExec = toExec->Run();
#ifdef NEX_TASK_SCHEDULAR_CHECKS
		NEX_ASSERT(toExec->meta.state == Task::TASK_RUNNING);
		toExec->meta.state = Task::TASK_FINISHED;
#endif
		toExec->meta.refCount.fetch_sub(1, std::memory_order_relaxed);
		if(toExec->meta.parent)
			toExec->meta.parent->meta.refCount.fetch_sub(1, std::memory_order_relaxed);
		return nextToExec;
	}

protected:

#ifdef NEX_TASK_SCHEDULAR_CHECKS
	String _name;
#endif

	atomic_int taskCount;
	static TaskRunner*& AsyncGetRunner();

	friend class TaskSchedular;

	TaskQueue requestQueue;
	uint32 laststeal;
	std::thread::id id;
	std::thread runner;
};


} /* namespace nextar */

#endif /* UNITTESTS_EXTERNAL_TST_JOBSCHEDULAR_LOCAL_INCLUDE_RUNNER_H_ */
