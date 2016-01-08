//@# Abhishek Dey
//@# Oct 24 2007
//@# Oct 25 2007

#include <xmath_c.h>
#include <xcontact.h>
#include <xmemory.h>

s32 cBoxCollide( 
				const f32* p1,const f32* r1,const f32* sz1, 
				const f32* p2,const f32* r2,const f32* sz2,
				cContact* c,f32* normal)
{
	const f32 fudge_factor = 1.05f;
	vec4_t p,pp1,pp2,nC; //secured_v
	mat34_t r,q;
	f32	s2,s;
	s32 code;
	const f32* nR;
	//mat34_t r1,r2;
	s32 invertnormal,i,j;
	nC[3] = 0;
	//xMTrans_34(r1,r1t);
	//xMTrans_34(r2,r2t);

	xQ4Sub(p,p2,p1);
	xMul0_344(pp1,p,r1);
	xMul0_344(pp2,p,r2);
	// r(i,j) = row(r1,i). row(r2,j)
	xMul0_34(r,r1,r2);
	q[0] = fabsr(r[0]); q[1] = fabsr(r[1]); q[2] = fabsr(r[2]); q[3] = 0;
	q[4] = fabsr(r[4]); q[5] = fabsr(r[5]); q[6] = fabsr(r[6]); q[7] = 0;
	q[8] = fabsr(r[8]); q[9] = fabsr(r[9]); q[10] = fabsr(r[10]); q[11] = 0;

#define XSATEST(e1,e2,n,cc) \
	s2 = fabsr(e1) - (e2); \
	if (s2 > 0 ) return 0;	\
	if (s2 > s) \
	{ \
	s = s2; nR = (n); \
	invertnormal = ((e1) < 0); \
	code = (cc); \
	}

	s =-N3DInfinity;
	invertnormal = 0;
	code = 0;
	// note that nC has not been used yet
	// we use nC for sz1 rough calculation 
	// here
	// separating axis = u1,u2,u3
	xMul0_344(nC,sz2,q);		// 3 multiplication with SSE
	xQ4Add(nC,nC,sz1);		// one addition with SSE
	//TODO: HERE we need to take a descision, we can
	//well over escape 3 multiplication for 1, in case
	//we use xVDot for each test, but for average case
	//I guess this will work better
	XSATEST(pp1[0],nC[0],r1+0,1);
	XSATEST(pp1[1],nC[1],r1+4,2);
	XSATEST(pp1[2],nC[2],r1+8,3);

	xMul1_344(nC,sz1,q);		// 3 multiplication with SSE
	xQ4Add(nC,nC,sz2);		// one addition with SSE
	// separating axis = v1,v2,v3
	XSATEST(pp2[0],nC[0],r2+0,4);
	XSATEST(pp2[1],nC[1],r2+4,5);
	XSATEST(pp2[2],nC[2],r2+8,6);

	// note that nC has not been used yet

#undef XSATEST
#define XSATEST(expr1,e2,n1,n2,n3,cc) \
	s2 = fabsr(expr1) - (e2); \
	if (s2 > 0) return 0; \
	l = sqrtr((n1)*(n1) + (n2)*(n2) + (n3)*(n3)); \
	if (l > 0) { \
	s2 /= l; \
	if (s2*fudge_factor > s) { \
	s = s2; \
	nR = 0; \
	nC[0] = (n1)/l; nC[1] = (n2)/l; nC[2] = (n3)/l; \
	invertnormal = ((expr1) < 0); \
	code = (cc); \
	} \
	}

	{
		// we need some temp vectors here
		vec4_t tmp1,tmp2;
		f32  l;
		// with SSE these are effectively
		xQ4ScaleS(tmp1,(f32*)&r[4],pp1[2]);	// one multiplication
		xQ4ScaleNSub(tmp1,(f32*)&r[8],pp1[1]);	// one addition and one multiplication
		xQ4ScaleS(tmp2,(f32*)&q[8],sz1[1]);		// one multiplication
		xQ4ScaleNAdd(tmp2,(f32*)&q[4],sz1[2]); // one addition and one multiplication

		// separating axis = u1 x (v1,v2,v3)
		XSATEST(tmp1[0],(tmp2[0]+sz2[1]*q[2]+sz2[2]*q[1]),0,-r[8],r[4],7);
		XSATEST(tmp1[1],(tmp2[1]+sz2[0]*q[2]+sz2[2]*q[0]),0,-r[9],r[5],8);
		XSATEST(tmp1[2],(tmp2[2]+sz2[0]*q[1]+sz2[1]*q[0]),0,-r[10],r[6],9);

		xQ4ScaleS(tmp1,(f32*)&r[8],pp1[0]);	// one multiplication
		xQ4ScaleNSub(tmp1,(f32*)&r[0],pp1[2]);	// one addition and one multiplication
		xQ4ScaleS(tmp2,(f32*)&q[8],sz1[0]);		// one multiplication
		xQ4ScaleNAdd(tmp2,(f32*)&q[0],sz1[2]); // one addition and one multiplication

		// separating axis = u2 x (v1,v2,v3)
		XSATEST(tmp1[0],(tmp2[0]+sz2[1]*q[6]+sz2[2]*q[5]),r[8],0,-r[0],10);
		XSATEST(tmp1[1],(tmp2[1]+sz2[0]*q[6]+sz2[2]*q[4]),r[9],0,-r[1],11);
		XSATEST(tmp1[2],(tmp2[2]+sz2[0]*q[5]+sz2[1]*q[4]),r[10],0,-r[2],12);

		xQ4ScaleS(tmp1,(f32*)&r[0],pp1[1]);	// one multiplication
		xQ4ScaleNSub(tmp1,(f32*)&r[4],pp1[0]);	// one addition and one multiplication
		xQ4ScaleS(tmp2,(f32*)&q[4],sz1[0]);		// one multiplication
		xQ4ScaleNAdd(tmp2,(f32*)&q[0],sz1[1]); // one addition and one multiplication

		// separating axis = u3 x (v1,v2,v3)
		XSATEST(tmp1[0],(tmp2[0]+sz2[1]*q[10]+sz2[2]*q[9]),-r[4],-r[0],0,13);
		XSATEST(tmp1[1],(tmp2[1]+sz2[0]*q[10]+sz2[2]*q[8]),-r[5],-r[1],0,14);
		XSATEST(tmp1[2],(tmp2[2]+sz2[0]*q[9]+sz2[1]*q[8]),-r[6],-r[2],0,15);
	}

#undef XSATEST
	if(!code) return 0;
	if(nR)
	{
		xQ4CpyMac(normal,nR);
	}
	else
	{
		xMul1_344(normal,nC,r1);
	}
	if(invertnormal)
	{
		xQ4Scale(normal,-1.0f);
	//	normal[0] = -normal[0];
	//	normal[1] = -normal[1];
	//	normal[2] = -normal[2];
	}

	s = -s;

	if (code > 6) 
	{
		// an edge from box 1 touches an edge from box 2.
		// find a point pa on the intersecting edge of box 1
		vec4_t	pa,pb,ua,ub;
		f32		alpha,beta;

		xQ4CpyMac(pa,p1);
		xQ4CpyMac(pb,p2);

		xMul0_344(ua,normal,r1);		

		for (j=0; j<3; j++) 
		{
			// add sign
			if( ISFLOATNEGETIVE(ua[j]) )
			{
				xQ4ScaleNSub(pa,&r1[j*4],sz1[j]);
			}
			else 
			{
				xQ4ScaleNAdd(pa,&r1[j*4],sz1[j]);
			}
		}

		// find a point pb on the intersecting edge of box 2
		xMul0_344(ua,normal,r2);		
		for (j=0; j<3; j++) 
		{			
			// add sign
			if( ISFLOATNEGETIVE(ua[j]) )
			{
				xQ4ScaleNSub(pb,&r2[j*4],sz2[j]);
			}
			else 
			{
				xQ4ScaleNAdd(pb,&r2[j*4],sz2[j]);
			}
		}

		// highly doubted
		xQ4CpyMac(ua,&r1[(((code)-7)/3)*4]);
		xQ4CpyMac(ub,&r2[(((code)-7)%3)*4]);
	
		{
			// line closest approach (pa,ua,pb,ub,&alpha,&beta);
			vec4_t p;// secured_v
			f32 uaub,q1,q2,d;

			xQ4Sub(p,pb,pa);
			xVDot(&uaub,ua,ub);
			xVDot(&q1,ua,p);
			xVDot(&q2,ub,p);
			d = 1-uaub*uaub;
			if (d <= 0.0001f) 
			{
				alpha = 0;
				beta  = 0;
			}
			else 
			{
				d = 1/d;
				alpha = (q1 - uaub*q2)*d;
				beta  = (uaub*q1 - q2)*d;
			}
		}

		xQ4ScaleNAdd(pa,ua,alpha);
		xQ4ScaleNAdd(pb,ub,beta);
		xQ4Add(c->pos,pa,pb);
		xQ4Scale(c->pos,0.5f);
		c->depth = s;
		return 1;
	}

	// okay, we have sz1 face-something intersection (because the separating
	// axis is perpendicular to sz1 face). define face 'sz1' to be the reference
	// face (i.e. the normal vector is perpendicular to this) and face 'sz2' to be
	// the incident face (the closest face of the other box).  
	{
		vec4_t center; // secured_v
		const f32 *ra,*rb;
		const f32 *pa,*pb,*sa,*sb;
		s32 lanr,a1,a2;
		f32 quad[8];	// 2D coordinate of incident face (x,y pairs)
		// nr = normal vector of reference face dotted with axes of incident box.
		// anr = absolute values of nr.
		vec4_t normal2,nr,anr;	// secured_v
		if (code <= 3) 
		{
			ra = r1;
			rb = r2;
			pa = (f32*)p1;
			pb = (f32*)p2;
			sa = (f32*)sz1;
			sb = (f32*)sz2;
			xQ4CpyMac(normal2,normal);
		}
		else 
		{    
			ra = r2;
			rb = r1;
			pa = (f32*)p2;
			pb = (f32*)p1;
			sa = (f32*)sz2;
			sb = (f32*)sz1;
			normal2[0] = -normal[0];
			normal2[1] = -normal[1];
			normal2[2] = -normal[2];
			normal2[3] = 0;
		}

		xMul0_344(nr,normal2,rb);
		anr[0] = fabsr (nr[0]);
		anr[1] = fabsr (nr[1]);
		anr[2] = fabsr (nr[2]);
		anr[3] = 0; // secured_v

		// find the largest compontent of anr: this corresponds to the normal
		// for the indident face. the other axis numbers of the indicent face
		// are stored in a1,a2.

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
		// compute center point of incident face, in reference-face coordinates
		xQ4Sub(center,pb,pa);
		if (nr[lanr] < 0) 
		{
			xQ4ScaleNAdd(center,&rb[4*lanr],sb[lanr]);
		}
		else 
		{   
			xQ4ScaleNSub(center,&rb[4*lanr],sb[lanr]);
		}

		// find the normal and non-normal axis numbers of the reference box
		{
			vec4_t point[8];		// penetrating contact points
			vec4_t m_;
			s32 codeN,code1,code2;
			f32 c1,c2;
			f32 rect[2];
			f32 ret[16];
			s32 inr;
			s32 cnum = 0;			// number of penetrating contact points found

			f32 dep[8];			// depths for those points
			f32 det1;

			if (code <= 3) 
				codeN = code-1; 
			else 
				codeN = code-4;
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

			// find the four corners of the incident face, in reference-face coordinates
			{
				xVDot(&c1,center,&ra[code1*4]);
				xVDot(&c2,center,&ra[code1*4]);
			}
			// r(i,j) = col(r1,i). col(r2,j)
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
			{
				f32 k1 = m_[0]*sb[a1];
				f32 k2 = m_[2]*sb[a1];
				f32 k3 = m_[1]*sb[a2];
				f32 k4 = m_[3]*sb[a2];
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
			rect[0] = sa[code1];
			rect[1] = sa[code2];
			
			// intersect the incident and reference faces
			{
				// s32 n = intersectRectQuad (rect,quad,ret);
				//(rect[2],p[8],ret[16])
				// q (and r) contain nq (and nr) coordinate points for the current (and
				// chopped) polygons
				s32 nq=4;
				f32 buffer[16];
				f32 *q = quad;
				f32 *r = ret;
				s32 dir,sign;
				for (dir=0; dir <= 1; dir++) 
				{
					// direction notation: xy[0] = x axis, xy[1] = y axis
					for (sign=-1; sign <= 1; sign += 2) 
					{
						// chop q along the line xy[dir] = sign*rect[dir]
						f32 *pq = q;
						f32 *pr = r;
						inr = 0;
						for (i=nq; i > 0; i--) 
						{
							f32 *nextq;
							// go through all points in q and all lines between adjacent points
							if (sign*pq[dir] < rect[dir]) 
							{
								// this point is inside the chopping line
								pr[0] = pq[0];
								pr[1] = pq[1];
								pr += 2;
								inr++;
								if (inr & 8) 
								{
									q = r;
									goto done;
								}
							}
							nextq = (i > 1) ? pq+2 : q;
							if ((sign*pq[dir] < rect[dir]) ^ (sign*nextq[dir] < rect[dir])) 
							{
								// this line crosses the chopping line
								pr[1-dir] = pq[1-dir] + (nextq[1-dir]-pq[1-dir]) /
									(nextq[dir]-pq[dir]) * (sign*rect[dir]-pq[dir]);
								pr[dir] = sign*rect[dir];
								pr += 2;
								inr++;
								if (inr & 8) 
								{
									q = r;
									goto done;
								}
							}
							pq += 2;
						}
						q = r;
						r = (q==ret) ? buffer : ret;
						nq = inr;
					}
				}
done:
				if (q != ret) 
					copyDwords(ret,q,inr*2);
			}

			if (inr < 1) return 0;		// this should never happen

			// convert the intersection points into reference-face coordinates,
			// and compute the contact position and depth for each point. only keep
			// those points that have sz1 positive (penetrating) depth. delete points in
			// the 'ret' array as necessary so that 'point' and 'ret' correspond.
			det1 = 1/(m_[0]*m_[3] - m_[1]*m_[2]);
			xQ4Scale(m_,det1);
			for (j=0; j < inr; j++) 
			{
				f32 k1 =  m_[3]*(ret[j*2]-c1) - m_[1]*(ret[j*2+1]-c2);
				f32 k2 = -m_[2]*(ret[j*2]-c1) + m_[0]*(ret[j*2+1]-c2);
				xQ4CpyMac(point[cnum],center);
				xQ4ScaleNAdd(point[cnum],&rb[a1*4],k1);
				xQ4ScaleNAdd(point[cnum],&rb[a2*4],k2);
				dep[cnum] = sa[codeN] - xVDotR(normal2,point[cnum]);
				if(dep[cnum] >= 0) 
				{
					ret[cnum*2] = ret[j*2];
					ret[cnum*2+1] = ret[j*2+1];
					cnum++;
				}
			}

			if (cnum < 1) 
				return 0;	// this should never happen
			// we can't generate more contacts than we actually have
			if (cnum <= N3DMaxContact) 
			{
				// we have less contacts than we need, so we use them all
				for (j=0; j < cnum; j++) 
				{
					cContact *con = c+j;
					xQ4Add(con->pos,point[j],pa);
					con->depth = dep[j];	// this seems ok
				}
			}
			else 
			{
				// we have more contacts than are wanted, some of them must be culled.
				// find the deepest point, it is always the first contact.
				s32 i1 = 0;
				f32 maxdepth = dep[0];
				s32 iret[8];
				for (i=1; i<cnum; i++) 
				{
					if (dep[i] > maxdepth) 
					{
						maxdepth = dep[i];
						i1 = i;
					}
				}

				{
					// compute the centroid of the polygon in cx,cy
					f32 a,cx,cy,q;
					f32 ang[8];
					s32 avail[8];
					if (cnum==1) 
					{
						cx = ret[0];
						cy = ret[1];
					}
					else if (cnum==2) 
					{
						cx = 0.5f*(ret[0] + ret[2]);
						cy = 0.5f*(ret[1] + ret[3]);
					}
					else 
					{
						a = 0;
						cx = 0;
						cy = 0;
						for (i=0;i<(cnum-1); i++) 
						{      
							q = ret[i*2]*ret[i*2+3] - ret[i*2+2]*ret[i*2+1];
							a += q;
							cx += q*(ret[i*2]+ret[i*2+2]);
							cy += q*(ret[i*2+1]+ret[i*2+3]);
						}
						q = ret[cnum*2-2]*ret[1] - ret[0]*ret[cnum*2-1];
						a = 1/((3*(a+q)));
						cx = a*(cx + q*(ret[cnum*2-2]+ret[0]));
						cy = a*(cy + q*(ret[cnum*2-1]+ret[1]));
					}

					// compute the angle of each point w.r.t. the centroid
					for (i=0; i<cnum; i++) 
						ang[i] = atan2r(p[i*2+1]-cy,p[i*2]-cx);
					// search for points that have angles closest to sz1[i1] + i*(2*pi/m).

					for (i=0; i<cnum; i++) avail[i] = 1;
					avail[i1] = 0;
					iret[0] = i1;
					{
						s32* piret = iret;
						piret++;
						for (j=1; j<N3DMaxContact; j++) 
						{
							f32 maxdiff=1e9,diff;
							a = (f32)j*(2*N3DPi/N3DMaxContact) + ang[i1];
							if(a > N3DPi) a -= 2*N3DPi;
							
							for (i=0; i<cnum; i++) 
							{
								if (avail[i]) 
								{	
									diff = fabsr (ang[i]-a);
									if (diff > N3DPi) diff = 2*N3DPi - diff;
									if (diff < maxdiff) 
									{
										maxdiff = diff;
										*piret = i;
									}
								}
							}
							avail[*piret] = 0;
							piret++;
						}
					}
				}

				for (j=0; j < N3DMaxContact; j++) 
				{
					cContact *con = c+j;
					xQ4Add(con->pos,point[iret[j]],pa);
					con->depth = dep[iret[j]];	// this seems ok
				}
				cnum = N3DMaxContact;
			}
			return cnum;
		}
	}
	// return_code = code;	
}

