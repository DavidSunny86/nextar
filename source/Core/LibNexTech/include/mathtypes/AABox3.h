#ifndef NEXTAR_AABB3_H
#define NEXTAR_AABB3_H

//# utility class AABB2

class _NexExport AABox3 : public nextar::AllocGeneral {
public:
	Vector3 min;
	Vector3 max;

	inline AABox3() {}
	inline AABox3(const AABox3& m) : min(m.min),max(m.max) {}
	inline AABox3(const Vector3& minV, const Vector3 & maxV) :
	min(minV),max(maxV) {}

	inline void MakeLargest() {
		max.x = max.y = max.z = Math::INFINITY;
		min.x = min.y = min.z =-Math::INFINITY;
	}

	inline void Invalidate() {
		min.x = Math::INFINITY;
		min.y = Math::INFINITY;
		min.z = Math::INFINITY;
		max.x =-Math::INFINITY;
		max.y =-Math::INFINITY;
		max.z =-Math::INFINITY;
	}

	inline bool IsValid() const {
		if((max.x < min.x) ||
				(max.y < min.y) ||
				(max.z < min.z))
		return false;
		return true;
	}
};

#endif //NEXTAR_AABB3_H
