//@ Abhishek Dey
//@ Oct 24 2007
//@ Oct 25 2007

#include <xmath_c.h>
#include <xcontact.h>
#include <xmemory.h>

#define dDOTpq(a,b,p,q) ((a)[0]*(b)[0] + (a)[p]*(b)[q] + (a)[2*(p)]*(b)[2*(q)])
#define dDOT14(a,b) dDOTpq(a,b,1,4)
#define dDOT41(a,b) dDOTpq(a,b,4,1)
#define dDOT44(a,b) dDOTpq(a,b,4,4)
#define dDOT(a,b)	dDOTpq(a,b,1,1)

void dLineClosestApproach ( const f32* pa, const f32* ua,
							const f32* pb, const f32* ub,
							f32 *alpha, f32 *beta)
{
  vec4_t p;
  f32 uaub,q1,q2,d;
  p[0] = pb[0] - pa[0];
  p[1] = pb[1] - pa[1];
  p[2] = pb[2] - pa[2];
  uaub = dDOT(ua,ub);
  q1 =  dDOT(ua,p);
  q2 = -dDOT(ub,p);
  d = 1-uaub*uaub;
  if (d <= (0.0001f)) {
    // @@@ this needs to be made more robust
    *alpha = 0;
    *beta  = 0;
  }
  else {
    d = 1/(d);
    *alpha = (q1 + uaub*q2)*d;
    *beta  = (uaub*q1 + q2)*d;
  }
}

static s32 intersectRectQuad (f32 h[2], f32 p[8], f32 ret[16])
{
	// q (and r) contain nq (and nr) coordinate points for the current (and
	// chopped) polygons
	s32 nq=4,nr;
	f32 buffer[16];
	f32 *q = p;
	f32 *r = ret;
	s32 dir,i,sign;
	for (dir=0; dir <= 1; dir++) {
		// direction notation: xy[0] = x axis, xy[1] = y axis
		for (sign=-1; sign <= 1; sign += 2) {
			// chop q along the line xy[dir] = sign*h[dir]
			f32 *pq = q;
			f32 *pr = r;
			nr = 0;
			for (i=nq; i > 0; i--) {
				f32 *nextq;
				// go through all points in q and all lines between adjacent points
				if (sign*pq[dir] < h[dir]) {
					// this point is inside the chopping line
					pr[0] = pq[0];
					pr[1] = pq[1];
					pr += 2;
					nr++;
					if (nr & 8) {
						q = r;
						goto done;
					}
				}
				nextq = (i > 1) ? pq+2 : q;
				if ((sign*pq[dir] < h[dir]) ^ (sign*nextq[dir] < h[dir])) {
					// this line crosses the chopping line
					pr[1-dir] = pq[1-dir] + (nextq[1-dir]-pq[1-dir]) /
						(nextq[dir]-pq[dir]) * (sign*h[dir]-pq[dir]);
					pr[dir] = sign*h[dir];
					pr += 2;
					nr++;
					if (nr & 8) {
						q = r;
						goto done;
					}
				}
				pq += 2;
			}
			q = r;
			r = (q==ret) ? buffer : ret;
			nq = nr;
		}
	}
done:
	if (q != ret) memcpy (ret,q,nr*2*sizeof(f32));
	return nr;
}


// given n points in the plane (array p, of size 2*n), generate m points that
// best represent the whole set. the definition of 'best' here is not
// predetermined - the idea is to select points that give good box-box
// collision detection behavior. the chosen point indexes are returned in the
// array iret (of size m). 'i0' is always the first entry in the array.
// n must be in the range [1..8]. m must be in the range [1..n]. i0 must be
// in the range [0..n-1].

