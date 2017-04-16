
#include <BaseHeaders.h>
#include <UTDefs.h>
#include <ctime>

NEX_IMPLEMENT_PLATFORM_APP()
;
using namespace nextar;

#ifdef NEX_WINDOWS
#	define _XM_NO_INTRINSICS_
#	include <DirectXMath.h>
#	include <DirectXCollision.h>
#endif

const float TEST_TOLERANCE = 0.0001f;

#ifdef NEX_WINDOWS
DirectX::XMMATRIX ToMATRIX(Mat4::pref m1) {
	DirectX::XMMATRIX m(m1.m00, m1.m01, m1.m02, m1.m03,
		m1.m10, m1.m11, m1.m12, m1.m13,
		m1.m20, m1.m21, m1.m22, m1.m23,
		m1.m30, m1.m31, m1.m32, m1.m33);

	return m;
}

Mat4::type ToMatrix4x4(DirectX::CXMMATRIX m1) {

	return Mat4::type(
		m1(0, 0), m1(0, 1), m1(0, 2), m1(0, 3),
		m1(1, 0), m1(1, 1), m1(1, 2), m1(1, 3),
		m1(2, 0), m1(2, 1), m1(2, 2), m1(2, 3),
		m1(3, 0), m1(3, 1), m1(3, 2), m1(3, 3));
}

#endif

bool Equals(float a, float b) {
	return NEX_FLOAT_TOLERANCE_EQUAL(a, b, TEST_TOLERANCE);
}

#ifdef NEX_WINDOWS
Vec4::type ToVector4A(DirectX::XMVECTOR v) {
	return Vec4::Set(DirectX::XMVectorGetX(v), DirectX::XMVectorGetY(v), DirectX::XMVectorGetZ(v), DirectX::XMVectorGetW(v));
}
#endif

bool Equals3(Vec4::pref a, Vec4::pref b) {
	return Equals(Vec4::GetX(a), Vec4::GetX(b)) &&
		Equals(Vec4::GetY(a), Vec4::GetY(b)) &&
		Equals(Vec4::GetZ(a), Vec4::GetZ(b)) ;
}

bool Equals(Vec4::pref a, Vec4::pref b) {
	return Equals(Vec4::GetX(a), Vec4::GetX(b)) &&
		Equals(Vec4::GetY(a), Vec4::GetY(b)) &&
		Equals(Vec4::GetZ(a), Vec4::GetZ(b)) &&
		Equals(Vec4::GetW(a), Vec4::GetW(b));
}

bool Equals(const Mat3::type& m1, const Mat3::type& m2) {
	return Equals(m1.r[0], m2.r[0]) &&
		Equals(m1.r[1], m2.r[1]) &&
		Equals(m1.r[2], m2.r[2]);
}

bool Equals(const Mat4::type& m1, const Mat4::type& m2) {
	return Equals(m1.r[0], m2.r[0]) &&
		Equals(m1.r[1], m2.r[1]) &&
		Equals(m1.r[2], m2.r[2]) &&
		Equals(m1.r[3], m2.r[3]);
}


/******************************************
/* TestVec3A
*******************************************/
void TestVec3A() {

	const float X = 21, Y = 41, Z = 121;
	const float EstimatedDot = X*X + Y*Y + Z*Z;
	Vec3A::type allOne = Vec3A::Set(X, Y, Z);
	Vec3A::type allMOne = Vec3A::Set(-X, -Y, -Z);


	float dotProd = Vec3A::Dot(allOne, allOne);

	NEX_UT_ASSERT(Equals(dotProd, EstimatedDot));

	allOne = Vec3A::Normalize(allOne);
	dotProd = Vec3A::Dot(allOne, allOne);

	NEX_UT_ASSERT(Equals(dotProd, 1));

	float length = Vec3A::SqLength(allOne);
	NEX_UT_ASSERT(Equals(length, 1));

	length = Vec3A::Length(allOne);
	NEX_UT_ASSERT(Equals(length, 1));


	allMOne = Vec3A::Normalize(allMOne);
	NEX_UT_ASSERT(Equals(allOne, Vec3A::Abs(allMOne)));

	NEX_UT_ASSERT(Equals(Vec3A::Add(allOne, allMOne), Vec3A::Zero()));

	length = Vec3A::Length(Vec3A::Cross(allOne, allMOne));
	NEX_UT_ASSERT(Equals(length, 0));

	Vec3A::type tmp = Vec3A::Mul(allOne, allMOne);
	Vec3A::type res1 = Vec3A::Add(tmp, allOne);
	Vec3A::type res2 = Vec3A::Madd(allOne, allMOne, allOne);
	NEX_UT_ASSERT(Equals(res1, res2));

	// @todo Write further tests
}

