//@ Abhishek Dey
//@ Nov 08 2007

#include "physics/csimulator.h"
#include "physics/cnpconstrain.h"
#include "collision/icollisionobject.h"
#include "xmemory.h"
#include "physics/cconstrainstack.h"

using namespace nextar;
const s32 N3DMaxConstrainStack = 128;
//@ static variables
//static vec4_t		skGravity = {0,0,-0.5f};
static vec4_t		skGravity = {0,-9.8f,0};
static f32*			spfMemoryChunk = 0;
static f32*			spfJsp = 0;			// s x 16 vector
static f32*			spfBsp	 = 0;		// s x 16 vector
static f32*			spfLambda = 0;		// s x 1 vector
static f32*			spfHi = 0;			// s x 1 vector
static f32*			spfLo = 0;			// s x 1 vector
static f32*			spfBias = 0;		// s x 1 vector
static f32*			spfD	= 0;		// s x 1 vector
static f32*			spfRhs	= 0;		// s x 1 vector
//static f32*			spfCDot = 0;		// s x 1 vector
//static f32*			spfRelax  = 0;
static cPhyBody**	spkJmap = 0;		// s x 2 vector
static s32			siStackOrder = 0;
static s32			siOrder = 0;
static cConstrainStack<cContactConstrain> skNPConstrainStack(N3DMaxConstrainStack);
static cConstrainStack<cFrictionConstrain> skFCConstrainStack(N3DMaxConstrainStack);

//@ macros
#define REALLOC_CACHE(type,ptr,num) { \
	type* t002_##ptr = (type*)reallocMemory(ptr,(num*sizeof(type)));\
	if(!t002_##ptr) { if(ptr) freeMemory(ptr); ptr=(type*)allocMemory(num*sizeof(type)); }\
	else ptr = t002_##ptr;\
	}

#define REALLOC_CACHE_ALIGNED(type,ptr,num) { \
	type* t001_##ptr = (type*)reallocMemoryAligned(ptr,(num*sizeof(type)));\
	if(!t001_##ptr) { if(ptr) freeMemoryAligned(ptr); ptr=(type*)allocMemoryAligned((num*sizeof(type))); }\
	else ptr = t001_##ptr;\
	}

static _n_inline void	_ComputeCDot();
static _n_inline void	_ComputeFc();
static _n_inline void	_ComputeBsp();
static _n_inline void _ComputeRhs(f32 invdt);
static _n_inline void	_SolveLCP();


SharedSingleInterfaceSetup(cSimulator);
#ifdef X_SSE_INCLUDE
_n_inline void _mul0_344_add(f32* v,f32* a,f32* r)
{
	__asm 
	{
		mov		eax, a
		movaps	xmm0,[eax]
		mov		eax, r
		movaps	xmm1,xmm0
		mulps	xmm1,[eax]
		movaps	xmm2,xmm0
		mulps	xmm2,[eax+16]
		mulps	xmm0,[eax+32]
		mov		eax, [v]

		movaps	xmm3,xmm1
		shufps	xmm3,xmm1,0FDh
		addss	xmm3,xmm1
		shufps	xmm1,xmm1,0FEh
		addss	xmm3,xmm1
		addss	xmm3,[eax]
		movss	[eax],xmm3

		movaps	xmm3,xmm2
		shufps	xmm3,xmm2,0FDh
		addss	xmm3,xmm2
		shufps	xmm2,xmm2,0FEh
		addss	xmm3,xmm2
		addss	xmm3,[eax+4]
		movss	[eax+4],xmm3

		movaps	xmm3,xmm0
		shufps	xmm3,xmm0,0FDh
		addss	xmm3,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm3,xmm0
		addss	xmm3,[eax+8]
		movss	[eax+8],xmm3
	}
}

