#ifndef NEXTAR_FRUSTUM_H
#define NEXTAR_FRUSTUM_H

#include <MathBase.h>

namespace nextar {

class _NexBaseAPI Frustum: public AllocGeneral {
public:
	// common frustums will have these planes

	enum PlaneType {
		PLANE_BOTTOM, PLANE_FAR, PLANE_LEFT, PLANE_NEAR, PLANE_RIGHT, PLANE_TOP,
	};

	enum {
		CAM_FRUSTUM_PLANE_COUNT = 6,
	};

private:

	Plane::type* planes;
	size_t numPlanes;

public:

	Frustum(Plane::type* planes = nullptr, size_t numPlanes = 0);
	~Frustum();

	void SetupPlanes(size_t numPlanes);

	_NexInline size_t GetNumPlanes() const {
		return numPlanes;
	}

	_NexInline Plane::type GetPlane(size_t i) const {
		NEX_ASSERT(i < numPlanes);
		return planes[i];
	}

	_NexInline void SetPlane(Plane::pref plane, size_t i) {
		NEX_ASSERT(i < numPlanes);
		planes[i] = plane;
	}

	/**
	 * @remarks Construct from a Transpose(view*projection) matrix
	 * @param mat Transpose(View*Projection) or Transpose(Proj)*Transpose(View) matrix
	 */
	void ConstructFrom(Mat4::pref mat);

	void SetPlanes(Plane::type* planes, size_t numPlanes) {
		this->planes = planes;
		this->numPlanes = numPlanes;
	}

	const Plane::type* GetPlanes() const {
		return planes;
	}
};
}

#endif //NEXTAR_FRUSTUM_H