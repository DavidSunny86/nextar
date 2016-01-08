//@ Abhishek Dey
//@ june 29, 07 (as per the actual code was written on)
//@ Oct 25 2007

#include <cbbox.h>
#include <cplane.h>
#include <icollisionspace.h>
#include <xcollider.h>

using namespace nextar;
cBBox::cBBox(iCollisionSpace *pkS, f32 fX, f32 fY, f32 fZ) : iCollisionObject(pkS)
{
	X_Assert( fX > 0 && fY > 0 && fZ > 0 );
	mkHalfSide = (f32*)allocMemoryAligned(sizeof(vec4_t));
	muClass    = Col_BBox;
	mkHalfSide[0] = fX*.5f;mkHalfSide[1] = fY*.5f;mkHalfSide[2] = fZ*.5f;mkHalfSide[3] = 0;
}

cBBox::~cBBox()
{
	freeMemoryAligned(mkHalfSide);
}

void cBBox::computeAABB()
{
	X_Assert(mpkWorld);
	// the code bellow is a good candidate for simd
	// but then fabsr is problematic
	vec4_t	tmp,tmp2;
	//@ I dont know if SIMD will help here,
	//@ but i do nonetheless
	const f32* r = mpkWorld->rot;
	const f32* p = mpkWorld->pos;

	xQ4Mul(tmp,mkHalfSide,(const f32*)r);
	tmp2[0] = (fabsr(tmp[0]) + fabsr(tmp[1]) + fabsr(tmp[2]));
	xQ4Mul(tmp,mkHalfSide,(const f32*)(r+4));
	tmp2[1] = (fabsr(tmp[0]) + fabsr(tmp[1]) + fabsr(tmp[2]));
	xQ4Mul(tmp,mkHalfSide,(const f32*)(r+8));
	tmp2[2] = (fabsr(tmp[0]) + fabsr(tmp[1]) + fabsr(tmp[2]));
	tmp2[3] = 0;
	xQ4Sub(tmp,p,tmp2);
	xQ4Add(tmp2,p,tmp2);
	mkBBox.min.x = tmp[0];
	mkBBox.min.y = tmp[1];
	mkBBox.min.z = tmp[2];
	mkBBox.max.x = tmp2[0];	
	mkBBox.max.y = tmp2[1];
	mkBBox.max.z = tmp2[2];
}

f32 cBBox::pointDepth(const f32 *point)
{
	X_Assert(mpkWorld);
	vec4_t	q;
	f32		d[6];

	xQ4Sub(q,point,mpkWorld->pos);
	xMul1_344(q,q,mpkWorld->rot);

	bool inside = true;
	for(s32 i=0; i<3; i++)
	{
		d[i+0] = mkHalfSide[i] - q[i];
		d[i+3] = mkHalfSide[i] + q[i];

		if(d[i] < 0||d[i+3] < 0)
			inside = false;
	}

	if(inside) 
	{
		f32 r = (f32)(unsigned)-1;
		for(s32 i=0; i<6; i++)
			if(d[i]<r) r = d[i];

		return r;
	}

	f32 r = 0;
	for(s32 i=0; i<6; i++)
		if(d[i]>r) r = d[i];

	return -r;
}

extern	s32 dBoxBox (const f32* p1, const f32* R1, const f32* side1,
			 const f32* p2, const f32* R2, const f32* side2,
			 cContact* contact,f32* normal);

