#include <BaseHeaders.h>

NEX_IMPLEMENT_PLATFORM_APP()
;
using namespace nextar;

const float TEST_TOLERANCE = 0.0001f;

bool Equals(float a, float b) {
	return NEX_FLOAT_TOLERANCE_EQUAL(a, b, TEST_TOLERANCE);
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
	// @todo Write further tests
}

void TestMatrix3x4() {

}

void TestMatrix4x4() {

}

void TestMath() {

}

void TestMathFunctions(nextar::ApplicationContext& application) {

	TestVec3A();
	TestVec4A();
	TestQuat();
	TestMatrix3x4();
	TestMatrix4x4();
	TestMath();
}

int NextarMain(int argc, char* argv[]) {
	nextar::ApplicationContext application("FileSystem");
	application.InitializeContext(argc, argv);
	TestMathFunctions(application);
	application.DestroyContext();
	std::cin.get();
	return 0;
}

