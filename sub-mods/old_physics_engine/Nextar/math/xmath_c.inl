
//@ Abhishek Dey
//@ June 27,2006
//@ September 04 2007
//@ October 24 2007
//===========================================================================
// This file contains C specific defn, an alternative
// to the SSE library, in case it is not supported
// All functions in this file are tested

#ifdef M34
#undef M34
#endif

#define M34(m,i,j)	m[i*4+j]
#define M44(m,i,j)	m[i*4+j]
// Add
_n_inline void xQ4Add(f32* store,const f32* v1,const f32* v2)
{
	store[0] = v1[0]+v2[0];
	store[1] = v1[1]+v2[1];
	store[2] = v1[2]+v2[2];
}
// Sub
_n_inline void xQ4Sub(f32* store,const f32* v1,const f32* v2)
{
	store[0] = v1[0]-v2[0];
	store[1] = v1[1]-v2[1];
	store[2] = v1[2]-v2[2];
}

// Mul
_n_inline void xQ4Mul(f32* store,const f32* v1,const f32* v2)
{
	store[0] = v1[0]*v2[0];
	store[1] = v1[1]*v2[1];
	store[2] = v1[2]*v2[2];
}

// Div
_n_inline void xQ4Div(f32* store,const f32* v1,const f32* v2)
{
	store[0] = v1[0]/v2[0];
	store[1] = v1[1]/v2[1];
	store[2] = v1[2]/v2[2];
}

// scale
_n_inline void xQ4ScaleS(f32* store,const f32* v,f32 val)
{
	store[0] = v[0]*val;
	store[1] = v[1]*val;
	store[2] = v[2]*val;
	store[3] = v[3]*val;
}
// scale
_n_inline void xQ3ScaleS(f32* store,const f32* v,f32 val)
{
	store[0] = v[0]*val;
	store[1] = v[1]*val;
	store[2] = v[2]*val;
}
// scale
_n_inline void xQ4Scale(f32* v,f32 val)
{
	v[0] = v[0]*val;
	v[1] = v[1]*val;
	v[2] = v[2]*val;
	v[3] = v[3]*val;
}
// scale
_n_inline void xQ3Scale(f32* v,f32 val)
{
	v[0] = v[0]*val;
	v[1] = v[1]*val;
	v[2] = v[2]*val;
}

// i.e store += scale*vec v3,v4 ans:v3
_n_inline void xQ3ScaleNAdd(f32* store,const f32* q,f32 val)
{
	store[0] += q[0]*val;
	store[1] += q[1]*val;
	store[2] += q[2]*val;
}
// i.e store += scale*vec v3,v4 ans:v3
_n_inline void xQ4ScaleNAdd(f32* store,const f32* q,f32 val)
{
	store[0] += q[0]*val;
	store[1] += q[1]*val;
	store[2] += q[2]*val;
	store[3] += q[3]*val;
}
// i.e store -= scale*vec v3,v4 ans:v3
_n_inline void xQ3ScaleNSub(f32* store,const f32* q,f32 val)
{
	store[0] -= q[0]*val;
	store[1] -= q[1]*val;
	store[2] -= q[2]*val;
}
// i.e store -= scale*vec v3,v4 ans:v3
_n_inline void xQ4ScaleNSub(f32* store,const f32* q,f32 val)
{
	store[0] -= q[0]*val;
	store[1] -= q[1]*val;
	store[2] -= q[2]*val;
	store[3] -= q[3]*val;
}
_n_inline void xQ3ScaleNMul(f32* store,const f32* q,f32 val)
{
	store[0] *= q[0]*val;
	store[1] *= q[1]*val;
	store[2] *= q[2]*val;
}
_n_inline void xQ4ScaleNMul(f32* store,const f32* q,f32 val)
{
	store[0] *= q[0]*val;
	store[1] *= q[1]*val;
	store[2] *= q[2]*val;
	store[3] *= q[3]*val;
}
_n_inline void xQ3ScaleNDiv(f32* store,const f32* q,f32 val)
{
	store[0] /= q[0]*val;
	store[1] /= q[1]*val;
	store[2] /= q[2]*val;
}
_n_inline void xQ4ScaleNDiv(f32* store,const f32* q,f32 val)
{
	store[0] /= q[0]*val;
	store[1] /= q[1]*val;
	store[2] /= q[2]*val;
	store[3] /= q[3]*val;
}
// TESTED
// Mul1_344
_n_inline void xMul1_344(f32* store,const f32* a,const f32* r)
{
	store[0] = a[0]*r[0] + a[1]*r[4] + a[2]*r[8];
	store[1] = a[0]*r[1] + a[1]*r[5] + a[2]*r[9];
	store[2] = a[0]*r[2] + a[1]*r[6] + a[2]*r[10];
}

// TESTED
// Mul0_344
_n_inline void xMul0_344(f32* store,const f32* a,const f32* r)
{
	store[0] = a[0]*r[0] + a[1]*r[1] + a[2]*r[2];
	store[1] = a[0]*r[4] + a[1]*r[5] + a[2]*r[6];
	store[2] = a[0]*r[8] + a[1]*r[9] + a[2]*r[10];
}

// TESTED
_n_inline void xMul2_34(f32* store,const f32* a,const f32* b)
{
	for(s32 i=0;i<3;i++) {
		for(s32 j=0;j<3;j++) {
			M34(store,i,j) = M34(a,0,i)*M34(b,0,j) 
			+ M34(a,1,i)*M34(b,1,j) + M34(a,2,i)*M34(b,2,j);
		}
	}
}

