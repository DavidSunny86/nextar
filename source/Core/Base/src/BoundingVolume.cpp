#include <NexBase.h>
#include <BaseHeaders.h>
#include <BoundingVolume.h>
#include <PooledAllocator.h>

namespace nextar {

const BoundsInfo BoundsInfo::Null;
/*********************************
 * BoundingBox
 *********************************/
BoundingBox::BoundingBox() {
	// largest bv
	extends = center = origCenter = Vec3A::Zero();
	origExtendsAndRadius = Vec4::Zero();
}

void BoundingBox::UpdateBounds(Mat4::pref m) {
	//@todo test which is tighter avro's bound transform or this one
	center = Mat4::TransformOrtho(m, origCenter);
	extends = Mat4::TransformBounds(m, Vec3A::From(origExtendsAndRadius));
}

void BoundingBox::UpdateBounds(float scale, Quat::pref rot, Vec3A::pref pos) {
	//@todo test which is tighter avro's bound transform or this one
	center = Vec3A::Add(Quat::Transform(rot, origCenter), pos);
	extends = Quat::TransformBounds(rot, Vec3A::Mul(origExtendsAndRadius, scale));
}

void BoundingBox::SetNull() {
	center = origCenter = extends = Vec3A::Zero();
	origExtendsAndRadius = Vec4::Zero();
}

void BoundingBox::SetVolume(Vec3A::pref center, Vec3A::pref extends) {
	this->center = this->origCenter = center;
	this->extends = this->origExtendsAndRadius = extends;
	this->origExtendsAndRadius = Vec4::SetW(origExtendsAndRadius, Vec3A::Length(Vec3A::Mul(extends, 2.0f)));
}

void BoundingBox::SetVolume(Vec3A::pref center, Vec3A::pref extends, float radius) {
	this->center = this->origCenter = center;
	this->extends = this->origExtendsAndRadius = extends;
	this->origExtendsAndRadius = Vec4::SetW(origExtendsAndRadius, radius);
}

void BoundingBox::UpdateBounds(const Vec3A::type* pt, uint32 numPts) {
	if (numPts) {
		// Costly calculate to encompass all points 
		AABox::type box = AABox::FromCenterExtends(center, extends);
		for (uint32 i = 1; i < numPts; i++) {
			box = AABox::Union(box, pt[i]);
		}
		SetVolume(AABox::GetCenter(box), AABox::GetHalfSize(box));
	}
}

/*********************************
 * BoundingVolume
 *********************************/
BoundingVolume::BoundingVolume() {
	box = NEX_NEW(BoundingBox);
}

BoundingVolume::~BoundingVolume() {
	NEX_DELETE((box));
}

void BoundingVolume::MergeBounds(const BoundingVolume& vol) {
	Vec3A::type a = Vec3A::Abs(Vec3A::Sub(box->center, vol.box->center));
	Vec3A::type b = Vec3A::Add(box->center, vol.box->center);
	box->center = box->origCenter = Vec3A::Mul(b, 0.5f);
	box->extends = box->origExtendsAndRadius = Vec3A::Add(a,
			Vec3A::Add(vol.box->extends, box->extends));
	box->radius += (vol.box->radius + Vec3A::Dot(a, a));
}

void BoundingVolume::SetVolume(Vec3A::pref center, Vec3A::pref extends, float radius) {
	box->SetVolume(center, extends);
	box->radius = radius;
}

}
