#ifndef NEXTAR_EULARANGLES_H
#define NEXTAR_EULARANGLES_H

//# Generic axis angle representing
//# 3d rotation.
//# Mostly avoids SSE or other optimization.

class _NexBaseExport EularAngles : public nextar::AllocGeneral {
public:
	float yaw; // heading
	float pitch;
	float roll;// banking

	inline EularAngles() : yaw(0), pitch(0), roll(0) {
	}

	inline EularAngles(const float _yaw, const float _pitch, const float _roll) :
	yaw(_yaw), pitch(_pitch), roll(_roll) {

	}

	inline EularAngles(const EularAngles& ea) : yaw(ea.yaw),
	pitch(ea.pitch), roll(ea.roll) {
	}

	// functions

	inline void Identity() {
		yaw = pitch = roll = 0.0f;
	}

	void Canonize();
	void FromQuat(QuatF);
	void FromInvQuat(QuatF);
	void FromMat3x3(const Matrix3x3&);

};

#endif //NEXTAR_EULARANGLES_H
