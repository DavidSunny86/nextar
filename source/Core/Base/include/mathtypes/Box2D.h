#ifndef NEXTAR_AABB2_H
#define NEXTAR_AABB2_H

//# utility class AABB2

class _NexBaseAPI Box2D : public nextar::AllocGeneral {
public:
	Vector2 min;
	Vector2 max;

	inline Box2D() {
	}

	inline Box2D(float minX, float minY, float maxX, float maxY) :
			min(minX, minY), max(maxX, maxY) {
	}

	inline Box2D(const Box2D & b) : min(b.min), max(b.max) {
	}
};

#endif //NEXTAR_AABB2_H
