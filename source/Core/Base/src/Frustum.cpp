/* 
 * File:   frustum.cpp
 * Author: obhi
 *
 * Created on January 24, 2011, 2:54 PM
 */
#include <BaseHeaders.h>
#include <Frustum.h>

namespace nextar {

Frustum::Frustum(Plane::type* _planes, size_t num) :
		planes(_planes), numPlanes(num) {
	if (!planes && num > 0)
		SetupPlanes(num);
}

Frustum::~Frustum() {
	if (planes)
		NEX_FREE(planes, MEMCAT_MATH_CORE);
}

void Frustum::SetupPlanes(size_t num) {
	if (planes) {
		if (numPlanes == num)
			return;
		else
			NEX_FREE(planes, MEMCAT_MATH_CORE);
	}
	numPlanes = num;
	planes = static_cast<Plane::type*>(NEX_ALLOC(num * sizeof(Plane::type),
			MEMCAT_MATH_CORE));
}

void Frustum::ConstructFrom(Mat4::pref combo) {
	// Left clipping planeT
	planes[PLANE_LEFT] = PlaneNormalize(
	Vec4ANegate(Vec4AAdd(Mat4x4Row(combo, 0), Mat4x4Row(combo, 3))));
	// Right clipping planeT
	planes[PLANE_RIGHT] = PlaneNormalize(
	Vec4ASub(Mat4x4Row(combo, 0), Mat4x4Row(combo, 3)));
	// Top clipping planeT
	planes[PLANE_TOP] = PlaneNormalize(
	Vec4ASub(Mat4x4Row(combo, 1), Mat4x4Row(combo, 3)));
	// Bottom clipping planeT
	planes[PLANE_BOTTOM] = PlaneNormalize(
	Vec4ANegate(Vec4AAdd(Mat4x4Row(combo, 1), Mat4x4Row(combo, 3))));
	// Near clipping planeT
	planes[PLANE_NEAR] = PlaneNormalize(Vec4ANegate(Mat4x4Row(combo, 2)));
	// Far clipping planeT
	planes[PLANE_FAR] = PlaneNormalize(
	Vec4ASub(Mat4x4Row(combo, 2), Mat4x4Row(combo, 3)));
}
}

