/*
 * SceneParametrs.h
 *
 *  Created on: 25-Sep-2013
 *      Author: obhi
 */

#ifndef SCENEPARAMETERS_H_
#define SCENEPARAMETERS_H_

#include "NexMath.h"

namespace nextar {

	class SceneParameters : public AllocGeneral {
	public:
		SceneParameters();
		~SceneParameters();

	protected:
		Vector3 maxPoint;
		Vector3 minPoint;
	};

} /* namespace nextar */
#endif /* SCENEPARAMETERS_H_ */