void cullPoints (s32 n, f32 p[], s32 m, s32 i0, s32 iret[])
{
	// compute the centroid of the polygon in cx,cy
	s32 i,j;
	f32 a,cx,cy,q;
	f32 A[8];
	s32 avail[8];
	if (n==1) {
		cx = p[0];
		cy = p[1];
	}
	else if (n==2) {
		cx = (f32)(0.5)*(p[0] + p[2]);
		cy = (f32)(0.5)*(p[1] + p[3]);
	}
	else {
		a = 0;
		cx = 0;
		cy = 0;
		for (i=0; i<(n-1); i++) {
			q = p[i*2]*p[i*2+3] - p[i*2+2]*p[i*2+1];
			a += q;
			cx += q*(p[i*2]+p[i*2+2]);
			cy += q*(p[i*2+1]+p[i*2+3]);
		}
		q = p[n*2-2]*p[1] - p[0]*p[n*2-1];
		a = 1.f/((f32)(3.0)*(a+q));
		cx = a*(cx + q*(p[n*2-2]+p[0]));
		cy = a*(cy + q*(p[n*2-1]+p[1]));
	}

	// compute the angle of each point w.r.t. the centroid
	
	for (i=0; i<n; i++) A[i] = atan2r(p[i*2+1]-cy,p[i*2]-cx);

	// search for points that have angles closest to A[i0] + i*(2*pi/m).
	
	for (i=0; i<n; i++) avail[i] = 1;
	avail[i0] = 0;
	iret[0] = i0;
	iret++;
	for (j=1; j<m; j++) {
		f32 maxdiff=1e9,diff;
		a = (f32)(j)*(2*N3DPi/m) + A[i0];
		if (a > N3DPi) a -= 2*N3DPi;
		for (i=0; i<n; i++) {
			if (avail[i]) {
				diff = fabsr (A[i]-a);
				if (diff > N3DPi) diff = 2*N3DPi - diff;
				if (diff < maxdiff) {
					maxdiff = diff;
					*iret = i;
				}
			}
		}
		avail[*iret] = 0;
		iret++;
	}
}


// given two boxes (p1,R1,side1) and (p2,R2,side2), collide them together and
// generate contact points. this returns 0 if there is no contact otherwise
// it returns the number of contacts generated.
// `normal' returns the contact normal.
// `depth' returns the maximum penetration depth along that normal.
// `return_code' returns a number indicating the type of contact that was
// detected:
//        1,2,3 = box 2 intersects with a face of box 1
//        4,5,6 = box 1 intersects with a face of box 2
//        7..15 = edge-edge contact
// `maxc' is the maximum number of contacts allowed to be generated, i.e.
// the size of the `contact' array.
// `contact' and `skip' are the contact array information provided to the
// collision functions. this function only fills in the position and depth
// fields.

