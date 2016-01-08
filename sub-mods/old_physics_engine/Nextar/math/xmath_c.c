//@ Abhishek Dey
//@ 1/17/2007

#include <xmath_c.h>
#ifndef X_SSE_INCLUDE
// the functions hereunder has 3 version as per the support of sse 
// is included
void nQuatMul(f32* qa,const f32* qb,const f32* qc)
{
	// qa = qb*qc
	qa[0] = qb[0]*qc[0] - qb[1]*qc[1] - qb[2]*qc[2] - qb[3]*qc[3];
	qa[1] = qb[0]*qc[1] + qb[1]*qc[0] + qb[2]*qc[3] - qb[3]*qc[2];
	qa[2] = qb[0]*qc[2] - qb[1]*qc[3] + qb[2]*qc[0] + qb[3]*qc[1];
	qa[3] = qb[0]*qc[3] + qb[1]*qc[2] - qb[2]*qc[1] + qb[3]*qc[0];
}

void nWtoDQuat(f32* dq, const f32* w, const f32* q)
{
  dq[0] = (0.5f)*(- w[0]*q[1] - w[1]*q[2] - w[2]*q[3]);
  dq[1] = (0.5f)*(  w[0]*q[0] + w[1]*q[3] - w[2]*q[2]);
  dq[2] = (0.5f)*(- w[0]*q[3] + w[1]*q[0] + w[2]*q[1]);
  dq[3] = (0.5f)*(  w[0]*q[2] - w[1]*q[1] + w[2]*q[0]);
}
#endif
const mat44_t N3D4x4IdentityMatrix		= {1,0,0,0, 0,1,0,0, 0,0,1,0,	0,0,0,1};
// these are generic functions
void nRSetIdentity(f32* r)
{
	r[0] = 1;	r[1] = 0;	r[2] = 0;	r[3] = 0;
	r[4] = 0;	r[5] = 1;	r[6] = 0;	r[7] = 0;
	r[8] = 0;	r[9] = 0;	r[10] = 1;	r[11] = 0;
}

void nQuatFromAxisAngle(f32* store,const f32* axis,const f32 ang)
{
	f32 sin_a = sinr((ang)/2);
	store[QW] = cosr((ang)/2);
	store[QA] = sin_a * axis[0];
	store[QB] = sin_a * axis[1];
	store[QC] = sin_a * axis[2];
}

void nQuatFromEulerAngles(f32* store,const f32 yaw,const f32 pitch,const f32 roll)
{
    f32 cosYaw = cosr(yaw / 2);
    f32 sinYaw = sinr(yaw / 2);
    f32 cosPitch = cosr(pitch / 2);
    f32 sinPitch = sinr(pitch / 2);
    f32 cosRoll = cosr(roll / 2);
    f32 sinRoll = sinr(roll / 2);
	f32 cosRollcosPitch = cosRoll*cosPitch;
	f32 cosRollsinPitch = cosRoll*sinPitch;
	f32 sinRollcosPitch = sinRoll*cosPitch;
	f32 sinRollsinPitch = sinRoll*sinPitch;

	store[QW] = cosRollcosPitch * cosYaw + sinRollsinPitch * sinYaw;
	store[QA] = cosRollsinPitch * cosYaw + sinRollcosPitch * sinYaw;
	store[QB] = cosRollcosPitch * sinYaw - sinRollsinPitch * cosYaw;
	store[QC] = sinRollcosPitch * cosYaw - cosRollsinPitch * sinYaw;

}

void nMatrixFromAxisAngle(f32* m,const f32* axis,const f32 ang)
{
	f32 cs,xy1_c,xz1_c,yz1_c;
	vec4_t xyzs,mstr;
	
	cs = cosr(ang);
	{
		f32 _1_c,s;
		s = sinr(ang);
		_1_c = 1.0f - cs;
		xQ4ScaleS(xyzs,axis,s);
		xQ4Mul(mstr,axis,axis);
		xQ4Scale(mstr,_1_c);
		xy1_c = axis[0]*axis[1]*_1_c;
		xz1_c = axis[0]*axis[2]*_1_c;
		yz1_c = axis[1]*axis[2]*_1_c;
	}
	
	m[0] = cs + mstr[0];
	m[1] = xy1_c - xyzs[2];
	m[2] = xz1_c + xyzs[1];

	m[4] = xy1_c + xyzs[2];
	m[5] = cs + mstr[1];
	m[6] = yz1_c - xyzs[0];

	m[8] = xz1_c - xyzs[1];
	m[9] = yz1_c + xyzs[0];
	m[10]= cs + mstr[2];
}

void nMatrixCameraLH(f32* m,const f32* eye,const f32* lookat,const f32* up)
{
	vec4_t zaxis,yaxis,xaxis;
	//-(looat-eye)
	xQ4Sub(zaxis,lookat,eye);
	xQ4Normalize(zaxis);
	xVCross(xaxis,up,zaxis);
	xQ4Normalize(xaxis);
	xVCross(yaxis,zaxis,xaxis);
	m[0 ] = xaxis[0];
	m[1 ] = yaxis[0];
	m[2 ] = zaxis[0];
	m[3 ] = 0;
	m[4 ] = xaxis[1];
	m[5 ] = yaxis[1];
	m[6 ] = zaxis[1];
	m[7 ] = 0;
	m[8 ] = xaxis[2];
	m[9 ] = yaxis[2];
	m[10] = zaxis[2];
	m[11] = 0;
	m[12] = -xVDotR(eye,xaxis);
	m[13] = -xVDotR(eye,yaxis);
	m[14] = -xVDotR(eye,zaxis);
	m[15] = 1.0f;
}