_n_inline void _compute_tmp_bod(f32* arr,f32* lvel,f32 *avel,f32* facc,
							  f32* tacc,f32* invi,f32 invmass,f32 invdt)
{
	__asm 
	{
		movss	xmm0, invdt
		movss	xmm1, invmass
		shufps	xmm0,xmm0,0
		shufps	xmm1,xmm1,0
		mov		eax, facc
		movaps	xmm2, [eax]
		mulps	xmm2, xmm1
		mov		eax, lvel
		movaps  xmm3, [eax]
		mulps	xmm3, xmm0
		addps	xmm3,xmm2
		mov		eax, arr
		mov		ecx, eax
		movaps	[eax],xmm3
		mov		eax, avel
		movaps	xmm3,[eax]
		mulps	xmm3,xmm0

		mov		eax, tacc
		movaps	xmm0, [eax]
		movaps	xmm1,xmm0
		mov		eax, invi
		mulps	xmm1,[eax]
		movaps	xmm2,xmm1
		shufps	xmm2,xmm1,0FDh
		addss	xmm2,xmm1
		shufps	xmm1,xmm1,0FEh
		addss	xmm2,xmm1
		addss	xmm2,xmm3
		movss	[ecx+16],xmm2
		shufps	xmm3,xmm3,0F9h

		movaps	xmm1,xmm0
		mulps	xmm1,[eax+16]
		movaps	xmm2,xmm1
		shufps	xmm2,xmm1,0FDh
		addss	xmm2,xmm1
		shufps	xmm1,xmm1,0FEh
		addss	xmm2,xmm1
		addss	xmm2,xmm3
		movss	[ecx+20],xmm2
		shufps	xmm3,xmm3,0F9h
		
		movaps	xmm1,xmm0
		mulps	xmm1,[eax+32]
		movaps	xmm2,xmm1
		shufps	xmm2,xmm1,0FDh
		addss	xmm2,xmm1
		shufps	xmm1,xmm1,0FEh
		addss	xmm2,xmm1
		addss	xmm2,xmm3
		movss	[ecx+24],xmm2
	}
}
_n_inline f32 _velocity_dot(f32* dotp,f32* lv,f32* avel)
{
	f32 returnval;
	__asm
	{
		mov		eax, dotp
		movaps  xmm0, xmmword ptr[eax]
		movaps  xmm1, xmmword ptr[eax+16]
		mov		eax, lv
		mulps	xmm0, xmmword ptr[eax] 
		mov		eax, avel
		mulps	xmm1, xmmword ptr[eax] 
		addps	xmm0, xmm1
		movaps  xmm1,xmm0
		shufps	xmm1,xmm0,0FDh			;xmm1 = shuffle(xmm0,xmm0)
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		movss	returnval, xmm1
	}
	return returnval;
}
_n_inline f32 _8dot(f32* ptr1,f32* ptr2)
{
	f32 returnval;
	__asm
	{
		mov		eax, ptr1
		movaps  xmm0, xmmword ptr[eax]
		movaps  xmm1, xmmword ptr[eax+16]
		mov		eax, ptr2
		mulps	xmm0, xmmword ptr[eax] 
		mulps	xmm1, xmmword ptr[eax+16] 
		addps	xmm0, xmm1
		movaps  xmm1,xmm0
		shufps	xmm1,xmm0,0FDh			;xmm1 = shuffle(xmm0,xmm0)
		addss	xmm1,xmm0
		shufps	xmm0,xmm0,0FEh
		addss	xmm1,xmm0
		movss	returnval, xmm1
	}
	return returnval;
}
_n_inline void _8scalenadd(f32* ptr1,const f32* ptr2,f32 val)
{
	__asm 
	{
		movss	xmm0,val
		shufps	xmm0,xmm0,0h
		mov		eax,[ptr2]
		movaps	xmm1,[eax]
		movaps	xmm2,[eax+16]
		mulps	xmm1,xmm0
		mulps	xmm2,xmm0
		mov		eax,[ptr1]
		addps	xmm1,[eax]
		addps	xmm2,[eax+16]
		movaps	[eax],xmm1
		movaps	[eax+16],xmm2
	}
}
_n_inline void _8scale(f32* ptr1,const f32* ptr2,f32 val)
{
	__asm 
	{
		movss	xmm0,val
		shufps	xmm0,xmm0,0h
		mov		eax,[ptr2]
		movaps	xmm1,[eax]
		mulps	xmm1,xmm0
		movaps	xmm2,[eax+16]
		mulps	xmm2,xmm0
		mov		eax,[ptr1]
		movaps	[eax],xmm1
		movaps	[eax+16],xmm2
	}
}
_n_inline void _8scale(f32* ptr1,f32 val)
{
	__asm 
	{
		movss	xmm0,val
		shufps	xmm0,xmm0,0h
		mov		eax,[ptr1]
		movaps	xmm1,[eax]
		movaps	xmm2,[eax+16]
		mulps	xmm1,xmm0
		mulps	xmm2,xmm0
		movaps	[eax],xmm1
		movaps	[eax+16],xmm2
	}
}
#else
_n_inline f32 _velocity_dot(f32* dotp,f32* lv,f32* avel)
{
	return	dotp[0]*lv[0] + dotp[1]*lv[1] + dotp[2]*lv[2] + 
			dotp[4]*avel[0] + dotp[5]*avel[1] + dotp[6]*avel[2];
}
_n_inline f32 _8dot(f32* ptr1,f32* ptr2)
{
	return	ptr1[0]*ptr2[0]+ptr1[1]*ptr2[1]+ptr1[2]*ptr2[2]+
			ptr1[4]*ptr2[4]+ptr1[5]*ptr2[5]+ptr1[6]*ptr2[6];
}
_n_inline void _8scalenadd(f32* ptr1,const f32* ptr2,f32 val)
{
	xQ4ScaleNAdd(ptr1,ptr2,val);
	xQ4ScaleNAdd(ptr1+4,ptr2+4,val);
}
_n_inline void _8scale(f32* ptr1,const f32* ptr2,f32 val)
{
	xQ4ScaleS(ptr1,ptr2,val);
	xQ4ScaleS(ptr1+4,ptr2+4,val);
}
_n_inline void _8scale(f32* ptr1,f32 val)
{
	xQ4Scale(ptr1,val);
	xQ4Scale(ptr1+4,val);
}
_n_inline void _compute_tmp_bod(f32* arr,f32* lvel,f32 *avel,f32* facc,
							  f32* tacc,f32* invi,f32 invmass,f32 invdt)
{
	for (s32 j=0; j<3; j++) arr[j] = facc[j] * invmass + lvel[j] * invdt;
	xMul0_344(arr+4,tacc,invi);
	for (s32 j=0; j<3; j++) arr[4+j] += avel[j] * invdt;
}
_n_inline void _mul0_344_add(f32* v,f32* a,f32* r)
{
	v[0] += a[0]*r[0]+a[1]*r[1]*a[2]*r[2];
	v[1] += a[0]*r[4]+a[1]*r[5]*a[2]*r[6];
	v[2] += a[0]*r[8]+a[1]*r[9]*a[2]*r[10];
}

