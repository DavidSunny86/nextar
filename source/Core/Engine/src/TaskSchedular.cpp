/*
 * JobSchedular.cpp
 *
 *  Created on: 22-Mar-2015
 *      Author: obhi
 */
#include <NexEngine.h>
#include <TaskRunner.h>
#include <TaskSchedular.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(TaskSchedular);

#ifdef NEX_TASK_SCHEDULAR_CHECKS
const bool TaskSchedular::_doAllowTraces = NEX_TASK_SCHEDULAR_TRACES;
#endif

TaskSchedular::TaskSchedular()  {

	quit.store(false, std::memory_order_relaxed);
	unsigned int c = std::max<uint32>(1, std::thread::hardware_concurrency()-1);
	// @todo might kill performance, we need a way to serialize
	// in case we dont have the hardware
	OutStringStream str;
	str << "Worker Count: " << c;
	Trace(str.str());
	runners.reserve(c);
	for(uint32 i = 0; i < c; ++i) {
		TaskRunner* r = NEX_NEW(TaskRunner(i+1));
#ifdef NEX_TASK_SCHEDULAR_CHECKS
		if (TaskSchedular::_doAllowTraces)	{
			OutStringStream str2;
			str2 << "Runner" << i;
			r->_name = str2.str();
		}
#endif
		runners.push_back(r);
	}

	mainThread = NEX_NEW(TaskRunner(c+1));
	mainThread->MakeMain();
	runners.push_back(mainThread);
#ifdef NEX_TASK_SCHEDULAR_CHECKS
	mainThread->_name = "Main";
#endif
	for(uint32 i = 0; i < runners.size()-1; ++i) {
		runners[i]->Start(this);
	}
}

TaskSchedular::~TaskSchedular() {
	Close();
}

void TaskSchedular::Close() {
	if (!quit) {
		quit = true;
		for(auto runner : runners) {
			if (runner != mainThread) {
				runner->Join();
	#ifdef NEX_TASK_SCHEDULAR_CHECKS
				if (TaskSchedular::_doAllowTraces)
					Trace(runner->_name + " died.");
	#endif
				NEX_DELETE(runner);
			}
		}
		if (mainThread) {
			NEX_DELETE(mainThread);
			mainThread = nullptr;
		}
		runners.clear();
		{
			std::lock_guard<mutex_type> guard(lock);
			if (submittedTasks.size()) {
				Warn("Tasks purged: " + Convert::ToString((uint32)submittedTasks.size()));
			}
			submittedTasks.clear();
		}
	}
}

void TaskSchedular::AsyncAddChildTask(Task* task) {
	NEX_ASSERT(task);
	task->PrepareSubmit();
	TaskRunner* runner = TaskRunner::AsyncGetRunner();
#ifdef NEX_TASK_SCHEDULAR_CHECKS
	if (TaskSchedular::_doAllowTraces)	{
		OutStringStream str;
		str << "Thread: " << runner->_name << ", task queue: "
				<< runner->requestQueue.Tail() << " via, " << task->_name;
		Trace(str.str());
	}
#endif
	runner->AddTask(task);
#ifdef NEX_TASK_SCHEDULAR_CHECKS
	task->meta.state = Task::TASK_QUEUED;
#endif
	runner->taskCount++;
}

Task* TaskSchedular::AsyncGetWork(TaskRunner* runner, bool repeat) {
	// try to retrieve from own queue first
	Task* task = nullptr;
	do {
		task = runner->RetrieveTask();

		if (!task) {
			uint32 n = (uint32)runners.size();
			uint32 i = runner->laststeal;
			uint32 counter = 0;
			TaskRunner* neighbour;
			while( !task && (counter++) < n ) {
				neighbour = runners[(i++)%n];
				if (neighbour == runner)
					continue;
				task = neighbour->requestQueue.AsyncSteal();
			}
#ifdef NEX_TASK_SCHEDULAR_CHECKS
			if (task) {
				if (TaskSchedular::_doAllowTraces)	{
					OutStringStream str;
					str << "Stolen: " << runner->_name << " has stolen " << neighbour->_name << "'s task: " << task->_name;
					Trace(str.str());
				}
				runner->laststeal = i;
			}
#endif
		}

		if (!task) {
			// try to work on a submitted task
			if(lock.try_lock()) {
				if (submittedTasks.size()) {
					task = submittedTasks.back();
					submittedTasks.pop_back();
#ifdef NEX_TASK_SCHEDULAR_CHECKS
					if (task) {
						if (TaskSchedular::_doAllowTraces)	{
							OutStringStream str;
							str << "Dequed: " << runner->_name << " has fetched task: " << task->_name;
							Trace(str.str());
						}
					}
#endif
				}
				lock.unlock();
			}
		}

		if (!repeat || task) {
#ifdef NEX_TASK_SCHEDULAR_CHECKS
			if(task)
				NEX_ASSERT(task->meta.state == Task::TASK_QUEUED);
#endif
			return task;
		}
#ifdef NEX_TASK_SCHEDULAR_CHECKS
		if (TaskSchedular::_doAllowTraces)	{
			//OutStringStream str;
			//str << "Yieldx: " << runner->_name << " is yielding.";
			//Trace(str.str());
		}
#endif
		std::this_thread::yield();
	} while(!quit.load(std::memory_order_relaxed));
	return nullptr;
}

