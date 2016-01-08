//@ Abhishek Dey
//@ Nov 8 2007

#ifndef NEXTAR_XPCONSTRAIN
#define NEXTAR_XPCONSTRAIN

#include "physics/cphybody.h"

namespace nextar
{
#define N3DBaumgarteStabalizer 0.5f
struct cConstrain
{
	enum Type {
		BasicNPConstrain = 0, // basic non penetration constrain
		BasicFCConstrain = 0, // the frictional contact model
	};
	enum {
		Active = 1 << 0,
		LastFlag = 1 << 1,
	};
	struct info {
		f32			invstep;
		f32*		jptr;
		f32*		bias;
		f32*		lhi;
		f32*		llo;
		//f32*		relax;
		cPhyBody**	bod_ptrs;
	};

	cPhyBody   *mpkBody[2];
	u32			muFlags;
	cConstrain*	mpkNext;
	cConstrain*	mpkPrev;

	_n_inline bool isDisabled() { return !(muFlags & Active); }
	_n_inline bool isEnabled() { return (muFlags & Active); }
	_n_inline void enable() { muFlags |= Active; }
	_n_inline void disable() { muFlags &= ~Active; }
	virtual s32  getOrder() = 0;	// returns the order
	virtual void getInfo(info*) = 0;
	virtual void destroy() {}
	cConstrain();
	virtual ~cConstrain() { }
};

}

#endif //NEXTAR_XPCONSTRAIN