#endif

_n_inline void _ComputeBsp()
{
	for(s32 i=0; i<siOrder; i++)
	{
		cPhyBody* b1 = spkJmap[i*2+0];
		cPhyBody* b2 = spkJmap[i*2+1];
		X_Assert(b1);
		xQ4ScaleS(&spfBsp[i*16+0],&spfJsp[i*16+0],b1->mfInvMass);
		xMul0_344(&spfBsp[i*16+4],&spfJsp[i*16+4],b1->mkInvIWorld);
		if(b2)
		{
			xQ4ScaleS(&spfBsp[i*16+8],&spfJsp[i*16+8],b2->mfInvMass);
			xMul0_344(&spfBsp[i*16+12],&spfJsp[i*16+12],b2->mkInvIWorld);
		}
	}
}

_n_inline void _ComputeRhs(f32 invdt)
{
	for(s32 i=0; i<siOrder; i++)
	{
		cPhyBody*	body1 = spkJmap[i*2 + 0];
		cPhyBody*	body2 = spkJmap[i*2 + 1];
		spfRhs[i] = -_8dot(&spfJsp[i*16+0],body1->mkTmp);
		if(body2)
		{
			spfRhs[i] -= _8dot(&spfJsp[i*16+8],body2->mkTmp);
		}
		spfRhs[i] += spfBias[i]*invdt;
	}
}

