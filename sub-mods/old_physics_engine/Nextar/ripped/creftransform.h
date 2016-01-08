
//@# Abhishek Dey
//@# November 01 2007

#ifndef NEXTAR_REFTRANSFORM_H
#define NEXTAR_REFTRANSFORM_H

#include <xmath_c.h>
#include <xtypes.h>

namespace nextar
{

class cRefTransform : public cBaseClass
{
	s32		miRefCount;
	f32*	mpfData;
public:
	// makes identity transform
	cRefTransform();
	cRefTransform(transform*);
protected:
	~cRefTransform();
public:
	xinline s32  getRefCount() const { return miRefCount; }
	xinline void addRef(s32 cnt) { miRefCount+=cnt; }
	xinline void addRef() { miRefCount++; }
	xinline void release() { 	X_BREAK_IF( miRefCount<=0 ); miRefCount--;	if( !miRefCount ) delete this; }
	xinline f32* operator *() { return mpfData; }
	xinline operator f32*() { return mpfData; }
	xinline operator transform*() { return (transform*)mpfData; }
	xinline transform* operator -> () { return (transform*)mpfData; }
	xinline f32*	   getRot() { return mpfData; }
	xinline f32*	   getPos() { return mpfData + 12; }
};

}

#endif