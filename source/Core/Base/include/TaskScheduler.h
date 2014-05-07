
#ifndef TASKSCHEDULER_H_
#define TASKSCHEDULER_H_

#include <NexBase.h>
#include <Singleton.h>

namespace nextar {

	class TaskScheduler : public Singleton<TaskScheduler>,
		public AllocGeneral {
	public:

		TaskScheduler();
		virtual ~TaskScheduler();

	protected:
	};
}

#endif //TASKSCHEDULER_H_