//@	Algorithm 4 Approximately solve JBl = h given l0
//@		Work variables: a (6n-by-1), d (s-by-1)
//@		l = l0
//@		a = Bl
//@		for i = 1 to s do
//@			di = Jsp(i,1) ·Bsp(1, i)+Jsp(i,2) ·Bsp(2, i)
//@		end for
//@		for iter = 1 to iteration limit do
//@			for i = 1 to s do
//@				b1 = Jmap(i,1)
//@				b2 = Jmap(i,2)
//@				Dli = [hi−Jsp(i,1) ·a(b1)−Jsp(i,2) ·a(b2)]/di
//@				l0i = li
//@				li = max(l−i ,min(l0i +Dli,l+i ))
//@				Dli = li−l0i
//@				a(b1) = a(b1)+DliBsp(1, i)
//@				a(b2) = a(b2)+DliBsp(2, i)
//@			end for
//@		end for
// solve the linear complementary problem
_n_inline void _SolveLCP(s32 iNumIterations)
{
	//@ TODO: we currently do not save
	//@ the lambda values, but if we implement
	//@ warm starting in any version, we
	//@ will initialize lambda to previous lambda
	//@ for now initialize to 0
	storeDwords(spfLambda,siOrder,0);
	// a = B * Lambda
	// TODO: now lambda is zero, so, we can skip
	// this calculation, this is wrong
	// however.
	//	compute spfBsp
	_ComputeBsp();

	// calculate di = Jsp(i,1)·Bsp(1,i)+Jsp(i,2)·Bsp(2,i)
	for(s32 i=0; i<siOrder; i++)
	{
		f32 sum = _8dot(&spfJsp[i*16+0],&spfBsp[i*16+0]);
		if(spkJmap[i*2+1])
			sum += _8dot(&spfJsp[i*16+8],&spfBsp[i*16+8]);
		// we can add a relaxation parameter here
#define LcpRelax 1.3f
#define LcpCfm	 0.0f
		spfD[i] = LcpRelax/(sum+LcpCfm);

		// pre multiply spfJsp and spfRhs by spfD
		spfRhs[i] *= spfD[i];
		_8scale(&spfJsp[i*16+0],spfD[i]);
		if(spkJmap[i*2 + 1])
			_8scale(&spfJsp[i*16+8],spfD[i]);
	//	spfD[i]*=LcpCfm;
#undef LcpRelax 
#undef LcpCfm
	}
	
	// do iterations
	for(s32 it = 0; it < iNumIterations; it++)
	{
		for(s32 i=0; i<siOrder; i++)
		{
			cPhyBody* b1 = spkJmap[i*2 + 0];
			cPhyBody* b2 = spkJmap[i*2 + 1];
			f32 delta = spfRhs[i];
			delta -= _8dot(&spfJsp[i*16+0],b1->mkAcc);
			if(b2)
				delta -= _8dot(&spfJsp[i*16+8],b2->mkAcc);
			//delta *= spfD[i];
			f32 fNewLambda = spfLambda[i]+delta;
			// doin simd :| dont kno if it will help :(
			_x_fclamp(fNewLambda,spfLo[i],spfHi[i]);
			delta = fNewLambda - spfLambda[i];
			spfLambda[i] = fNewLambda;
			// we should not clamp the corrective impulse here
			// rather we clamp the impulse after all iterations
			_8scalenadd(b1->mkAcc,&spfBsp[i*16+0],delta);
			if(b2)
				_8scalenadd(b2->mkAcc,&spfBsp[i*16+8],delta);
		}
	}
}

cSimulator::cSimulator(const tchar* sConf) : miNumIterations(20)
{
	RefObjectInit(cSimulator);
	SharedSingleInterfaceInit(cSimulator);
	mkDisable.mfAngularThreshold = 0.0001f;
	mkDisable.mfLinearThreshold = 0.0001f;
	mkDisable.mtIdleTime = 0.20f;
}

cSimulator::~cSimulator()
{
	// destroy memory chunk
	if(spfMemoryChunk)
		freeMemoryAligned(spfMemoryChunk);
	if(spkJmap)
		freeMemory(spkJmap);
	// destroy all the existing np constrains
	skNPConstrainStack.destroyMemory();
	skFCConstrainStack.destroyMemory();
	// destroy all bodies
	{
	cPhyBody* start = mkBodies.begin();
	while(start)
	{
		cPhyBody* next = start->mpkNext;
		remBody(start);
		delete start;
		start = next;
	}
	}
	// destroy all constrains
	{
	cConstrain* start = mkConstrains.begin();
	while(start)
	{
		cConstrain* next = start->mpkNext;
		remConstrain(start);
		delete start;
		start = next;
	}
	}
	SharedSingleInterfaceExit(cSimulator);
}

