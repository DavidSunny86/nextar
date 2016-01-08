//@ Abhishek Dey
//@ Nov 8 2007

#include "physics/cfcconstrain.h"
#include "physics/csimulator.h"

using namespace nextar;
namespace nextar
{
namespace collision
{
extern cContact* gpkContactHolder;
}
}
f32 cFrictionConstrain::sfStabalizer = N3DBaumgarteStabalizer;
cFrictionConstrain::cFrictionConstrain() : mpkC(0),miNum(0)
{
	
}

cFrictionConstrain::~cFrictionConstrain()
{
	if(mpkC) 
		freeMemoryAligned(mpkC);
	mpkC = 0;
}

s32 cFrictionConstrain::getOrder()
{
	s32 iOrder = miNum;
	if(mpkSurf1->mfMu > 0)
		muFlags |= Mu1;
	if(mpkSurf2->mfMu > 0)
		muFlags |= Mu2;
	if(muFlags & (Mu1|Mu2))
		iOrder += miNum*2;
	mfBounce = mpkSurf1->mfBounce + mpkSurf2->mfBounce;
	//mfSoft = mpkSurf1->mfSoftness + mpkSurf2->mfSoftness;
	if(mfBounce > 0)
		muFlags |= Bounce;
	return iOrder;
}

void cFrictionConstrain::initialize(cPhyBody* b1,cPhyBody* b2,cSurface* s1,cSurface *s2,s32 iNum,bool reverse)
{
	X_Assert(iNum > 0);
	X_Assert(iNum <= N3DMaxContact);
	X_Assert(s1 && s2);
	if(b1->isDisabled())
		X_SharedPtr(cSimulator)->enableBody(b1);
	if(b2 && b2->isDisabled())
		X_SharedPtr(cSimulator)->enableBody(b2);
	mpkBody[0] = b1;
	mpkBody[1] = b2;
	mpkSurf1 = s1;mpkSurf2 = s2;
	if(iNum > miNum)
	{
		if(mpkC) freeMemoryAligned(mpkC);
		mpkC = (cContact*)allocMemoryAligned(sizeof(cContact)*iNum);
	}
	miNum = iNum;
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

void cFrictionConstrain::getInfo(info* pkInfo)
{
	vec4_t r1;
	vec4_t r2;
	vec4_t u1,u2;
	u1[3] = u2[3] = 0;
	//f32 fSoft = mfSoft*pkInfo->invstep;
	for(s32 i=miNum-1; i>=0; --i)
	{
		// NP Constrain
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
		pkInfo->bias[0] = sfStabalizer *mpkC[i].depth*pkInfo->invstep;
	//	pkInfo->relax[0] = fSoft;
		// if bodies have bounce use it
		if(muFlags & Bounce)
		{
			f32 outvelocity;
			xVDot(&outvelocity,pkInfo->jptr,mpkBody[0]->mkPdot);
			outvelocity += xVDotR(pkInfo->jptr+4,mpkBody[0]->mkOmega);
			if(mpkBody[1])
			{
				outvelocity += xVDotR(pkInfo->jptr+8,mpkBody[1]->mkPdot)+xVDotR(pkInfo->jptr+12,mpkBody[1]->mkOmega);
			}
			f32 newbias = -outvelocity*mfBounce;
			if(newbias > pkInfo->bias[0]) pkInfo->bias[0] = newbias;
		}
		pkInfo->jptr += 16;
		pkInfo->bod_ptrs += 2;
		//pkInfo->relax++;
		pkInfo->lhi++;
		pkInfo->llo++;
		pkInfo->bias++;

		// tangent 
		if(muFlags & (Mu1|Mu2))
		{
			nOrthoVectors(u1,u2,mpkC[i].norm);
			
			// set body 1
			xQ4CpyMac(pkInfo->jptr,u1);
			xVCross(pkInfo->jptr+4,r1,u1);
			xQ4CpyMac(pkInfo->jptr+16,u2);
			xVCross(pkInfo->jptr+20,r1,u2);

			if(mpkBody[1])
			{
				xQ4ScaleS(pkInfo->jptr+8,u1,-1);
				xVCross(pkInfo->jptr+12,u1,r2);
				xQ4ScaleS(pkInfo->jptr+24,u2,-1);
				xVCross(pkInfo->jptr+28,u2,r2);
			}

			f32 mu1,mu2;
			mu1 = muFlags & Mu1 ? mpkSurf1->mfMu : mpkSurf2->mfMu;
			mu2 = muFlags & Mu2 ? mpkSurf2->mfMu : mpkSurf1->mfMu;

			pkInfo->llo[0] = -mu1;
			pkInfo->lhi[0] =  mu1;
			pkInfo->llo[1] = -mu2;
			pkInfo->lhi[1] =  mu2;

			pkInfo->bias[0] = 0;
			pkInfo->bias[0] = 0;

			pkInfo->bod_ptrs[0] = pkInfo->bod_ptrs[2] = mpkBody[0];
			pkInfo->bod_ptrs[1] = pkInfo->bod_ptrs[3] = mpkBody[1];
						
			//pkInfo->relax[1] = 0;
			//pkInfo->relax[2] = 0;

			pkInfo->jptr += 32;
			pkInfo->bod_ptrs += 4;
			//pkInfo->relax+=2;
			pkInfo->lhi+=2;
			pkInfo->llo+=2;
			pkInfo->bias+=2;
		}
	}
	muFlags &= ~(Bounce|Mu1|Mu2);
}