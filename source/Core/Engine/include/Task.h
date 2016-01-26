/*
 * Task.h
 *
 *  Created on: 12-Apr-2015
 *      Author: obhi
 */

#ifndef UNITTESTS_EXTERNAL_TST_JOBSCHEDULAR_LOCAL_INCLUDE_TASK_H_
#define UNITTESTS_EXTERNAL_TST_JOBSCHEDULAR_LOCAL_INCLUDE_TASK_H_

#include <NexEngine.h>
#include <NexThread.h>
#include <Logger.h>

namespace nextar {

class _NexEngineAPI Task : public AllocGeneral {
protected:
	NEX_LOG_HELPER(Task);
	enum State : uint32 {
		TASK_INIT,
		TASK_RUNNING,
		TASK_FINISHED,
		TASK_QUEUED,
	};

	struct Meta {
#ifdef NEX_DEBUG
		std::atomic<State> state;
		atomic_bool waitingForChildren;
		atomic_bool waiting;
		std::thread::id waitingForChildrenId;
		std::thread::id waitingId;
		std::thread::id runId;
#endif
		Task* parent;
		atomic_int refCount;
	};
public:


#ifdef NEX_TASK_SCHEDULAR_CHECKS
	String _name;
	Task(const String& name) : Task() {
		_name = name;
	}
#else
	Task(const String& name) : Task() {}
#endif

	inline Task() {
#ifdef NEX_TASK_SCHEDULAR_CHECKS
		meta.state = TASK_INIT;
		meta.waitingForChildren.store(false);
#endif
		meta.parent = nullptr;
		meta.refCount.store(0, std::memory_order_relaxed);
	}
	// entry point
	virtual Task* Run() = 0;
	// spawn a child task
	void Spawn(Task* child);
	// wait for this task to finish
	void Wait();
	// wait for children to finish
	void WaitForChildren();

protected:
	
	~Task() {}
	
private:

	inline void PrepareSubmit() {
#ifdef NEX_TASK_SCHEDULAR_CHECKS
		meta.state = Task::TASK_INIT;
#endif
		meta.refCount.store(1, std::memory_order_relaxed);
	}

	friend class TaskSchedular;
	friend class TaskRunner;
	Meta meta;
};

} /* namespace nextar */

#endif /* UNITTESTS_EXTERNAL_TST_JOBSCHEDULAR_LOCAL_INCLUDE_TASK_H_ */