// the simulate function
void cSimulator::wSimulate(f32 step)
{
	f32 invstep = 1/step;
	// solve for constrains
	if(!mkBodies.size()) return;
	// precompute certain fields
	cPhyBody* startb = mkBodies.begin();
	while(startb != mkBodies.endactive())
	{
		mat34_t	kTmpM;
		xMul1_34(kTmpM,startb->mkInvI,startb->mkWorld.rot);
		xMul0_34(startb->mkInvIWorld,startb->mkWorld.rot,kTmpM);
		if(!(startb->muFlags & cPhyBody::GroundStaticObject))
			xQ4ScaleNAdd(startb->mkFext,skGravity,startb->mfMass);
		startb = startb->mpkNext;
	}
	siOrder = 0;
	// calculate the order
	cConstrain* startc = mkConstrains.begin();
	while(startc != mkConstrains.endactive())
	{
		siOrder += startc->getOrder();
		startc = startc->mpkNext;
	}
	for(s32 i=0; i<skFCConstrainStack.size(); i++)
		siOrder += skFCConstrainStack[i]->getOrder();
#ifdef NEXTAR_INCLUDE_NON_PENETRATION_CONSTRAINS
	for(s32 i=0; i<skNPConstrainStack.size(); i++)
		siOrder += skNPConstrainStack[i]->getOrder();
#endif
	if(siOrder) 
	{
		// we assume spfJsp is 2*8*m where m is the number of constrains
		if(siOrder > siStackOrder || siOrder*2 < siStackOrder)
		{
			REALLOC_CACHE_ALIGNED(f32,spfMemoryChunk,siOrder*38);
			REALLOC_CACHE(cPhyBody*,spkJmap,siOrder*2);
			storeDwords(spfMemoryChunk,siOrder*38,0);
			storeDwords(spkJmap,siOrder*2,0);
			spfJsp = spfMemoryChunk;
			spfBsp = spfMemoryChunk + siOrder*16;
			spfLambda = spfMemoryChunk + siOrder*32;
			spfHi = spfMemoryChunk + siOrder*33;
			spfLo = spfMemoryChunk + siOrder*34;
			spfBias = spfMemoryChunk + siOrder*35;
			spfD = spfMemoryChunk + siOrder*36;
			spfRhs = spfMemoryChunk + siOrder*37;
		//	spfRelax = spfMemoryChunk + siOrder*38;
			siStackOrder = siOrder;
		}
		
		cConstrain::info kInfo;
		kInfo.invstep = invstep;
		kInfo.jptr = spfJsp;
		kInfo.bias = spfBias;
		kInfo.lhi  = spfHi;
		kInfo.llo  = spfLo;
		//kInfo.relax = spfRelax;
		kInfo.bod_ptrs = spkJmap;
		// lets take the np constrains now
				
		for(s32 i=0; i<skFCConstrainStack.size(); i++)
			skFCConstrainStack[i]->getInfo(&kInfo);
#ifdef NEXTAR_INCLUDE_NON_PENETRATION_CONSTRAINS
		for(s32 i=0; i<skNPConstrainStack.size(); i++)
			skNPConstrainStack[i]->getInfo(&kInfo);
#endif //NEXTAR_INCLUDE_NON_PENETRATION_CONSTRAINS
		startc = mkConstrains.begin();
		while(startc != mkConstrains.endactive())
		{
			startc->getInfo(&kInfo);
			startc = startc->mpkNext;
		}
		// calculate spfLambda
		// solve
		// v/h + invmass*fe this is calculated and stored in tmp of every active body
		startb = mkBodies.begin();
		while(startb != mkBodies.endactive())
		{	
		//@ There is potential optimization in this function,
		//@	but needs proper testing:
		//@	_compute_tmp_bod(startb->mkTmp,startb->mkPdot,startb->mkOmega,
		//@		startb->mkFacc,startb->mkTacc,startb->mkInvIWorld,startb->mfInvMass,invstep);
			xQ4ScaleS(startb->mkTmp,startb->mkFext,startb->mfInvMass);
			xQ4ScaleNAdd(startb->mkTmp,startb->mkPdot,invstep);
			xMul0_344(startb->mkTmp+4,startb->mkText,startb->mkInvIWorld);
			xQ4ScaleNAdd(startb->mkTmp+4,startb->mkOmega,invstep);
			startb = startb->mpkNext;
		}
		_ComputeRhs(invstep);
		// initialize A
		startb = mkBodies.begin();
		while(startb != mkBodies.endactive())
		{	
			// since we do not use warm starting
			// we initialize A to 0 
			storeDwords(startb->mkAcc,8,0);
			startb = startb->mpkNext;
		}
		_SolveLCP(miNumIterations);
		// caculate constrain update
		startb = mkBodies.begin();
		while(startb != mkBodies.endactive())
		{	
			xQ4ScaleNAdd(startb->mkPdot,startb->mkFacc,step);
			xQ4ScaleNAdd(startb->mkOmega,startb->mkTacc,step);
			startb = startb->mpkNext;
		}

	}
	_bodyStep(step);
	_autoDisable(step);
	// destroy the NP constrains
	skFCConstrainStack.destroy();
#ifdef NEXTAR_INCLUDE_NON_PENETRATION_CONSTRAINS
	skNPConstrainStack.destroy();
#endif
}