namespace nextar
{
namespace collision
{

extern cContact* gpkContactHolder;
s32 xColBoxBox(iCollisionObject* o1,iCollisionObject* o2)
{
	X_Assert(o1->muClass==iCollisionObject::Col_BBox);
	X_Assert(o2->muClass==iCollisionObject::Col_BBox);
	vec4_t	normal;
	cBBox	*box1 = (cBBox*)o1;
	cBBox	*box2 = (cBBox*)o2;

	vec4_t side1,side2;
	xQ4ScaleS(side1,box1->mkHalfSide,2.0f);
	xQ4ScaleS(side2,box2->mkHalfSide,2.0f);
	// this is goin to consume a lot of time, calculating the
	// collision points, though simd will help a little
	cContact* g = gpkContactHolder;
	s32 numc = dBoxBox(	
		box1->mpkWorld->pos,box1->mpkWorld->rot,side1,
		box2->mpkWorld->pos,box2->mpkWorld->rot,side2,
		g,normal);
//	s32 numc = cBoxCollide(	
//		box1->mpkWorld->pos,box1->mpkWorld->rot,box1->mkHalfSide,
//		box2->mpkWorld->pos,box2->mpkWorld->rot,box2->mkHalfSide,
//		g,normal);
	

	// I'm not sure if the normal needs negetion
	// this step might be unnecessary
	if( numc )
	{
		normal[0] =-normal[0];
		normal[1] =-normal[1];
		normal[2] =-normal[2];
		normal[3] = 0;
		s32 num = numc;
		while( num )
		{
			--num;
			xQ4CpyMac(gpkContactHolder[num].norm,normal);
		}
	}
	return numc;
}

s32 xColBoxPlane(iCollisionObject* o1,iCollisionObject* o2)
{
	X_Assert(o1->muClass==iCollisionObject::Col_BBox);
	X_Assert(o2->muClass==iCollisionObject::Col_Plane);
	cBBox	*box   = (cBBox*)o1;
	cPlane	*plane = (cPlane*)o2;
	const f32* r = box->mpkWorld->rot;
	const f32* n = plane->mkPlane;

	vec4_t a,p;
	f32 b1,b2,b3;
	xMul0_344(a,n,r);	// this is safe
	xQ4Mul(a,a,box->mkHalfSide);
	xQ4Scale(a,2.0f);
	b1 = fabsr(a[0]);
	b2 = fabsr(a[1]);
	b3 = fabsr(a[2]);
	// early exit test
	f32 depth;
	depth = n[PND] + 0.5f*(b1+b2+b3) - xVDotR(n,box->mpkWorld->pos);
	if (X_IsFloatNeg(depth)) return 0;

	// find number of contacts requested
	const s32 maxc = 3;
	// find deepest point
	xQ4CpyMac(p,box->mpkWorld->pos);

#define P1(i,op) xQ4ScaleN##op(p,&r[i*4],box->mkHalfSide[i]);
#define PALL(i) if (a[i]>0) { P1(i,Sub) } else { P1(i,Add) }
	PALL(0);
	PALL(1);
	PALL(2);
#undef P1
#undef PALL
	// the deepest point is the first contact point
	xQ4CpyMac(gpkContactHolder[0].pos,p);
	xQ4CpyMac(gpkContactHolder[0].norm,n);
	gpkContactHolder[0].depth = depth;	// 
	s32 ret = 1;		// ret is number of contact points found so far
	// get the second and third contact points by starting from `p' and going
	// along the two sides with the smallest projected length.
#define P1(i,j,op) \
	xQ4CpyMac(gpkContactHolder[i].pos,p);\
	xQ4ScaleN##op(gpkContactHolder[i].pos,&r[j*4],2*box->mkHalfSide[j]);
#define PALL(ctact,side,sideinc) \
	depth -= b ## sideinc; \
	if (depth < 0) goto done; \
	if (a[sideinc-1] > 0) { P1(ctact,side,Add) } else { P1(ctact,side,Sub) } \
	gpkContactHolder[ctact].depth = depth; \
	ret++;

	xQ4CpyMac(gpkContactHolder[1].norm,n);
	xQ4CpyMac(gpkContactHolder[2].norm,n);
	if (b1 < b2) 
	{
		if (b3 < b1) goto use_side_3; 
		else 
		{
			PALL(1,0,1);	// use side 1
			if (b2 < b3) goto contact2_2; 
			else goto contact2_3;
		}
	}
	else 
	{
		if (b3 < b2) 
		{
use_side_3:	// use side 3
			PALL(1,2,3);
			if (b1 < b2) goto contact2_1; 
			else goto contact2_2;
		}
		else 
		{
			PALL(1,1,2);	// use side 2
			if (b1 < b3) goto contact2_1; 
			else goto contact2_3;
		}
	}

contact2_1: PALL(2,0,1); goto done;
contact2_2: PALL(2,1,2); goto done;
contact2_3: PALL(2,2,3); goto done;
#undef P1
#undef PALL

done:
	return ret;
}

/*

#define dDOTpq(a,b,p,q) ((a)[0]*(b)[0] + (a)[p]*(b)[q] + (a)[2*(p)]*(b)[2*(q)])
#define dDOT14(a,b) dDOTpq(a,b,1,4)
#define dDOT41(a,b) dDOTpq(a,b,4,1)
#define dDOT44(a,b) dDOTpq(a,b,4,4)
#define dDOT(a,b)	dDOTpq(a,b,1,1)



// all row ops are changed to coloumn ops
// and vice versa
s32 xColBoxPlane(iCollisionObject* o1,iCollisionObject* o2)
{
	X_Assert(o1->muClass==iCollisionObject::Col_BBox);
	X_Assert(o2->muClass==iCollisionObject::Col_Plane);
	cBBox	*box   = (cBBox*)o1;
	cPlane	*plane = (cPlane*)o2;
	const f32* R = box->mpkWorld->rot;
	const f32* n = plane->mkPlane;
	vec4_t side1;
	xQ4ScaleS(side1,box->mkHalfSide,2.0f);

	int ret = 0;
	cContact* contact = gpkContactHolder;

	// project sides lengths along normal vector, get absolute values
	f32 Q1 = dDOT(n,R+0);
	f32 Q2 = dDOT(n,R+4);
	f32 Q3 = dDOT(n,R+8);
	f32 A1 = side1[0] * Q1;
	f32 A2 = side1[1] * Q2;
	f32 A3 = side1[2] * Q3;
	f32 B1 = fabsr(A1);
	f32 B2 = fabsr(A2);
	f32 B3 = fabsr(A3);

	// early exit test
	f32 depth = plane->mkPlane[3] + f32(0.5)*(B1+B2+B3) - dDOT(n,o1->mpkWorld->pos);
	if (depth < 0) return 0;

	// find number of contacts requested
	int maxc = 4;
	if (maxc < 1) maxc = 1;
	if (maxc > 3) maxc = 3;	// no more than 3 contacts per box allowed

	// find deepest point
	vec4_t p={0};
	p[0] = o1->mpkWorld->pos[0];
	p[1] = o1->mpkWorld->pos[1];
	p[2] = o1->mpkWorld->pos[2];
#define FOO(i,op) \
	p[0] op f32(0.5)*side1[i] * R[4*i+0]; \
	p[1] op f32(0.5)*side1[i] * R[4*i+1]; \
	p[2] op f32(0.5)*side1[i] * R[4*i+2];
#define BAR(i,iinc) if (A ## iinc > 0) { FOO(i,-=) } else { FOO(i,+=) }
	BAR(0,1);
	BAR(1,2);
	BAR(2,3);
#undef FOO
#undef BAR

	// the deepest point is the first contact point
	contact->pos[0] = p[0];
	contact->pos[1] = p[1];
	contact->pos[2] = p[2];
	contact->norm[0] = n[0];
	contact->norm[1] = n[1];
	contact->norm[2] = n[2];
	contact->depth = depth;
	ret = 1;		// ret is number of contact points found so far
	if (maxc == 1) goto done;

	// get the second and third contact points by starting from `p' and going
	// along the two sides with the smallest projected length.

#define FOO(i,j,op) \
	(contact+i)->pos[0] = p[0] op side1[j] * R[4*j+0]; \
	(contact+i)->pos[1] = p[1] op side1[j] * R[4*j+1]; \
	(contact+i)->pos[2] = p[2] op side1[j] * R[4*j+2];
#define BAR(ctact,side,sideinc) \
	depth -= B ## sideinc; \
	if (depth < 0) goto done; \
	if (A ## sideinc > 0) { FOO(ctact,side,+) } else { FOO(ctact,side,-) } \
	contact[ctact].depth = depth; \
	ret++;

	contact[1].norm[0] = n[0];
	contact[1].norm[1] = n[1];
	contact[1].norm[2] = n[2];
	if (maxc == 3) {
		contact[2].norm[0] = n[0];
		contact[2].norm[1] = n[1];
		contact[2].norm[2] = n[2];
	}

	if (B1 < B2) {
		if (B3 < B1) goto use_side_3; else {
			BAR(1,0,1);	// use side 1
			if (maxc == 2) goto done;
			if (B2 < B3) goto contact2_2; else goto contact2_3;
		}
	}
	else {
		if (B3 < B2) {
use_side_3:	// use side 3
			BAR(1,2,3);
			if (maxc == 2) goto done;
			if (B1 < B2) goto contact2_1; else goto contact2_2;
		}
		else {
			BAR(1,1,2);	// use side 2
			if (maxc == 2) goto done;
			if (B1 < B3) goto contact2_1; else goto contact2_3;
		}
	}

contact2_1: BAR(2,0,1); goto done;
contact2_2: BAR(2,1,2); goto done;
contact2_3: BAR(2,2,3); goto done;
#undef FOO
#undef BAR

done:
	return ret;
}
*/
}
}



