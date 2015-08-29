#include <BaseHeaders.h>
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
DirectX::XMMATRIX ToMATRIX(const Matrix4x4& m1) {
	DirectX::XMMATRIX m(m1.m00, m1.m01, m1.m02, m1.m03,
		m1.m10, m1.m11, m1.m12, m1.m13,
		m1.m20, m1.m21, m1.m22, m1.m23,
		m1.m30, m1.m31, m1.m32, m1.m33);

	return m;
}

Matrix4x4 ToMatrix4x4(DirectX::CXMMATRIX m1) {

	return Matrix4x4(
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
Vector4A ToVector4A(DirectX::XMVECTOR v) {
	return Vec4ASet(DirectX::XMVectorGetX(v), DirectX::XMVectorGetY(v), DirectX::XMVectorGetZ(v), DirectX::XMVectorGetW(v));
}
#endif

bool Equals3(Vec4AF a, Vec4AF b) {
	return Equals(Vec4AGetX(a), Vec4AGetX(b)) &&
		Equals(Vec4AGetY(a), Vec4AGetY(b)) &&
		Equals(Vec4AGetZ(a), Vec4AGetZ(b)) ;
}

bool Equals(Vec4AF a, Vec4AF b) {
	return Equals(Vec4AGetX(a), Vec4AGetX(b)) &&
		Equals(Vec4AGetY(a), Vec4AGetY(b)) &&
		Equals(Vec4AGetZ(a), Vec4AGetZ(b)) &&
		Equals(Vec4AGetW(a), Vec4AGetW(b));
}

bool Equals(const Matrix3x4& m1, const Matrix3x4& m2) {
	return Equals(m1.r[0], m2.r[0]) &&
		Equals(m1.r[1], m2.r[1]) &&
		Equals(m1.r[2], m2.r[2]);
}

bool Equals(const Matrix4x4& m1, const Matrix4x4& m2) {
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
	Vector3A allOne = Vec3ASet(X, Y, Z);
	Vector3A allMOne = Vec3ASet(-X, -Y, -Z);


	float dotProd = Vec3ADot(allOne, allOne);

	NEX_ASSERT(Equals(dotProd, EstimatedDot));

	allOne = Vec3ANormalize(allOne);
	dotProd = Vec3ADot(allOne, allOne);

	NEX_ASSERT(Equals(dotProd, 1));

	float length = Vec3ASqLength(allOne);
	NEX_ASSERT(Equals(length, 1));

	length = Vec3ALength(allOne);
	NEX_ASSERT(Equals(length, 1));


	allMOne = Vec3ANormalize(allMOne);
	NEX_ASSERT(Equals(allOne, Vec3AAbs(allMOne)));

	NEX_ASSERT(Equals(Vec3AAdd(allOne, allMOne), Vec3AZero()));

	length = Vec3ALength(Vec3ACross(allOne, allMOne));
	NEX_ASSERT(Equals(length, 0));

	Vector3A tmp = Vec3AMul(allOne, allMOne);
	Vector3A res1 = Vec3AAdd(tmp, allOne);
	Vector3A res2 = Vec3AMulAdd(allOne, allMOne, allOne);
	NEX_ASSERT(Equals(res1, res2));

	// @todo Write further tests
}

/******************************************
/* TestVec4A
*******************************************/
void TestVec4A() {
	const float X = 21, Y = 41, Z = 121, W = 24;
	const float EstimatedDot = X*X + Y*Y + Z*Z + W*W;
	Vector4A allOne = Vec4ASet(X, Y, Z, W);
	Vector4A allMOne = Vec4ASet(-X, -Y, -Z, -W);


	float dotProd = Vec4ADot(allOne, allOne);

	NEX_ASSERT(Equals(dotProd, EstimatedDot));

	allOne = Vec4ANormalize(allOne);
	dotProd = Vec4ADot(allOne, allOne);

	NEX_ASSERT(Equals(dotProd, 1));

	float length = Vec4ASqLength(allOne);
	NEX_ASSERT(Equals(length, 1));

	length = Vec4ALength(allOne);
	NEX_ASSERT(Equals(length, 1));

	allMOne = Vec4ANormalize(allMOne);
	NEX_ASSERT(Equals(allOne, Vec4AAbs(allMOne)));

	NEX_ASSERT(Equals(Vec4AAdd(allOne, allMOne), Vec4AZero()));

	Vector4A tmp = Vec4AMul(allOne, allMOne);
	Vector4A res1 = Vec4AAdd(tmp, allOne);
	Vector4A res2 = Vec4AMulAdd(allOne, allMOne, allOne);
	NEX_ASSERT(Equals(res1, res2));

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
	Quaternion q1 = QuatFromAxisAng(Vec3ASet(0, 1, 0), ang);
	DirectX::XMVECTOR q2 = DirectX::XMQuaternionRotationAxis(Axis, ang);
	NEX_ASSERT(Equals(q1, ToVector4A(q2)));

	ang = RandomGen::RangeFloat(-Math::PI, Math::PI);

	Quaternion qm1 = QuatFromAxisAng(Vec3ASet(0, 1, 0), ang);
	DirectX::XMVECTOR vqm1 = DirectX::XMQuaternionRotationAxis(Axis, ang);

	NEX_ASSERT(Equals(qm1, ToVector4A(vqm1)));

	ang = RandomGen::RangeFloat(-Math::PI, Math::PI);

	Quaternion qm2 = QuatFromAxisAng(Vec3ASet(1, 0, 0), ang);
	DirectX::XMVECTOR vqm2 = DirectX::XMQuaternionRotationAxis(Axis2, ang);

	NEX_ASSERT(Equals(qm2, ToVector4A(vqm2)));

	Quaternion r1 = QuatMul(qm1, qm2);
	DirectX::XMVECTOR r2 = DirectX::XMQuaternionMultiply(vqm1, vqm2);
	NEX_ASSERT(Equals(r1, ToVector4A(r2)));

	Quaternion n1 = QuatNormalize(r1);
	DirectX::XMVECTOR n2 = DirectX::XMQuaternionNormalize(r2);
	NEX_ASSERT(Equals(n1, ToVector4A(n2)));

	// test quat mul accumulation
	ang = RandomGen::RangeFloat(-Math::PI, Math::PI);
	Quaternion qmc = QuatFromAxisAng(Vec3ASet(1, 0, 0), ang);
	DirectX::XMVECTOR vqmc = DirectX::XMQuaternionRotationAxis(Axis2, ang);

	ang = RandomGen::RangeFloat(-Math::PI, Math::PI);
	Quaternion accq = QuatFromAxisAng(Vec3ASet(0, 1, 0), ang);
	DirectX::XMVECTOR vaccq = DirectX::XMQuaternionRotationAxis(Axis, ang);

	for (int i = 0; i < 20; ++i) {
		accq = QuatMul(qmc, accq);
		vaccq = DirectX::XMQuaternionMultiply(vqmc, vaccq);
	}

	NEX_ASSERT(Equals(accq, ToVector4A(vaccq)));

	for (int i = 0; i < 20; ++i) {
		accq = QuatMul(accq, qmc);
		vaccq = DirectX::XMQuaternionMultiply(vaccq, vqmc);
	}

	NEX_ASSERT(Equals(accq, ToVector4A(vaccq)));
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
	Matrix4x4 m1 = Mat4x4FromPerspective(fov, aspect, zn, zf);
	Matrix4x4 m2;
#ifdef NEX_WINDOWS
	DirectX::XMMATRIX m = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, zn, zf);
	m2 = ToMatrix4x4(m);
#else
	m2 = m1;
#endif
	NEX_ASSERT(Equals(m1, m2));
}

void TestLookAtMatrix() {
	Quaternion rotation = QuatFromAxisAng(Vector3::XAxis, Math::PI_BY_4);
	Vector3A pos = Vec3ASet(0, 50, -50);
	Matrix4x4 m1 = Mat4x4FromScaleRotPos(1, rotation, pos);

	// @todo do a quick test here
	Matrix4x4 v2 = Mat4x4FromCameraLookAt(pos, Vec3AZero(), Vec3ASet(0, 1, 0));
	Matrix4x4 v1 = Mat4x4ViewFromWorld(m1);
#ifdef NEX_WINDOWS
	DirectX::XMVECTOR e = { 0, 50, -50 };
	DirectX::XMVECTOR t = { 0, 0, 0 };
	DirectX::XMVECTOR u = { 0, 1, 0 };
	DirectX::XMMATRIX m = DirectX::XMMatrixLookAtLH(e, t, u);
	Matrix4x4 v3 = ToMatrix4x4(m);
	NEX_ASSERT(Equals(v3, v2));
#endif
	NEX_ASSERT(Equals(v1, v2));
}

void TestMatrix4x4Mul() {

	RandomGen::Seed(123434);
	Vector3A pos = Vec3ASet(0, 50, -50);
	float fov = RandomGen::RangeFloat(0, Math::PI);
	float aspect = RandomGen::RangeFloat(1.0f, 1.4f);
	float zn = RandomGen::RangeFloat(0.0f, 10.0f);
	float zf = RandomGen::RangeFloat(5000.0f, 10000.0f);
	Matrix4x4 m1 = Mat4x4FromPerspective(fov, aspect, zn, zf);
	Matrix4x4 m2 = Mat4x4FromCameraLookAt(pos, Vec3AZero(), Vec3ASet(0, 1, 0));
	Matrix4x4 m = Mat4x4Mul(m2, m1);
	Matrix4x4 cm = m;
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
	NEX_ASSERT(Equals(m, cm));
}

void TestMatrix4x4AABoxTransform() {



	Vector3A minPoint = Vec3ASet(RandomGen::RangeFloat(-100, 80), RandomGen::RangeFloat(-100, 70), RandomGen::RangeFloat(-100, 50));
	Vector3A maxPoint = Vec3ASet(RandomGen::RangeFloat(20, 80), RandomGen::RangeFloat(40, 70), RandomGen::RangeFloat(40, 50));
	AxisAlignedBox box = AxisAlignedBox(minPoint, maxPoint);

	// construct a matrix
	float scale = RandomGen::RangeFloat(1, 2);
	float ang = RandomGen::RangeFloat(-Math::PI, Math::PI);
	Quaternion rotation = QuatFromAxisAng(Vector3(1, 0, 0), ang);
	float tx = RandomGen::RangeFloat(-20, 20);
	float ty = RandomGen::RangeFloat(-20, 20);
	float tz = RandomGen::RangeFloat(-20, 20);
	Matrix4x4 tform = Mat4x4FromScaleRotPos(scale, rotation, Vec3ASet(tx, ty, tz));


	AxisAlignedBox box1 = Mat4x4TransAABox(box, tform);
	// transform by multiplication and combination
	AxisAlignedBox box2 = AxisAlignedBox::InvalidBox;
	for (unsigned int i = 0; i < 8; ++i) {
		Vector3A point = AABoxGetPoint(box, i);
		Vector3A tpoint = Mat4x4TransVec3A(point, tform);
		box2 = AABoxUnion(box2, tpoint);
	}

	NEX_ASSERT(Equals3(box1.minPoint, box2.minPoint) && Equals3(box1.maxPoint, box2.maxPoint));
	// use directx method
#ifdef NEX_WINDOWS
	DirectX::XMVECTOR Axis = { 1, 0, 0 };
	DirectX::XMVECTOR Scaling = { scale, scale, scale };
	DirectX::XMVECTOR RotOrig = { 0, 0, 0 };
	DirectX::XMVECTOR Trans = { tx, ty, tz };
	DirectX::XMVECTOR q2 = DirectX::XMQuaternionRotationAxis(Axis, ang);
	DirectX::XMMATRIX dxtform = DirectX::XMMatrixAffineTransformation(Scaling, RotOrig, q2, Trans);
	NEX_ASSERT(Equals(tform, ToMatrix4x4(dxtform)));
	Vector3A c = AABoxGetCenter(box);
	Vector3A e = AABoxGetSize(box);
	DirectX::XMFLOAT3 boxCenter = { Vec3AGetX(c), Vec3AGetY(c), Vec3AGetZ(c) };
	DirectX::XMFLOAT3 boxSize = { Vec3AGetX(e), Vec3AGetY(e), Vec3AGetZ(e) };
	DirectX::BoundingBox box3(boxCenter, boxSize);
	DirectX::BoundingBox box4;
	box3.Transform(box4, scale, q2, Trans);
	Vector3A box4Max = Vec3ASet(box4.Center.x + box4.Extents.x, box4.Center.y + box4.Extents.y, box4.Center.z + box4.Extents.z);
	Vector3A box4Min = Vec3ASet(box4.Center.x - box4.Extents.x, box4.Center.y - box4.Extents.y, box4.Center.z - box4.Extents.z);
	NEX_ASSERT(Equals3(box1.minPoint, box4Min) && Equals3(box1.maxPoint, box4Max));
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
		TestVec4A();
		TestQuat();
		TestMatrix3x4();
		TestMatrix4x4();
		TestMath();
	}
}

int NextarMain(int argc, char* argv[]) {
	nextar::ApplicationContext application("MathTest");
	application.InitializeContext(argc, argv);
	TestMathFunctions(application);
	application.DestroyContext();
	std::cin.get();
	return 0;
}

