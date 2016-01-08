
//@# Abhishek Dey
//@# November 01 2007

#include <creftransform.h>
#include <xalloccache.h>
#include <xmemory.h>

using namespace nextar;

cRefTransform::cRefTransform() : miRefCount(1)
{
	// allocate space for data
	mpfData = (f32*)allocTransform();
	zeroMemory(mpfData,16*sizeof(f32));
	mpfData[0] = mpfData[5] = mpfData[10] = mpfData[15] = 1.f;
}

cRefTransform::cRefTransform(transform* mpkPtr) : miRefCount(1)
{
	// keep the ptr space
	mpfData = (f32*)mpkPtr;
	zeroMemory(mpfData,16*sizeof(f32));
	mpfData[0] = mpfData[5] = mpfData[10] = mpfData[15] = 1.f;
}

cRefTransform::~cRefTransform()
{
	freeTransform((transform*)mpfData);
}