/******************************************
/* TestVec4
*******************************************/
void TestVec4() {
	const float X = 21, Y = 41, Z = 121, W = 24;
	const float EstimatedDot = X*X + Y*Y + Z*Z + W*W;
	Vec4::type allOne = Vec4::Set(X, Y, Z, W);
	Vec4::type allMOne = Vec4::Set(-X, -Y, -Z, -W);


	float dotProd = Vec4::Dot(allOne, allOne);

	NEX_UT_ASSERT(Equals(dotProd, EstimatedDot));

	allOne = Vec4::Normalize(allOne);
	dotProd = Vec4::Dot(allOne, allOne);

	NEX_UT_ASSERT(Equals(dotProd, 1));

	float length = Vec4::SqLength(allOne);
	NEX_UT_ASSERT(Equals(length, 1));

	length = Vec4::Length(allOne);
	NEX_UT_ASSERT(Equals(length, 1));

	allMOne = Vec4::Normalize(allMOne);
	NEX_UT_ASSERT(Equals(allOne, Vec4::Abs(allMOne)));

	NEX_UT_ASSERT(Equals(Vec4::Add(allOne, allMOne), Vec4::Zero()));

	Vec4::type tmp = Vec4::Mul(allOne, allMOne);
	Vec4::type res1 = Vec4::Add(tmp, allOne);
	Vec4::type res2 = Vec4::Madd(allOne, allMOne, allOne);
	NEX_UT_ASSERT(Equals(res1, res2));

	// @todo Write further tests
}

/******************************************
/* TestQuat
*******************************************/
void TestQuat() {

	float ang;
#ifdef NEX_WINDOWS
	ang = RandomGen::RangeFloat(-Math::PI, Math::PI);
	DirectX::XMVECTOR Axis = { 0, 1, 0 };
	DirectX::XMVECTOR Axis2 = { 1, 0, 0 };
	Quat::type q1 = Quat::FromAxisAngle(Vec3::Set(0, 1, 0), ang);
	DirectX::XMVECTOR q2 = DirectX::XMQuaternionRotationAxis(Axis, ang);
	NEX_UT_ASSERT(Equals(q1, ToVector4A(q2)));

	ang = RandomGen::RangeFloat(-Math::PI, Math::PI);

	Quat::type qm1 = Quat::FromAxisAngle(Vec3::Set(0, 1, 0), ang);
	DirectX::XMVECTOR vqm1 = DirectX::XMQuaternionRotationAxis(Axis, ang);

	NEX_UT_ASSERT(Equals(qm1, ToVector4A(vqm1)));

	ang = RandomGen::RangeFloat(-Math::PI, Math::PI);

	Quat::type qm2 = Quat::FromAxisAngle(Vec3::Set(1, 0, 0), ang);
	DirectX::XMVECTOR vqm2 = DirectX::XMQuaternionRotationAxis(Axis2, ang);

	NEX_UT_ASSERT(Equals(qm2, ToVector4A(vqm2)));

	Quat::type r1 = Quat::Mul(qm1, qm2);
	DirectX::XMVECTOR r2 = DirectX::XMQuaternionMultiply(vqm1, vqm2);
	NEX_UT_ASSERT(Equals(r1, ToVector4A(r2)));

	Quat::type n1 = Quat::Normalize(r1);
	DirectX::XMVECTOR n2 = DirectX::XMQuaternionNormalize(r2);
	NEX_UT_ASSERT(Equals(n1, ToVector4A(n2)));

	// test quat mul accumulation
	ang = RandomGen::RangeFloat(-Math::PI, Math::PI);
	Quat::type qmc = Quat::FromAxisAngle(Vec3::Set(1, 0, 0), ang);
	DirectX::XMVECTOR vqmc = DirectX::XMQuaternionRotationAxis(Axis2, ang);

	ang = RandomGen::RangeFloat(-Math::PI, Math::PI);
	Quat::type accq = Quat::FromAxisAngle(Vec3::Set(0, 1, 0), ang);
	DirectX::XMVECTOR vaccq = DirectX::XMQuaternionRotationAxis(Axis, ang);

	for (int i = 0; i < 20; ++i) {
		accq = Quat::Mul(qmc, accq);
		vaccq = DirectX::XMQuaternionMultiply(vqmc, vaccq);
	}

	NEX_UT_ASSERT(Equals(accq, ToVector4A(vaccq)));

	for (int i = 0; i < 20; ++i) {
		accq = Quat::Mul(accq, qmc);
		vaccq = DirectX::XMQuaternionMultiply(vaccq, vqmc);
	}

	NEX_UT_ASSERT(Equals(accq, ToVector4A(vaccq)));
#endif
}