// multiply each row of r with a, add each COLOUMN of r and store in store
_n_inline void xMul2_344(f32* store,const f32* a,const f32* r)
{
	store[0] = a[0]*(r[0]+r[4]+r[8]);
	store[1] = a[1]*(r[1]+r[5]+r[9]);
	store[2] = a[2]*(r[2]+r[6]+r[10]);
}

_n_inline void xMul1_34(f32* store,const f32* a,const f32* b)
{
	store[0+0] = a[0+0]*b[0+0] + a[0+1]*b[0+4] + a[0+2]*b[0+8];
	store[0+1] = a[0+0]*b[1+0] + a[0+1]*b[1+4] + a[0+2]*b[1+8];
	store[0+2] = a[0+0]*b[2+0] + a[0+1]*b[2+4] + a[0+2]*b[2+8];
	store[4+0] = a[4+0]*b[0+0] + a[4+1]*b[0+4] + a[4+2]*b[0+8];
	store[4+1] = a[4+0]*b[1+0] + a[4+1]*b[1+4] + a[4+2]*b[1+8];
	store[4+2] = a[4+0]*b[2+0] + a[4+1]*b[2+4] + a[4+2]*b[2+8];
	store[8+0] = a[8+0]*b[0+0] + a[8+1]*b[0+4] + a[8+2]*b[0+8];
	store[8+1] = a[8+0]*b[1+0] + a[8+1]*b[1+4] + a[8+2]*b[1+8];
	store[8+2] = a[8+0]*b[2+0] + a[8+1]*b[2+4] + a[8+2]*b[2+8];
}

_n_inline void xTMulEfficient(transform* store,const transform* offset,const transform* parent)
{
	store->pos[0] = offset->pos[0]*parent->rot[0] + 
					offset->pos[1]*parent->rot[4] + 
					offset->pos[2]*parent->rot[8] + parent->pos[0];

	store->pos[1] = offset->pos[0]*parent->rot[1] + 
					offset->pos[1]*parent->rot[5] + 
					offset->pos[2]*parent->rot[9] + parent->pos[1];

	store->pos[2] = offset->pos[0]*parent->rot[2] + 
					offset->pos[1]*parent->rot[6] + 
					offset->pos[2]*parent->rot[10] + parent->pos[2];

	store->rot[0+0] = offset->rot[0+0]*parent->rot[0+0] + offset->rot[0+1]*parent->rot[0+4] + offset->rot[0+2]*parent->rot[0+8];
	store->rot[0+1] = offset->rot[0+0]*parent->rot[1+0] + offset->rot[0+1]*parent->rot[1+4] + offset->rot[0+2]*parent->rot[1+8];
	store->rot[0+2] = offset->rot[0+0]*parent->rot[2+0] + offset->rot[0+1]*parent->rot[2+4] + offset->rot[0+2]*parent->rot[2+8];
	store->rot[4+0] = offset->rot[4+0]*parent->rot[0+0] + offset->rot[4+1]*parent->rot[0+4] + offset->rot[4+2]*parent->rot[0+8];
	store->rot[4+1] = offset->rot[4+0]*parent->rot[1+0] + offset->rot[4+1]*parent->rot[1+4] + offset->rot[4+2]*parent->rot[1+8];
	store->rot[4+2] = offset->rot[4+0]*parent->rot[2+0] + offset->rot[4+1]*parent->rot[2+4] + offset->rot[4+2]*parent->rot[2+8];
	store->rot[8+0] = offset->rot[8+0]*parent->rot[0+0] + offset->rot[8+1]*parent->rot[0+4] + offset->rot[8+2]*parent->rot[0+8];
	store->rot[8+1] = offset->rot[8+0]*parent->rot[1+0] + offset->rot[8+1]*parent->rot[1+4] + offset->rot[8+2]*parent->rot[1+8];
	store->rot[8+2] = offset->rot[8+0]*parent->rot[2+0] + offset->rot[8+1]*parent->rot[2+4] + offset->rot[8+2]*parent->rot[2+8];

}

// TESTED 
_n_inline f32 xPCalculatePointR(const f32* pl,const f32* point)
{
	return pl[0]*point[0]+pl[1]*point[1]+pl[2]*point[2]+pl[3];
}

// TESTED 
_n_inline void xPCalculatePoint(f32* store_ptr,const f32* pl,const f32* point)
{
	*store_ptr = pl[0]*point[0]+pl[1]*point[1]+pl[2]*point[2]+pl[3];
}

//TESTED
_n_inline void xPNormalize(f32* q)
{
	f32 v = sqrtr(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]);
	q[0] = q[0]/v;
	q[1] = q[1]/v;
	q[2] = q[2]/v;
	q[3] = q[3]/v;
}

//TESTED
_n_inline void xPNormalizeS(f32* q1,const f32* q)
{
	f32 v = sqrtr(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]);
	q1[0] = q[0]/v;
	q1[1] = q[1]/v;
	q1[2] = q[2]/v;
	q1[3] = q[3]/v;
}

_inline void xMul1_44(f32* store, const f32* a,const f32* b)
{
	for(s32 i=0;i<4;i++) {
		for(s32 j=0;j<4;j++) {
			M44(store,i,j) = M44(a,i,0)*M44(b,0,j) 
			+ M44(a,i,1)*M44(b,1,j) + M44(a,i,2)*M34(b,2,j) + M44(a,i,3)*M34(b,3,j);
		}
	}
}