/*
 * Runner.cpp
 *
 *  Created on: 12-Apr-2015
 *      Author: obhi
 */

#include <TaskRunner.h>
#include <TaskSchedular.h>


namespace nextar {

void TaskRunner::Start(TaskSchedular* schedular) {
	runner = std::thread(&TaskSchedular::AsyncWork, schedular, this);
}

TaskRunner::~TaskRunner() {
}

TaskRunner*& TaskRunner::AsyncGetRunner() {
	thread_local static TaskRunner* thisRunner = nullptr;
	return thisRunner;
}

void TaskRunner::Relinquish(TaskSchedular* to) {
	requestQueue.Acquire();
	TaskItem** items = nullptr;
	uint32 n = 0;
	requestQueue.RetrieveAll(items, n);
	if (n) {
		to->AsyncSubmit(items, n);
	}
	requestQueue.Release();
}

} /* namespace nextar */
