//@ Abhishek Dey
//@ Nov 11 2007

//@ this will get rid of the fixed stack that
//@ i was using for contact type constrains
//@ for group deallocation and deinitialization.
//@ these constrains are created and destroyed
//@ in possibly one step. The elements in the
//@ stack are not added to the system constrain 
//@ queue.

#include "xtypes.h"
#ifdef X_DEBUG
#include "xmemory.h"
#endif

namespace nextar
{

//@ WARNING: on call to destroymemory the stack will
//@ try to delete all the valid ptrs that was allocated
//@ before.
template < typename Constrain >
class cConstrainStack : public cBaseClass
{
	Constrain** mpkStack;
	s32			miAllocated;
	s32			miAllocConstrains;
	s32			miActive;
public:
	
	_n_inline Constrain* operator [] (s32 idx) { X_BreakOutIf(idx>=miActive); return mpkStack[idx]; }
	_n_inline s32 size() { return miActive; }
	
	cConstrainStack(s32 initalsize) : mpkStack(0),miActive(0),miAllocated(0) {	_resize(initalsize);	}
	_n_inline ~cConstrainStack() { destroyMemory(); }

	_n_inline Constrain*	alloc() { 
		if(miActive>=miAllocConstrains)
		{
			if(miActive>=miAllocated)
				_resize(miAllocated+1);
			_createElements(miAllocConstrains+1);
		}
		return mpkStack[miActive++];
	}
	_n_inline void destroy() {
		// we do not call the constrain destroy function
		// currently its useless
		miActive = 0;
	}
	void destroyMemory() {
		if(miAllocated && mpkStack)
		{
			X_Assert(miAllocated>0);
			for(s32 i=0; i < miAllocConstrains ; i++)
			{
				X_Assert(mpkStack[i])
				mpkStack[i]->destroy();
				delete mpkStack[i];
			}
			delete [] mpkStack;
			mpkStack = 0;
			miActive = miAllocated = 0;
		}
	}
private:
	void _destroyElements(s32 from) {
		for(s32 i=from; i<miAllocConstrains; i++)
		{
			mpkStack[i]->destroy();
			delete mpkStack[i];
			mpkStack[i] = 0;
		}
	}
	void _createElements(s32 till) {
		X_Assert(till>=miAllocConstrains);
		for(s32 i=miAllocConstrains; i<till; i++)
			mpkStack[i] = new Constrain();
		miAllocConstrains = till;
	}
	void _resize(s32 count) {
		X_Assert(count>0);
		X_Assert(count!=miAllocated);
		Constrain** pkOldData = mpkStack;
		mpkStack = new Constrain*[count];
#ifdef X_DEBUG
		storeDwords(mpkStack,count,0);
#endif
		s32 copyamt = miAllocated;
		if(count<miAllocated)
		{
			copyamt = count;
			_destroyElements(count);
		}
		for (s32 i=0; i<copyamt; ++i)
			mpkStack[i] = pkOldData[i];
		miAllocated = count;
		if (miAllocated < miActive)
			miActive = miAllocated;
		if( pkOldData ) 
			delete [] pkOldData;
	}
};

}
