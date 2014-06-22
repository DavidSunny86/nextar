/*
 * Task.h
 *
 *  Created on: 10-Sep-2013
 *      Author: obhi
 */

#ifndef TASK_H_
#define TASK_H_

namespace nextar {

class Task {
public:
	Task();
	virtual ~Task();

	virtual Task* Execute() = 0;
};

} /* namespace nextar */
#endif /* TASK_H_ */
