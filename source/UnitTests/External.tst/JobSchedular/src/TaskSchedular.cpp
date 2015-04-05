/*
 * JobSchedular.cpp
 *
 *  Created on: 22-Mar-2015
 *      Author: obhi
 */

#include <TaskSchedular.h>

namespace nextar {

NEX_DEFINE_SINGLETON_PTR(TaskSchedular);

void nextar::Task::Spawn(Task* child) {
	child->parent = this;
	refCount++;
	TaskSchedular::Instance().AsyncAddChildTask(child);
}

void nextar::Task::Wait() {
	if (!refCount.load(std::memory_order_relaxed))
		return;
	TaskSchedular::Instance().AsyncResume(this);
}

void Runner::Start() {
	runner = std::thread(&Runner::DoRun, this);
}

void Runner::MakeMain() {
	Task* toExec = nullptr;
	AsyncGetRunner() = this;
	id = std::this_thread::get_id();
}

void Runner::DoRun() {

	Task* toExec = nullptr;
	AsyncGetRunner() = this;
	id = std::this_thread::get_id();
	while (toExec || (toExec = TaskSchedular::Instance().AsyncWaitForWork(this))) {
#ifdef NEX_DEBUG
		OutStringStream str;
		str << "Thread: " << id << ", running task: " << toExec->_name;
		Trace(str.str());
#endif
		//@todo This is an issue, wait might not work because of this
		// so we prove it will.
		toExec->refCount--;
		Task* nextToExec = toExec->Run();
		if(toExec->parent)
			toExec->parent->refCount--;
		toExec = nextToExec;
	}
}

Runner::Runner(uint32 startIndex) :
		laststeal(startIndex),
		quitThread(false) {
}

Runner*& Runner::AsyncGetRunner() {
	thread_local static Runner* thisRunner = nullptr;
	return thisRunner;
}

Task* Runner::AsyncPopTask() {
	if (taskCount.load(std::memory_order_relaxed) > 0) {
		taskCount--;
		std::unique_lock<Runner::mutex_type> g(queueLock);
		if (requestQueue.size()) {
			Task* ret = requestQueue.front();
			requestQueue.pop_front();
			return ret;
		}
	}
	return nullptr;
}

TaskSchedular::TaskSchedular()  {

	unsigned int c = std::max<uint32>(1, std::thread::hardware_concurrency()-1);
	// @todo might kill performance, we need a way to serialize
	// in case we dont have the hardware
	OutStringStream str;
	str << "Worker Count: " << c;
	Trace(str.str());
	runners.reserve(c);
	for(uint32 i = 0; i < c; ++i) {
		runners.push_back(NEX_NEW(Runner(i+1)));
	}

	for(auto r : runners)
		r->Start();
	mainThread = NEX_NEW(Runner(c+1));
	mainThread->MakeMain();
}

TaskSchedular::~TaskSchedular() {
	for(auto runner : runners) {
		{
			std::unique_lock<Runner::mutex_type> g(runner->queueLock);
			runner->quitThread = true;
		}
	}
	requestQueueVar.notify_all();

	for(auto runner : runners) {
		runner->runner.join();
		NEX_DELETE(runner);
	}

	NEX_DELETE(mainThread);

	runners.clear();
}

void TaskSchedular::AsyncAddChildTask(Task* task) {
	NEX_ASSERT(task);
	{
		Runner* runner = Runner::AsyncGetRunner();
		std::unique_lock<Runner::mutex_type> g(runner->queueLock);
		runner->requestQueue.push_back(task);
		runner->taskCount++;
	}

	requestQueueVar.notify_one();
}

void TaskSchedular::AsyncResume(Task* waitingOn) {
	Task* toExec = nullptr;
	while(waitingOn->refCount.load(std::memory_order_relaxed)) {
		if (toExec ||
				(toExec = TaskSchedular::Instance().AsyncWaitForWork(Runner::AsyncGetRunner()))
			) {
#ifdef NEX_DEBUG
			OutStringStream str;
			str << "Thread: " << std::this_thread::get_id() << ", running task: " << toExec->_name;
			Trace(str.str());
#endif
			toExec->refCount--;
			Task* nextToExec = toExec->Run();
			if(toExec->parent)
				toExec->parent->refCount--;
			toExec = nextToExec;
		}
	}
}

Task* TaskSchedular::AsyncWaitForWork(Runner* runner) {
	std::unique_lock<Runner::mutex_type> g(runner->queueLock);
	requestQueueVar.wait(g, [this, runner] {
		if (runner->requestQueue.empty()) {
			uint32 n = (uint32)runners.size();
			uint32 i = runner->laststeal;
			uint32 counter = 0;
			Task* task = nullptr;
			while( !task && (counter++) < n ) {
				Runner* neighbour = runners[(i++)%n];
				if (neighbour == runner)
					continue;
				task = neighbour->AsyncPopTask();
			}
			if (task) {
#ifdef NEX_DEBUG
				OutStringStream str;
				str << "Thread: " << runner->id << ", pushing task: " << task->_name;
				Trace(str.str());
#endif
				runner->requestQueue.push_back(task);
				runner->taskCount++;
			}
			runner->laststeal = i%n;
		}
		return !runner->requestQueue.empty() || runner->quitThread;
	});

	if (runner->quitThread)
		return nullptr;
	runner->taskCount--;
	Task* ret = runner->requestQueue.front();
	runner->requestQueue.pop_front();
	return ret;
}

} /* namespace nextar */

