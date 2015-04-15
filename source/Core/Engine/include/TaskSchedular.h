/*
 * JobSchedular.h
 *
 *  Created on: 22-Mar-2015
 *      Author: obhi
 */

#ifndef UNITTESTS_EXTERNAL_TST_JOBSCHEDULAR_LOCAL_INCLUDE_JOBSCHEDULAR_H_
#define UNITTESTS_EXTERNAL_TST_JOBSCHEDULAR_LOCAL_INCLUDE_JOBSCHEDULAR_H_

#include <NexEngine.h>
#include <NexThread.h>
#include <Singleton.h>
#include <Logger.h>
#include <Task.h>

namespace nextar {

class Task;
class TaskRunner;

typedef vector<TaskRunner*>::type RunnerList;
typedef deque<Task*>::type TaskList;

class _NexEngineAPI TaskSchedular :
		public Singleton<TaskSchedular>,
		public AllocGeneral {
public:
	NEX_LOG_HELPER(TaskSchedular);

	TaskSchedular();
	~TaskSchedular();

	void Close();
	void AsyncSpawn(Task* t) {
		AsyncAddChildTask(t);
	}
	void AsyncSubmit(Task* t);
	void AsyncSubmit(Task** t, uint32 n);

protected:
	void AsyncResume(Task* waitingOn, uint32 limit);
	void AsyncAddChildTask(Task*);
	Task* AsyncGetWork(TaskRunner*, bool repeat=true);
	void AsyncWork(TaskRunner* runner);

#ifdef NEX_TASK_SCHEDULAR_CHECKS
	static const bool _doAllowTraces;
#endif

	typedef spinlock_mutex mutex_type;
	mutex_type lock;
	TaskList submittedTasks;
	atomic_bool quit;
	RunnerList runners;
	TaskRunner* mainThread;

	friend class TaskRunner;
	friend class Task;
};

} /* namespace nextar */

#endif /* UNITTESTS_EXTERNAL_TST_JOBSCHEDULAR_LOCAL_INCLUDE_JOBSCHEDULAR_H_ */
