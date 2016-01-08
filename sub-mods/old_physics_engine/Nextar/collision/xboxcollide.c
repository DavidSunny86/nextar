//@ Abhishek Dey
//@ Oct 24 2007
//@ Oct 25 2007
//@ Nov 17 2007

#include <xmath_c.h>
#include <xcontact.h>
#include <xmemory.h>

static void cCullPoints (s32 n, f32 p[], s32 m, s32 i0, s32 iret[])
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

static s32 cIntersectRectQuad (f32 *h, f32 *p, f32 *ret)
{
	s32 nq,nr;
	f32 buffer[16];
	f32 *q,*r;
	s32 dir,i,sign;

	nq=4;
	
	q = p;
	r = ret;
	
	for (dir=0; dir <= 1; dir++) 
	{
		// direction notation: xy[0] = x axis, xy[1] = y axis
		for (sign=-1; sign <= 1; sign += 2) 
		{
			f32 *pq,*pr;
			// chop q along the line xy[dir] = sign*h[dir]
			pq = q;
			pr = r;
			nr = 0;
			for (i=nq; i > 0; i--) 
			{
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
	if (q != ret) copyDwords(ret,q,nr*2);
	return nr;
}

void cLineClosestApproach( const f32* pa, const f32* ua,
							const f32* pb, const f32* ub,
							f32 *alpha, f32 *beta)
{	vec4_t p;
	f32 uaub,q1,q2,d;

	xQ4Sub(p,pb,pa);
	xVDot(&uaub,ua,ub);
	xVDot(&q1,ua,p);
	q2 = -xVDotR(ub,p);
	d = 1-uaub*uaub;
	if (d <= (0.0001f)) 
	{
		*alpha = 0;
		*beta  = 0;
	}
	else 
	{
		*alpha = (q1 + uaub*q2)/d;
		*beta  = (uaub*q1 + q2)/d;
	}
}

s32 cBoxCollide(const f32* p1, const f32* r1, const f32* a,
				const f32* p2, const f32* r2, const f32* b,
				cContact* contact,f32* normal)
{
	const f32 fudge_factor = (f32)(1.05);
	s32 maxc = N3DMaxContact;
	vec4_t p,pp1,pp2,normalC;
	mat34_t r,q;
	const f32* normalR = 0;
	f32 s,depth;
	s32 invert_normal = 0,code = 0;
	s32 j;

	xQ4Sub(p,p2,p1);
	xMul0_344(pp1,p,r1);
	xMul0_344(pp2,p,r2);
	xMul0_34(r,r1,r2);
	q[0] = fabsr(r[0]); q[1] = fabsr(r[1]); q[2] = fabsr(r[2]); q[3] = 0;
	q[4] = fabsr(r[4]); q[5] = fabsr(r[5]); q[6] = fabsr(r[6]); q[7] = 0;
	q[8] = fabsr(r[8]); q[9] = fabsr(r[9]); q[10] = fabsr(r[10]); q[11] = 0;

#define SATTest(exp1,exp2,norm,cc)\
	s2 = fabsr(exp1)-exp2;\
	if(!X_IsFloatNeg(s2)) return 0;\
	if(s2 > s) \
	{\
		s = s2; normalR = (norm);\
		invert_normal = ((exp1) < 0);\
		code = cc;\
	}

	{
		f32 s2;
		s =-N3DInfinity;
		SATTest(pp1[0], (a[0] + xVDotR(b,q+0)), r1+0, 1);
		SATTest(pp1[1], (a[1] + xVDotR(b,q+4)), r1+4, 2);
		SATTest(pp1[2], (a[2] + xVDotR(b,q+8)), r1+8, 3);

		// misuse normalC
		xMul1_344(normalC,a,q);
		SATTest(pp2[0], (b[0] + normalC[0]), r2+0, 4);
		SATTest(pp2[1], (b[1] + normalC[1]), r2+4, 5);
		SATTest(pp2[2], (b[2] + normalC[2]), r2+8, 6);

#undef SATTest

#define SATTest(exp1,exp2,n1,n2,n3,cc) \
	s2 = fabsr(exp1)-exp2;\
	if(!X_IsFloatNeg(s2)) return 0;\
	l = fsqrtr((n1)*(n1) + (n2)*(n2) + (n3)*(n3)); \
	if(!X_IsFloatNeg(l)) {\
		s2 /= l;\
		if(fudge_factor*s2>s) {\
			s = s2;\
			normalR = 0;\
			normalC[0] = (n1)/l; normalC[1] = (n2)/l; normalC[2] = (n3)/l; \
			invert_normal = ((exp1) < 0); \
			code = cc;\
		}\
	}


		{
			vec4_t tmp1,tmp2;
			f32 l;
			// separating axis = u1 x (v1,v2,v3)
			xQ4ScaleS(tmp1,r+4,pp1[2]);
			xQ4ScaleNSub(tmp1,r+8,pp1[1]);
			xQ4ScaleS(tmp2,q+8,a[1]);
			xQ4ScaleNAdd(tmp2,q+4,a[2]);
		
			SATTest(tmp1[0],(tmp2[0]+b[1]*q[2]+b[2]*q[1]),0,-r[8],r[4],7);
			SATTest(tmp1[1],(tmp2[1]+b[0]*q[2]+b[2]*q[0]),0,-r[9],r[5],8);
			SATTest(tmp1[2],(tmp2[2]+b[0]*q[1]+b[1]*q[0]),0,-r[10],r[6],9);

			// separating axis = u2 x (v1,v2,v3)
			xQ4ScaleS(tmp1,r+8,pp1[0]);
			xQ4ScaleNSub(tmp1,r+0,pp1[2]);
			xQ4ScaleS(tmp2,q+8,a[0]);
			xQ4ScaleNAdd(tmp2,q+0,a[2]);

			SATTest(tmp1[0],(tmp2[0]+b[1]*q[6]+b[2]*q[5]),r[8],0,-r[0],10);
			SATTest(tmp1[1],(tmp2[1]+b[0]*q[6]+b[2]*q[4]),r[9],0,-r[1],11);
			SATTest(tmp1[2],(tmp2[2]+b[0]*q[5]+b[1]*q[4]),r[10],0,-r[2],12);

			// separating axis = u3 x (v1,v2,v3)
			xQ4ScaleS(tmp1,r+0,pp1[1]);
			xQ4ScaleNSub(tmp1,r+4,pp1[0]);
			xQ4ScaleS(tmp2,q+4,a[0]);
			xQ4ScaleNAdd(tmp2,q+0,a[1]);

			SATTest(tmp1[0],(tmp2[0]+b[1]*q[10]+b[2]*q[9]),-r[4],r[0],0,13);
			SATTest(tmp1[1],(tmp2[1]+b[0]*q[10]+b[2]*q[8]),-r[5],r[1],0,14);
			SATTest(tmp1[2],(tmp2[2]+b[0]*q[9]+b[1]*q[8]),-r[6],r[2],0,15);
		}
	}
#undef SATTest

	if (!code) return 0;
	if(normalR) 
	{
		xQ4CpyMac(normal,normalR);
	}
	else 
	{
		xMul1_344 (normal,normalC,r1);
	}
	if(invert_normal)
		xQ4Scale(normal,-1);

	depth = -s;

	if(code > 6)
	{
		// an edge from box 1 touches an edge from box 2.
		// find a point pa on the intersecting edge of box 1
		vec4_t pa,pb;
		vec4_t ua,ub;
		f32 alpha,beta;
		s32 j;


		xQ4CpyMac(pa,p1);
		xQ4CpyMac(pb,p2);

		xMul0_344(ua,normal,r1);
		xMul0_344(ub,normal,r2);
		for (j=0; j<3; j++) 
		{
			// add sign
			X_IsFloatNeg(ua[j]) ? xQ4ScaleNSub(pa,&r1[j*4],a[j]) : xQ4ScaleNAdd(pa,&r1[j*4],a[j]);
			X_IsFloatNeg(ub[j]) ? xQ4ScaleNSub(pb,&r2[j*4],b[j]) : xQ4ScaleNAdd(pb,&r2[j*4],b[j]);
		}

		xQ4CpyMac(ua,&r1[(((code)-7)/3)*4]);
		xQ4CpyMac(ub,&r1[(((code)-7)%3)*4]);

		cLineClosestApproach(pa,ua,pb,ub,&alpha,&beta);
		xQ4ScaleNAdd(pa,ua,alpha);
		xQ4ScaleNAdd(pb,ub,beta);
		xQ4Add(contact[0].pos,pa,pb);
		xQ4Scale(contact[0].pos,0.5f);
		contact[0].depth = depth;
		return 1;
	}
	else
	{
		vec4_t normal2,nr,anr,center;
		vec4_t	m_;
		vec4_t	point[8];
		f32		quad[8];
		f32		rect[2];
		f32		ret[16];
		f32		dep[8];
		s32		cnum,n;
		f32		c1,c2;
		const f32 *Ra,*Rb,*pa,*pb,*Sa,*Sb;
		s32 a1,a2,lanr,codeN,code1,code2;
		
	
		if (code <= 3) 
		{
			Ra = r1;
			Rb = r2;
			pa = p1;
			pb = p2;
			Sa = a;
			Sb = b;
			xQ4CpyMac(normal2,normal);
		}
		else 
		{
			Ra = r2;
			Rb = r1;
			pa = p2;
			pb = p1;
			Sa = b;
			Sb = a;
			xQ4ScaleS(normal2,normal,-1.f);
		}
		xMul0_344 (nr,normal2,Rb);
		anr[0] = fabsr (nr[0]);
		anr[1] = fabsr (nr[1]);
		anr[2] = fabsr (nr[2]);
		if (anr[1] > anr[0]) 
		{
			if (anr[1] > anr[2]) 
			{
				a1 = 0;
				lanr = 1;
				a2 = 2;
			}
			else 
			{
				a1 = 0;
				a2 = 1;
				lanr = 2;
			}
		}
		else 
		{
			if (anr[0] > anr[2]) 
			{
				lanr = 0;
				a1 = 1;
				a2 = 2;
			}
			else 
			{
				a1 = 0;
				a2 = 1;
				lanr = 2;
			}
		}

		xQ4Sub(center,pb,pa);
		X_IsFloatNeg(nr[lanr]) ? xQ4ScaleNAdd(center,&Rb[lanr*4],Sb[lanr]) : 
			xQ4ScaleNSub(center,&Rb[lanr*4],Sb[lanr]);
		// find the normal and non-normal axis numbers of the reference box	
		if (code <= 3) 
			codeN = code-1; else codeN = code-4;
		if (codeN==0) 
		{
			code1 = 1;
			code2 = 2;
		}
		else if (codeN==1) 
		{
			code1 = 0;
			code2 = 2;
		}
		else 
		{
			code1 = 0;
			code2 = 1;
		}

		if(code <= 3) 
		{
			m_[0] = r[code1*4+a1];
			m_[1] = r[code1*4+a2];
			m_[2] = r[code2*4+a1];
			m_[3] = r[code2*4+a2];
		}
		else
		{
			m_[0] = r[a1*4+code1];
			m_[1] = r[a2*4+code1];
			m_[2] = r[a1*4+code2];
			m_[3] = r[a2*4+code2];
		}
		
		xVDot(&c1,center,(Ra+code1*4));
		xVDot(&c2,center,(Ra+code2*4));

		{			
			f32 k1 = m_[0]*Sb[a1];
			f32 k2 = m_[2]*Sb[a1];
			f32 k3 = m_[1]*Sb[a2];
			f32 k4 = m_[3]*Sb[a2];
			quad[0] = c1 - k1 - k3;
			quad[1] = c2 - k2 - k4;
			quad[2] = c1 - k1 + k3;
			quad[3] = c2 - k2 + k4;
			quad[4] = c1 + k1 + k3;
			quad[5] = c2 + k2 + k4;
			quad[6] = c1 + k1 - k3;
			quad[7] = c2 + k2 - k4;
		}

		rect[0] = Sa[code1];
		rect[1] = Sa[code2];
		n = cIntersectRectQuad(rect,quad,ret);
		X_Assert(n >= 1);

		{
			f32 det1 = 1/(m_[0]*m_[3] - m_[1]*m_[2]);
			xQ4Scale(m_,det1);
			cnum = 0;
		}
		for (j=0; j < n; j++) 
		{
			f32 st1,st2,k1,k2;
			st1 = ret[j*2]-c1;st2 = ret[j*2+1]-c2;
			k1 =  m_[3]*(st1) - m_[1]*(st2);
			k2 = -m_[2]*(st1) + m_[0]*(st2);
			xQ4CpyMac(point[cnum],center);
			xQ4ScaleNAdd(point[cnum],&Rb[a1*4],k1);
			xQ4ScaleNAdd(point[cnum],&Rb[a2*4],k2);
			dep[cnum] = Sa[codeN] - xVDotR(normal2,point[cnum]);
			if(dep[cnum] >= 0) 
			{
				ret[cnum*2] = ret[j*2];
				ret[cnum*2+1] = ret[j*2+1];
				cnum++;
			}
		}

		X_Assert(cnum >= 1);
		if (cnum <= maxc) 
		{
			// we have less contacts than we need, so we use them all
			for (j=0; j < cnum; j++) {
				cContact *con = &contact[j];
				xQ4Add(con->pos,point[j],pa);
				con->depth = dep[j];
			}
		}
		else
		{
			// we have more contacts than are wanted, some of them must be culled.
			// find the deepest point, it is always the first contact.
			s32 iret[8];
			s32 i1 = 0;
			f32 maxdepth = dep[0];
			for (j=1; j<cnum; j++) {
				if (dep[j] > maxdepth) {
					maxdepth = dep[j];
					i1 = j;
				}
			}

			cCullPoints (cnum,ret,maxc,i1,iret);
			for (j=0; j < maxc; j++) {
				cContact *con = &contact[j];
				xQ4Add(con->pos,point[iret[j]],pa);
				con->depth = dep[iret[j]];
			}
			cnum = maxc;
		}	
		return cnum;
	}
#ifdef X_DEBUG
	return 0;
#endif
}