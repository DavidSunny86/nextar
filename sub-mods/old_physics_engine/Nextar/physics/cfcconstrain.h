//@ Abhishek Dey
//@ Nov 8 2007

#ifndef NEXTAR_FRICTIONALCONTACT_CONSTRAIN
#define NEXTAR_FRICTIONALCONTACT_CONSTRAIN

#include "physics/cconstrain.h"
#include "collision/xcontact.h"

namespace nextar
{

class cFrictionConstrain : public cConstrain
{
	enum {
		Bounce = cConstrain::LastFlag << 0,
		Mu1 = cConstrain::LastFlag << 1,
		Mu2 = cConstrain::LastFlag << 2,
	};
public:
	cFrictionConstrain();
	~cFrictionConstrain();
	void initialize(cPhyBody* b1,cPhyBody* b2,cSurface* s1,cSurface* s2,s32 iNum,bool reverse = false);
	virtual s32 getOrder();
	virtual void getInfo(info*);

	static _inline void setStabalizer(f32 v) { sfStabalizer = v; }
	static _inline f32	getStabalizer() { return sfStabalizer; }
	static f32			sfStabalizer;
	cContact			*mpkC;
	s32					miNum;
	f32					mfBounce;
	//f32					mfSoft;
	cSurface			*mpkSurf1,*mpkSurf2;
};

}

#endif //NEXTAR_FRICTIONALCONTACT_CONSTRAIN