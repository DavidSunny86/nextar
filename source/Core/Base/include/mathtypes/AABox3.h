#ifndef NEXTAR_AABB3_H
#define NEXTAR_AABB3_H

//# utility class AABB2

class _NexBaseAPI AABox3 : public nextar::AllocGeneral {
public:
	Vector3 min;
	Vector3 max;

	inline AABox3() {}
	inline AABox3(const AABox3& m) : min(m.min),max(m.max) {}
	inline AABox3(const Vector3& minV, const Vector3 & maxV) :
	min(minV),max(maxV) {}

	inline void MakeLargest() {
		max.x = max.y = max.z = Math::SCALAR_MAX;
		min.x = min.y = min.z =-Math::SCALAR_MAX;
	}

	inline void Invalidate() {
		min.x = Math::SCALAR_MAX;
		min.y = Math::SCALAR_MAX;
		min.z = Math::SCALAR_MAX;
		max.x =-Math::SCALAR_MAX;
		max.y =-Math::SCALAR_MAX;
		max.z =-Math::SCALAR_MAX;
	}

	inline bool IsValid() const {
		if((max.x < min.x) ||
				(max.y < min.y) ||
				(max.z < min.z))
		return false;
		return true;
	}

	inline Vector3A GetCenter() const {
		return Vec3ASet((min.x + max.x) * .5f,
			(min.y + max.y) * .5f,
			(min.z + max.z) * .5f);
	}

	inline Vector3A GetSize() const {
		return Vec3ASet((max.x - min.x),
			(max.y - min.y),
			(max.z - min.z));
	}
};

#endif //NEXTAR_AABB3_H