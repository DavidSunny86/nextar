
#ifndef TRANSFORMDATA_H_
#define TRANSFORMDATA_H_

#include "NexMath.h"
#include "BoundingVolume.h"

namespace nextar {

	class TransformData : public AllocMathPool<TransformData, NEX_MATRIX_POOL_NUM_PER_BLOCK> {
	public:
		/* World rotation */
		Quaternion wrot;
		/* World postion */
		Vector3A wpos;
		/* Initial rotation */
		Quaternion irot;
		/* Initial position */
		Vector3A ipos;
		/* Cached matrix */
		Matrix4x4 cached;
		/* variable placement takes into account the alignment */
		float wscale;
		float iscale;
		
		/* we have 2 floats to spare here, mass and volume? */		
		float pad[2];
	};
}

#endif //TRANSFORMDATA_H_