void TaskSchedular::AsyncResume(Task* waitingOn, uint32 limit) {
	Task* toExec = nullptr;
	TaskRunner* runner = TaskRunner::AsyncGetRunner();
	while(waitingOn->meta.refCount.load(std::memory_order_relaxed) > (int)limit) {
		if (toExec ||
				(toExec = AsyncGetWork(runner, false))
			) {
#ifdef NEX_TASK_SCHEDULAR_CHECKS
			if (TaskSchedular::_doAllowTraces)	{
				OutStringStream str;
				str << "Waited: " << runner->_name << ", running task: " << toExec->_name;
				Trace(str.str());
			}
#endif
			toExec = TaskRunner::Execute(toExec);
		}
	}

	if (toExec) {
#ifdef NEX_TASK_SCHEDULAR_CHECKS
		NEX_ASSERT(toExec->meta.state == Task::TASK_QUEUED ||
				toExec->meta.state == Task::TASK_INIT);
		toExec->meta.state = Task::TASK_QUEUED;
#endif
		runner->AddTask(toExec);
	}
}

void TaskSchedular::AsyncWork(TaskRunner* runner) {
	Task* toExec = nullptr;
	TaskRunner::AsyncGetRunner() = runner;
	runner->id = std::this_thread::get_id();
	while (toExec || (toExec = AsyncGetWork(runner))) {
#ifdef NEX_TASK_SCHEDULAR_CHECKS
		String lastTask = toExec->_name;
		if (TaskSchedular::_doAllowTraces)	{
			OutStringStream str;
			str << "Worker: " << runner->_name << ", running task: " << lastTask;
			Trace(str.str());
		}
#endif
		toExec = TaskRunner::Execute(toExec);
#ifdef NEX_TASK_SCHEDULAR_CHECKS
		if (TaskSchedular::_doAllowTraces)	{
			OutStringStream str;
			str << "Worker: " << runner->_name << ", finished task: " << lastTask;
			Trace(str.str());
		}
#endif

	}
	// reliquish all tasks to someone else
	runner->Relinquish(this);
#ifdef NEX_TASK_SCHEDULAR_CHECKS
	if (TaskSchedular::_doAllowTraces)	{
		Trace(runner->_name + " finished.");
	}
#endif
}

void TaskSchedular::AsyncSubmit(Task* singleTask) {
	singleTask->PrepareSubmit();
	std::lock_guard<mutex_type> guard(lock);
#ifdef NEX_TASK_SCHEDULAR_CHECKS
	NEX_ASSERT(singleTask->meta.state == Task::TASK_QUEUED ||
			singleTask->meta.state == Task::TASK_INIT);
	singleTask->meta.state = Task::TASK_QUEUED;
	if (TaskSchedular::_doAllowTraces)	{
		OutStringStream str;
		str << "Submit to queue: " <<  "task: " << singleTask->_name;
		Trace(str.str());
	}
#endif

	submittedTasks.push_back(singleTask);
}

void TaskSchedular::AsyncSubmit(Task** multipleTasks, uint32 n) {
	std::lock_guard<mutex_type> guard(lock);
#ifdef NEX_TASK_SCHEDULAR_CHECKS
	if (TaskSchedular::_doAllowTraces)	{
		OutStringStream str;
		str << "Submit: tasks " << n;
		Trace(str.str());
	}
	std::for_each(multipleTasks, multipleTasks+n, [] (Task* singleTask) {
		NEX_ASSERT(singleTask->meta.state == Task::TASK_QUEUED ||
						singleTask->meta.state == Task::TASK_INIT);
				singleTask->meta.state = Task::TASK_QUEUED;
	});
#endif
	std::for_each(multipleTasks, multipleTasks + n, [](Task* singleTask) {
		singleTask->PrepareSubmit();
	});
	submittedTasks.insert(submittedTasks.end(), multipleTasks, multipleTasks+n);
}

} /* namespace nextar */

