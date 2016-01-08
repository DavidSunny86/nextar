//@ Abhishek Dey
//@ Nov 8 2007

#ifndef NEXTAR_NON_PENETRATION_CONSTRAIN
#define NEXTAR_NON_PENETRATION_CONSTRAIN

#include "physics/cconstrain.h"
#include "collision/xcontact.h"

namespace nextar
{

struct cContactConstrain : public cConstrain
{
	void initialize(cPhyBody* b1,cPhyBody* b2,s32 iNum,bool reverse = false);
	virtual s32 getOrder();
	virtual void getInfo(info*);
	cContactConstrain();
	virtual ~cContactConstrain();
	// data
	cContact	*mpkC;
//	f32			mfSoft;
	s32			miNum;
};

}

#endif //NEXTAR_NON_PENETRATION_CONSTRAIN