s32 dBoxBox (const f32* p1, const f32* R1, const f32* side1,
			 const f32* p2, const f32* R2, const f32* side2,
			 cContact* contact,f32* normal)
{
	s32 maxc = 4;
	const f32 fudge_factor = (f32)(1.05);
	vec4_t p,pp,normalC;
	const f32 *normalR = 0;
	f32 depth;
	f32 A[3],B[3],R11,R12,R13,R21,R22,R23,R31,R32,R33,
		Q11,Q12,Q13,Q21,Q22,Q23,Q31,Q32,Q33,s,s2,l;
	s32 i,j,invert_normal,code;
	const f32 *Ra,*Rb,*pa,*pb,*Sa,*Sb;
	vec4_t normal2,nr,anr;vec4_t center;
	s32 lanr,a1,a2;
	s32 codeN,code1,code2;

	// get vector from centers of box 1 to box 2, relative to box 1
	p[0] = p2[0] - p1[0];
	p[1] = p2[1] - p1[1];
	p[2] = p2[2] - p1[2];
	p[3] = 0;
	xMul0_344 (pp,p,R1);		// get pp = p relative to body 1

	// get side lengths / 2
	A[0] = side1[0]*(f32)(0.5);
	A[1] = side1[1]*(f32)(0.5);
	A[2] = side1[2]*(f32)(0.5);
	B[0] = side2[0]*(f32)(0.5);
	B[1] = side2[1]*(f32)(0.5);
	B[2] = side2[2]*(f32)(0.5);

	// Rij is R1'*R2, i.e. the relative rotation between R1 and R2
	R11 = dDOT(R1+0,R2+0); R12 = dDOT(R1+0,R2+4); R13 = dDOT(R1+0,R2+8);
	R21 = dDOT(R1+4,R2+0); R22 = dDOT(R1+4,R2+4); R23 = dDOT(R1+4,R2+8);
	R31 = dDOT(R1+8,R2+0); R32 = dDOT(R1+8,R2+4); R33 = dDOT(R1+8,R2+8);

	Q11 = fabsr(R11); Q12 = fabsr(R12); Q13 = fabsr(R13);
	Q21 = fabsr(R21); Q22 = fabsr(R22); Q23 = fabsr(R23);
	Q31 = fabsr(R31); Q32 = fabsr(R32); Q33 = fabsr(R33);

	// for all 15 possible separating axes:
	//   * see if the axis separates the boxes. if so, return 0.
	//   * find the depth of the penetration along the separating axis (s2)
	//   * if this is the largest depth so far, record it.
	// the normal vector will be set to the separating axis with the smallest
	// depth. note: normalR is set to point to a column of R1 or R2 if that is
	// the smallest depth normal so far. otherwise normalR is 0 and normalC is
	// set to a vector relative to body 1. invert_normal is 1 if the sign of
	// the normal should be flipped.

#define TST(expr_1,expr2,norm,cc) \
	expr1 = expr_1;\
	s2 = fabsr(expr1) - (expr2); \
	if (s2 > 0) return 0; \
	if (s2 > s) { \
	s = s2; \
	normalR = norm; \
	invert_normal = ((expr1) < 0); \
	code = (cc); \
	}

	f32 expr1;
	s = -N3DInfinity;
	invert_normal = 0;
	code = 0;

	// separating axis = u1,u2,u3
	TST (pp[0],(A[0] + B[0]*Q11 + B[1]*Q12 + B[2]*Q13),R1+0,1);
	TST (pp[1],(A[1] + B[0]*Q21 + B[1]*Q22 + B[2]*Q23),R1+4,2);
	TST (pp[2],(A[2] + B[0]*Q31 + B[1]*Q32 + B[2]*Q33),R1+8,3);

	// separating axis = v1,v2,v3
	TST (dDOT(R2+0,p),(A[0]*Q11 + A[1]*Q21 + A[2]*Q31 + B[0]),R2+0,4);
	TST (dDOT(R2+4,p),(A[0]*Q12 + A[1]*Q22 + A[2]*Q32 + B[1]),R2+4,5);
	TST (dDOT(R2+8,p),(A[0]*Q13 + A[1]*Q23 + A[2]*Q33 + B[2]),R2+8,6);

	// note: cross product axes need to be scaled when s is computed.
	// normal (n1,n2,n3) is relative to box 1.
#undef TST
#define TST(expr1,expr2,n1,n2,n3,cc) \
	s2 = fabsr(expr1) - (expr2); \
	if (s2 > 0) return 0; \
	l = fsqrtr((n1)*(n1) + (n2)*(n2) + (n3)*(n3)); \
	if (l > 0) { \
	s2 /= l; \
	if (s2*fudge_factor > s) { \
	s = s2; \
	normalR = 0; \
	normalC[0] = (n1)/l; normalC[1] = (n2)/l; normalC[2] = (n3)/l; \
	invert_normal = ((expr1) < 0); \
	code = (cc); \
	} \
	}

	// separating axis = u1 x (v1,v2,v3)
	TST(pp[2]*R21-pp[1]*R31,(A[1]*Q31+A[2]*Q21+B[1]*Q13+B[2]*Q12),0,-R31,R21,7);
	TST(pp[2]*R22-pp[1]*R32,(A[1]*Q32+A[2]*Q22+B[0]*Q13+B[2]*Q11),0,-R32,R22,8);
	TST(pp[2]*R23-pp[1]*R33,(A[1]*Q33+A[2]*Q23+B[0]*Q12+B[1]*Q11),0,-R33,R23,9);

	// separating axis = u2 x (v1,v2,v3)
	TST(pp[0]*R31-pp[2]*R11,(A[0]*Q31+A[2]*Q11+B[1]*Q23+B[2]*Q22),R31,0,-R11,10);
	TST(pp[0]*R32-pp[2]*R12,(A[0]*Q32+A[2]*Q12+B[0]*Q23+B[2]*Q21),R32,0,-R12,11);
	TST(pp[0]*R33-pp[2]*R13,(A[0]*Q33+A[2]*Q13+B[0]*Q22+B[1]*Q21),R33,0,-R13,12);

	// separating axis = u3 x (v1,v2,v3)
	TST(pp[1]*R11-pp[0]*R21,(A[0]*Q21+A[1]*Q11+B[1]*Q33+B[2]*Q32),-R21,R11,0,13);
	TST(pp[1]*R12-pp[0]*R22,(A[0]*Q22+A[1]*Q12+B[0]*Q33+B[2]*Q31),-R22,R12,0,14);
	TST(pp[1]*R13-pp[0]*R23,(A[0]*Q23+A[1]*Q13+B[0]*Q32+B[1]*Q31),-R23,R13,0,15);

#undef TST

	if (!code) return 0;

	// if we get to this point, the boxes interpenetrate. compute the normal
	// in global coordinates.
	if (normalR) {
		normal[0] = normalR[0];
		normal[1] = normalR[1];
		normal[2] = normalR[2];
	}
	else {
		xMul1_344 (normal,normalC,R1);
	}
	if (invert_normal) {
		normal[0] = -normal[0];
		normal[1] = -normal[1];
		normal[2] = -normal[2];
	}
	depth = -s;

	// compute contact point(s)

	if (code > 6) {
		// an edge from box 1 touches an edge from box 2.
		// find a point pa on the intersecting edge of box 1
		vec4_t pa,pb;
		f32 sign;f32 alpha,beta;
		vec4_t ua,ub;
		for (i=0; i<3; i++) pa[i] = p1[i];
		for (j=0; j<3; j++) {
			sign = (dDOT(normal,(R1+j*4)) > 0) ? (f32)(1.0) : (f32)(-1.0);
			xQ4ScaleNAdd(pa,&R1[j*4],sign * A[j]);
		}

		// find a point pb on the intersecting edge of box 2
		for (i=0; i<3; i++) pb[i] = p2[i];
		for (j=0; j<3; j++) {
			sign = (dDOT(normal,(R2+j*4)) > 0) ? (f32)(-1.0) : (f32)(1.0);
			xQ4ScaleNAdd(pb,&R2[j*4],sign * B[j]);
		}

		
		
		for (i=0; i<3; i++) ua[i] = R1[(((code)-7)/3)*4 + i];
		for (i=0; i<3; i++) ub[i] = R2[(((code)-7)%3)*4 + i];

		dLineClosestApproach (pa,ua,pb,ub,&alpha,&beta);
		for (i=0; i<3; i++) pa[i] += ua[i]*alpha;
		for (i=0; i<3; i++) pb[i] += ub[i]*beta;

		for (i=0; i<3; i++) contact[0].pos[i] = (f32)(0.5)*(pa[i]+pb[i]);
		contact[0].depth = depth;
		return 1;
	}

	// okay, we have a face-something intersection (because the separating
	// axis is perpendicular to a face). define face 'a' to be the reference
	// face (i.e. the normal vector is perpendicular to this) and face 'b' to be
	// the incident face (the closest face of the other box).

	if (code <= 3) {
		Ra = R1;
		Rb = R2;
		pa = p1;
		pb = p2;
		Sa = A;
		Sb = B;
	}
	else {
		Ra = R2;
		Rb = R1;
		pa = p2;
		pb = p1;
		Sa = B;
		Sb = A;
	}

	// nr = normal vector of reference face dotted with axes of incident box.
	// anr = absolute values of nr.
	
	if (code <= 3) {
		normal2[0] = normal[0];
		normal2[1] = normal[1];
		normal2[2] = normal[2];
	}
	else {
		normal2[0] = -normal[0];
		normal2[1] = -normal[1];
		normal2[2] = -normal[2];
	}
	xMul0_344 (nr,normal2,Rb);
	anr[0] = fabsr (nr[0]);
	anr[1] = fabsr (nr[1]);
	anr[2] = fabsr (nr[2]);

	// find the largest compontent of anr: this corresponds to the normal
	// for the indident face. the other axis numbers of the indicent face
	// are stored in a1,a2.
	if (anr[1] > anr[0]) {
		if (anr[1] > anr[2]) {
			a1 = 0;
			lanr = 1;
			a2 = 2;
		}
		else {
			a1 = 0;
			a2 = 1;
			lanr = 2;
		}
	}
	else {
		if (anr[0] > anr[2]) {
			lanr = 0;
			a1 = 1;
			a2 = 2;
		}
		else {
			a1 = 0;
			a2 = 1;
			lanr = 2;
		}
	}

	// compute center point of incident face, in reference-face coordinates
	
	if (nr[lanr] < 0) {
		for (i=0; i<3; i++) center[i] = pb[i] - pa[i] + Sb[lanr] * Rb[i+lanr*4];
	}
	else {
		for (i=0; i<3; i++) center[i] = pb[i] - pa[i] - Sb[lanr] * Rb[i+lanr*4];
	}

	// find the normal and non-normal axis numbers of the reference box	
	if (code <= 3) codeN = code-1; else codeN = code-4;
	if (codeN==0) {
		code1 = 1;
		code2 = 2;
	}
	else if (codeN==1) {
		code1 = 0;
		code2 = 2;
	}
	else {
		code1 = 0;
		code2 = 1;
	}

	// find the four corners of the incident face, in reference-face coordinates
	f32 quad[8];	// 2D coordinate of incident face (x,y pairs)
	f32 c1,c2,m11,m12,m21,m22;
	c1 = dDOT (center,(Ra+code1*4));
	c2 = dDOT (center,(Ra+code2*4));
	// optimize this? - we have already computed this data above, but it is not
	// stored in an easy-to-index format. for now it's quicker just to recompute
	// the four dot products.
	m11 = dDOT (Ra+code1*4,Rb+a1*4);
	m12 = dDOT (Ra+code1*4,Rb+a2*4);
	m21 = dDOT (Ra+code2*4,Rb+a1*4);
	m22 = dDOT (Ra+code2*4,Rb+a2*4);
	{
		f32 k1 = m11*Sb[a1];
		f32 k2 = m21*Sb[a1];
		f32 k3 = m12*Sb[a2];
		f32 k4 = m22*Sb[a2];
		quad[0] = c1 - k1 - k3;
		quad[1] = c2 - k2 - k4;
		quad[2] = c1 - k1 + k3;
		quad[3] = c2 - k2 + k4;
		quad[4] = c1 + k1 + k3;
		quad[5] = c2 + k2 + k4;
		quad[6] = c1 + k1 - k3;
		quad[7] = c2 + k2 - k4;
	}

	// find the size of the reference face
	f32 rect[2];
	rect[0] = Sa[code1];
	rect[1] = Sa[code2];

	// intersect the incident and reference faces
	f32 ret[16];
	s32 n = intersectRectQuad (rect,quad,ret);
	if (n < 1) return 0;		// this should never happen

	// convert the intersection points into reference-face coordinates,
	// and compute the contact position and depth for each point. only keep
	// those points that have a positive (penetrating) depth. delete points in
	// the 'ret' array as necessary so that 'point' and 'ret' correspond.
	f32 point[3*8];		// penetrating contact points
	f32 dep[8];			// depths for those points
	f32 det1 = 1.f/(m11*m22 - m12*m21);
	m11 *= det1;
	m12 *= det1;
	m21 *= det1;
	m22 *= det1;
	s32 cnum = 0;			// number of penetrating contact points found
	for (j=0; j < n; j++) {
		f32 k1 =  m22*(ret[j*2]-c1) - m12*(ret[j*2+1]-c2);
		f32 k2 = -m21*(ret[j*2]-c1) + m11*(ret[j*2+1]-c2);
		for (i=0; i<3; i++) point[cnum*3+i] =
			center[i] + k1*Rb[i+a1*4] + k2*Rb[i+a2*4];
		dep[cnum] = Sa[codeN] - dDOT(normal2,point+cnum*3);
		if (dep[cnum] >= 0) {
			ret[cnum*2] = ret[j*2];
			ret[cnum*2+1] = ret[j*2+1];
			cnum++;
		}
	}
	if (cnum < 1) return 0;	// this should never happen

	// we can't generate more contacts than we actually have
	if (maxc > cnum) maxc = cnum;
	if (maxc < 1) maxc = 1;

	if (cnum <= maxc) {
		// we have less contacts than we need, so we use them all
		for (j=0; j < cnum; j++) {
			cContact *con = &contact[j];
			for (i=0; i<3; i++) con->pos[i] = point[j*3+i] + pa[i];
			con->depth = dep[j];
		}
	}
	else {
		// we have more contacts than are wanted, some of them must be culled.
		// find the deepest point, it is always the first contact.
		s32 i1 = 0;
		f32 maxdepth = dep[0];
		for (i=1; i<cnum; i++) {
			if (dep[i] > maxdepth) {
				maxdepth = dep[i];
				i1 = i;
			}
		}

		//s32 iret[8];
		//cullPoints (cnum,ret,maxc,i1,iret);

		for (j=0; j < maxc; j++) {
			cContact *con = &contact[j];
			for (i=0; i<3; i++) con->pos[i] = point[j*3+i] + pa[i];
			con->depth = dep[j];
		}
		cnum = maxc;
	}
	return cnum;
}