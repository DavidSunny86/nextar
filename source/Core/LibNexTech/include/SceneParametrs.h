/*
 * SceneParametrs.h
 *
 *  Created on: 25-Sep-2013
 *      Author: obhi
 */

#ifndef SCENEPARAMETRS_H_
#define SCENEPARAMETRS_H_

#include "NexMath.h"

namespace nextar {

	class SceneParametrs {
	public:
		SceneParametrs();
		~SceneParametrs();

	protected:
		Vector3 maxPoint;
		Vector3 minPoint;
	};

} /* namespace nextar */
#endif /* SCENEPARAMETRS_H_ */