void nMatrixProjectionFovLH(f32* m, const f32 fov, const f32 aspect, const f32 tnearp, const f32 tfarp )
{
	f32 h,w,q;
	h = fov*0.5f;
	h = cosr(h) / sinr(h);
	w = h / aspect;
	q = tfarp / (tfarp-tnearp);
    // Setup ProjectionMatrix
	m[0 ] = w;
	m[1 ] = 0;
	m[2 ] = 0;
	m[3 ] = 0;
	m[4 ] = 0;
	m[5 ] = h;
	m[6 ] = 0;
	m[7 ] = 0;
	m[8 ] = 0;
	m[9 ] = 0;
	m[10] = q;
	m[11] = 1;
	m[12] = 0;
	m[13] = 0;
	m[14] = -q*tnearp;
	m[15] = 0;
}

//@ tested
void nQuatToR(f32* r,const f32 *q)
{
	f32 xx,xy,xz,xw,yy,yz,yw,zz,zw;

	xx      = q[QX] * q[QX];
    xy      = q[QX] * q[QY];
    xz      = q[QX] * q[QZ];
    xw      = q[QX] * q[QW];

    yy      = q[QY] * q[QY];
    yz      = q[QY] * q[QZ];
    yw      = q[QY] * q[QW];

    zz      = q[QZ] * q[QZ];
    zw      = q[QZ] * q[QW];

    r[0]  = 1 - 2 * ( yy + zz );
	r[1]  =     2 * ( xy + zw );
	r[2]  =     2 * ( xz - yw );

    r[4]  =     2 * ( xy - zw );
	r[5]  = 1 - 2 * ( xx + zz );
	r[6]  =     2 * ( yz + xw );

    r[8]  =     2 * ( xz + yw );   
    r[9]  =     2 * ( yz - xw );
    r[10] = 1 - 2 * ( xx + yy );

}

void nM12Inverse(f32* store,const f32* m)
{
	//SIMDIze
	f32 d;
	store[0] = (m[5]*m[10] - m[6]*m[9]);
	store[4] = (m[6]*m[8] - m[4]*m[10]);
	store[8] = (m[4]*m[9] - m[8]*m[5]);
	d = ( m[0] * store[0] + m[1] * store[1] + m[2] * store[2] );
	if(d > 0)
	{
		d = 1/d;
		store[0] *= d;
		store[1] = d*(m[2]*m[9]-m[1]*m[10]);
		store[2] = d*(m[1]*m[6]-m[2]*m[5]);
		store[4] *= d;
		store[5] = d*(m[10]*m[0] - m[2]*m[8]);
		store[6] = d*(m[2]*m[4] - m[6]*m[0]);
		store[8] *= d;
		store[9] = d*(m[8]*m[1] - m[0]*m[9]);
		store[10] = d*(m[0]*m[5] - m[4]*m[1]);
	}
}
/*
void cM12InverseDP(f32* store,const f32* m)
{
	//SIMDIze
	double d;
	double s0 = (m[5]*m[10] - m[9]*m[6]);
	double s1 = (m[9]*m[2] - m[1]*m[10]);
	double s2 = (m[1]*m[6] - m[5]*m[2]);
	d = ( m[0] * s0 + m[4] * s1 + m[8] * s2 );
	if(d > 0)
	{
		d = 1/d;
		store[0] = (f32)(s0*d);
		store[1] = (f32)(s1*d);
		store[2] = (f32)(s2*d);
		store[4] = (f32)(d*(m[6]*m[8] - m[10]*m[4]));
		store[5] = (f32)(d*(m[10]*m[0] - m[2]*m[8]));
		store[6] = (f32)(d*(m[2]*m[4] - m[6]*m[0]));
		store[8] = (f32)(d*(m[4]*m[9] - m[8]*m[5]));
		store[9] = (f32)(d*(m[8]*m[1] - m[0]*m[9]));
		store[10] = (f32)(d*(m[0]*m[5] - m[4]*m[1]));
	}
}
*/
void nOrthoVectors(f32* p,f32* q,const f32* n)
{
	// plane = {nx,ny,nz,d}
	f32 a,k;
	if (fabsr(n[2]) > N3DInvSqrt2) 
	{
		// choose p in y-z plane
		a = n[1]*n[1] + n[2]*n[2];
		k = rsqrtr(a);
		p[0] = 0;
		p[1] =-n[2]*k;
		p[2] = n[1]*k;
		// set q = n x p
		q[0] = a*k;
		q[1] =-n[0]*p[2];
		q[2] = n[0]*p[1];		
	}
	else 
	{
		// choose p in x-y plane
		a = n[0]*n[0] + n[1]*n[1];
		k = rsqrtr(a);
		p[0] =-n[1]*k;
		p[1] = n[0]*k;
		p[2] = 0;
		// set q = n x p
		q[0] =-n[2]*p[1];
		q[1] = n[2]*p[0];
		q[2] = a*k;
	}
}