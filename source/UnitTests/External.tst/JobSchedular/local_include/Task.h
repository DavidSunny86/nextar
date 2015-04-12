/*
 * Task.h
 *
 *  Created on: 12-Apr-2015
 *      Author: obhi
 */

#ifndef UNITTESTS_EXTERNAL_TST_JOBSCHEDULAR_LOCAL_INCLUDE_TASK_H_
#define UNITTESTS_EXTERNAL_TST_JOBSCHEDULAR_LOCAL_INCLUDE_TASK_H_

#include <NexBase.h>
#include <NexThread.h>
#include <Logger.h>

namespace nextar {

class Task : public AllocGeneral {
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
#endif
		Task* parent;
		atomic_int refCount;
	};
public:


#ifdef NEX_DEBUG
	String _name;
	Task(const String& name) : Task() {
		_name = name;
	}
#else
	Task(const String& name) : Task() {}
#endif

	inline Task() {
		meta.state = TASK_INIT;
		meta.parent = nullptr;
		meta.refCount.store(1, std::memory_order_relaxed);
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

	friend class TaskSchedular;
	friend class TaskRunner;
	Meta meta;
};

} /* namespace nextar */

#endif /* UNITTESTS_EXTERNAL_TST_JOBSCHEDULAR_LOCAL_INCLUDE_TASK_H_ */