void TestMatrix3x4() {

}

void TestPerspectiveMatrix() {

	RandomGen::Seed((uint32)std::time(nullptr));
	float fov = RandomGen::RangeFloat(0, Math::PI);
	float aspect = RandomGen::RangeFloat(1.0f, 1.4f);
	float zn = RandomGen::RangeFloat(0.0f, 10.0f);
	float zf = RandomGen::RangeFloat(5000.0f, 10000.0f);
	Mat4::type m1 = Mat4x4::FromPerspectiveProjection(fov, aspect, zn, zf);
	Mat4::type m2;
#ifdef NEX_WINDOWS
	DirectX::XMMATRIX m = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, zn, zf);
	m2 = ToMatrix4x4(m);
#else
	m2 = m1;
#endif
	NEX_UT_ASSERT(Equals(m1, m2));
}

void TestLookAtMatrix() {
	Quat::type rotation = Quat::FromAxisAngle(Vec3::type::XAxis, Math::PI_BY_4);
	Vec3A::type pos = Vec3A::Set(0, 50, -50);
	Mat4::type m1 = Mat4x4::FromScaleRotPos(1, rotation, pos);

	// @todo do a quick test here
	Mat4::type v2 = Mat4x4::FromLookAt(pos, Vec3A::Zero(), Vec3A::Set(0, 1, 0));
	Mat4::type v1 = Mat4x4::FromWorldToView(m1);
#ifdef NEX_WINDOWS
	DirectX::XMVECTOR e = { 0, 50, -50 };
	DirectX::XMVECTOR t = { 0, 0, 0 };
	DirectX::XMVECTOR u = { 0, 1, 0 };
	DirectX::XMMATRIX m = DirectX::XMMatrixLookAtLH(e, t, u);
	Mat4::type v3 = ToMatrix4x4(m);
	NEX_UT_ASSERT(Equals(v3, v2));
#endif
	NEX_UT_ASSERT(Equals(v1, v2));
}

void TestMatrix4x4Mul() {

	RandomGen::Seed(123434);
	Vec3A::type pos = Vec3A::Set(0, 50, -50);
	float fov = RandomGen::RangeFloat(0, Math::PI);
	float aspect = RandomGen::RangeFloat(1.0f, 1.4f);
	float zn = RandomGen::RangeFloat(0.0f, 10.0f);
	float zf = RandomGen::RangeFloat(5000.0f, 10000.0f);
	Mat4::type m1 = Mat4x4::FromPerspectiveProjection(fov, aspect, zn, zf);
	Mat4::type m2 = Mat4x4::FromLookAt(pos, Vec3A::Zero(), Vec3A::Set(0, 1, 0));
	Mat4::type m = Mat4x4::Mul(m2, m1);
	Mat4::type cm = m;
#ifdef NEX_WINDOWS
	DirectX::XMMATRIX xp = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, zn, zf);
	DirectX::XMVECTOR e = { 0, 50, -50 };
	DirectX::XMVECTOR t = { 0, 0, 0 };
	DirectX::XMVECTOR u = { 0, 1, 0 };
	DirectX::XMMATRIX xv = DirectX::XMMatrixLookAtLH(e, t, u);
	DirectX::XMMATRIX  xm = DirectX::XMMatrixMultiply(xv, xp);
	cm = ToMatrix4x4(xm);
#else
#endif
	NEX_UT_ASSERT(Equals(m, cm));
}

