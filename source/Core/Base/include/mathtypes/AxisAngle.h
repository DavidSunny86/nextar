#ifndef NEXTAR_AXISANGLE_H
#define NEXTAR_AXISANGLE_H

//# Generic axis angle representing
//# 3d rotation.
//# Mostly avoids SSE or other optimization.

class _NexBaseAPI AxisAngle : public nextar::AllocGeneral {
public:
	Vector3 axis;
	float angle;
};

#endif //NEXTAR_AXISANGLE_H