void cSimulator::_autoDisable(f32 dt)
{
	//@ loop thru only the active bodies
	cPhyBody* start = mkBodies.begin();
	while(start != mkBodies.endactive())
	{	
		// if we disable a body which is stable in this step, a
		// lot of calculations are avoided in the next step
		if(start->muFlags & cPhyBody::AutoDisable)
		{
			bool isidle = false;
			if(xQ4LenSqR(start->mkPdot) <  mkDisable.mfLinearThreshold )
			{
				if( xQ4LenSqR(start->mkOmega) < mkDisable.mfAngularThreshold )
						isidle = true;
			}

			if(isidle)
			{
				// this must be idle, decrease time
				start->mtTimeToIdle -= dt;
				if(X_LessThanZero(start->mtTimeToIdle))
				{
					// disable
					start->disable();
					start = mkBodies.deactivate(start);
					continue;
				}
			}
			else
				start->mtTimeToIdle = mkDisable.mtIdleTime;
		}
		start = start->mpkNext;
	}
}

void cSimulator::disableBody(nextar::cPhyBody *bod)
{
	X_Assert(bod);
	X_Assert(!bod->isDisabled());
	bod->disable();
	mkBodies.deactivate(bod);
}

void cSimulator::enableBody(nextar::cPhyBody *bod)
{
	X_Assert(bod);
	X_Assert(bod->isDisabled());
	bod->enable();
	mkBodies.activate(bod);
}

void cSimulator::disableConstrain(nextar::cConstrain *c)
{
	X_Assert(c);
	X_Assert(c->isEnabled());
	c->disable();
	mkConstrains.deactivate(c);
}

void cSimulator::enableConstrain(nextar::cConstrain *c)
{
	X_Assert(c);
	X_Assert(c->isDisabled());
	c->enable();
	mkConstrains.activate(c);
}

void cSimulator::_bodyStep(f32 dt)
{
	cPhyBody* start = mkBodies.begin();
	while(start != mkBodies.endactive())
	{
		// update velocity using force components
		f32 invmass = start->mfInvMass;
		xQ4ScaleNAdd(start->mkPdot,start->mkFext,invmass*dt);
		xQ4Scale(start->mkText,dt);
		_mul0_344_add(start->mkOmega,start->mkText,start->mkInvIWorld);

		// update position
		xQ4ScaleNAdd(start->mkWorld.pos,start->mkPdot,dt);
		// update rotation
		quat_t dq;
		nWtoDQuat(dq,start->mkOmega,start->mkQ);
		xQ4ScaleNAdd(start->mkQ,dq,dt);

		// normalize
		xQuatNormalize(start->mkQ);
		nQuatToR(start->mkWorld.rot,start->mkQ);
		iCollisionObject* ptr = start->mpkObjects;
		while(ptr)
		{
			ptr->validateMove();
			ptr = ptr->mpkBodyNext;
		}
		// nutralize the external forces
		storeDwords(start->mkAext,8,0);
		start = start->mpkNext;
	}
}

cPhyBody* cSimulator::addBody()
{
	cPhyBody* ptr = new cPhyBody();
	mkBodies.add(ptr);
	return ptr;
}

void cSimulator::remBody(nextar::cPhyBody *bod)
{
	iCollisionObject* ptr = bod->mpkObjects;
	while(ptr)
	{
		iCollisionObject* next = ptr->mpkBodyNext;
		ptr->detach();
		ptr = next;
	}
	mkBodies.rem(bod);
}

cContactConstrain* cSimulator::addNPConstrain()
{
	return skNPConstrainStack.alloc();
}

cFrictionConstrain* cSimulator::addFCConstrain()
{
	return skFCConstrainStack.alloc();
}

void cSimulator::remConstrain(nextar::cConstrain *c)
{
	mkConstrains.rem(c);
}

void cSimulator::setGravity(f32 x,f32 y,f32 z)
{
	skGravity[0] = x;
	skGravity[1] = y;
	skGravity[2] = z;
}