void TestMatrix4x4AABoxTransform() {



	Vec3A::type minPoint = Vec3A::Set(RandomGen::RangeFloat(-100, 80), RandomGen::RangeFloat(-100, 70), RandomGen::RangeFloat(-100, 50));
	Vec3A::type maxPoint = Vec3A::Set(RandomGen::RangeFloat(20, 80), RandomGen::RangeFloat(40, 70), RandomGen::RangeFloat(40, 50));
	AABox::type box = AABox::type(minPoint, maxPoint);

	// construct a matrix
	float scale = RandomGen::RangeFloat(1, 2);
	float ang = RandomGen::RangeFloat(-Math::PI, Math::PI);
	Quat::type rotation = Quat::FromAxisAngle(Vec3::Set(1.0f, 0, 0), ang);
	float tx = RandomGen::RangeFloat(-20, 20);
	float ty = RandomGen::RangeFloat(-20, 20);
	float tz = RandomGen::RangeFloat(-20, 20);
	Mat4::type tform = Mat4x4::FromScaleRotPos(scale, rotation, Vec3A::Set(tx, ty, tz));


	AABox::type box1 = Mat4x4::TransformAABox(tform, box);
	// transform by multiplication and combination
	AABox::type box2 = AABox::type::InvalidBox;
	for (unsigned int i = 0; i < 8; ++i) {
		Vec3A::type point = AABox::GetPoint(box, i);
		Vec3A::type tpoint = Mat4x4::TransformOrtho(tform, point);
		box2 = AABox::Union(box2, tpoint);
	}

	NEX_UT_ASSERT(Equals3(box1.minPoint, box2.minPoint) && Equals3(box1.maxPoint, box2.maxPoint));
	// use directx method
#ifdef NEX_WINDOWS
	DirectX::XMVECTOR Axis = { 1, 0, 0 };
	DirectX::XMVECTOR Scaling = { scale, scale, scale };
	DirectX::XMVECTOR RotOrig = { 0, 0, 0 };
	DirectX::XMVECTOR Trans = { tx, ty, tz };
	DirectX::XMVECTOR q2 = DirectX::XMQuaternionRotationAxis(Axis, ang);
	DirectX::XMMATRIX dxtform = DirectX::XMMatrixAffineTransformation(Scaling, RotOrig, q2, Trans);
	NEX_UT_ASSERT(Equals(tform, ToMatrix4x4(dxtform)));
	Vec3A::type c = AABox::GetCenter(box);
	Vec3A::type e = AABox::GetSize(box);
	DirectX::XMFLOAT3 boxCenter = { Vec3A::GetX(c), Vec3A::GetY(c), Vec3A::GetZ(c) };
	DirectX::XMFLOAT3 boxSize = { Vec3A::GetX(e), Vec3A::GetY(e), Vec3A::GetZ(e) };
	DirectX::BoundingBox box3(boxCenter, boxSize);
	DirectX::BoundingBox box4;
	box3.Transform(box4, scale, q2, Trans);
	Vec3A::type box4Max = Vec3A::Set(box4.Center.x + box4.Extents.x, box4.Center.y + box4.Extents.y, box4.Center.z + box4.Extents.z);
	Vec3A::type box4Min = Vec3A::Set(box4.Center.x - box4.Extents.x, box4.Center.y - box4.Extents.y, box4.Center.z - box4.Extents.z);
	NEX_UT_ASSERT(Equals3(box1.minPoint, box4Min) && Equals3(box1.maxPoint, box4Max));
#endif

}

void TestMatrix4x4() {
	TestPerspectiveMatrix();
	TestLookAtMatrix();
	TestMatrix4x4Mul();
	TestMatrix4x4AABoxTransform();
}

void TestMath() {

}

void TestMathFunctions(nextar::ApplicationContext& application) {
	const int TEST_COUNT = 20;
	for (int i = 0; i < TEST_COUNT; ++i) {
		TestVec3A();
		TestVec4();
		TestQuat();
		TestMatrix3x4();
		TestMatrix4x4();
		TestMath();
	}
}

int NextarMain(int argc, char* argv[]) {
	nextar::ACBaseImpl impl;
	nextar::ApplicationContext application("MathTest", impl);
	application.InitializeContext(argc, argv);
	TestMathFunctions(application);
	application.DestroyContext();
	std::cin.get();
	return 0;
}

