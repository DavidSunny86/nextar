//@ Abhishek Dey
//@ Nov 8 2007

#include "physics/cnpconstrain.h"
#include "physics/csimulator.h"

using namespace nextar;

namespace nextar
{
namespace collision
{
extern cContact* gpkContactHolder;
}
}
cContactConstrain::cContactConstrain() : mpkC(0),miNum(0)
{
}

cContactConstrain::~cContactConstrain()
{
	if(mpkC) 
		freeMemoryAligned(mpkC);
	mpkC = 0;
}

s32 cContactConstrain::getOrder()
{
	return miNum;
}

void cContactConstrain::getInfo(nextar::cConstrain::info *pkInfo)
{
	vec4_t r1,r2;
	for(s32 i=miNum-1; i>=0; --i)
	{
		xQ4CpyMac(pkInfo->jptr,mpkC[i].norm);
		// r1 = p-x1 r2 = p-x2
		xQ4Sub(r1,mpkC[i].pos,mpkBody[0]->mkWorld.pos);
		xVCross(pkInfo->jptr+4,r1,mpkC[i].norm);
		if(mpkBody[1])
		{
			xQ4ScaleS(pkInfo->jptr+8,mpkC[i].norm,-1.0f);
			xQ4Sub(r2,mpkC[i].pos,mpkBody[1]->mkWorld.pos);
			xVCross(pkInfo->jptr+12,mpkC[i].norm,r2);
		}
		pkInfo->llo[0] = 0;
		pkInfo->lhi[0] = N3DInfinity;
		pkInfo->bod_ptrs[0] = mpkBody[0];
		pkInfo->bod_ptrs[1] = mpkBody[1];
		pkInfo->bias[0] = N3DBaumgarteStabalizer*mpkC[i].depth*pkInfo->invstep;
		//pkInfo->relax[0] = mfSoft*pkInfo->invstep;
		// if bod 1 has bounce use it
		pkInfo->jptr += 16;
		pkInfo->bod_ptrs += 2;
//		pkInfo->relax++;
		pkInfo->lhi++;
		pkInfo->llo++;
		pkInfo->bias++;
	}
}

void cContactConstrain::initialize(nextar::cPhyBody *b1, nextar::cPhyBody *b2,s32 iNum,bool reverse)
{
	X_Assert(iNum > 0);
	X_Assert(iNum <= N3DMaxContact);
	if(b1->isDisabled())
		X_SharedPtr(cSimulator)->enableBody(b1);
	if(b2 && b2->isDisabled())
		X_SharedPtr(cSimulator)->enableBody(b2);
	mpkBody[0] = b1;
	mpkBody[1] = b2;
	if(iNum > miNum)
	{
		if(mpkC) freeMemoryAligned(mpkC);
		mpkC = (cContact*)allocMemoryAligned(sizeof(cContact)*iNum);
	}
	miNum = iNum;
	//mfSoft = fSoftness;
	if(!reverse)
	{
		for(s32 i=0; i<miNum; i++)
		{
			xQ4CpyMac(mpkC[i].norm,collision::gpkContactHolder[i].norm);
			xQ4CpyMac(mpkC[i].pos,collision::gpkContactHolder[i].pos);
			mpkC[i].depth = collision::gpkContactHolder[i].depth;
		}
	}
	else
	{
		for(s32 i=0; i<miNum; i++)
		{
			xQ4ScaleS(mpkC[i].norm,collision::gpkContactHolder[i].norm,-1.0f);
			xQ4CpyMac(mpkC[i].pos,collision::gpkContactHolder[i].pos);
			mpkC[i].depth = collision::gpkContactHolder[i].depth;
		}
	}
}
