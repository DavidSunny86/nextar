//@ Abhishek Dey
//@ September 01 2007
//@ September 30 2007

#ifndef XBASIC_TYPES
#define XBASIC_TYPES
#include <xtypes_c.h>
// this is fully made for c - type lib usage
#ifdef __cplusplus
extern "C" {
#endif
typedef s32	i2d;
// as it is the basic types are not part of the nextar namespace
typedef struct _coord2  { i2d x, y; }coord2;
typedef struct _rect2 { i2d left,top,right,bottom; }rect2;
typedef struct _size2 { i2d dx,dy; }size2;
typedef struct vec2_t{ f32 x,y; }vec2_t;
typedef struct vec3_t{ f32 x,y,z; }vec3_t;
enum AXISSPECIFICATION {
	VX = 0,
	VY = 1,
	VZ = 2,
	VH = 3,
	PNA = 0,
	PNX = PNA,
	PNB = 1,
	PNY = PNB,
	PNC = 2,
	PNZ = PNC,
	PND = 3,
	QW = 0,
	QA = 1,
	QX = QA,
	QB = 2,
	QY = QB,
	QC = 3,
	QZ = QC
};
X_EFFICIENT_TYPEDEF(f32 quad_t[4]);
X_EFFICIENT_TYPEDEF(f32 vec4_t[4]);
X_EFFICIENT_TYPEDEF(f32 plane_t[4]);
X_EFFICIENT_TYPEDEF(f32 mat44_t[16]);
X_EFFICIENT_TYPEDEF(f32 mat34_t[12]);
X_EFFICIENT_TYPEDEF(f32 mat43_t[12]);
X_EFFICIENT_TYPEDEF(f32 quat_t[4]);
struct transform_tag {
	X_EFFICIENT_ALIGN( f32	rot[12]);
	X_EFFICIENT_ALIGN( f32	pos[4]);
}XALIGN_STRUCT;
X_EFFICIENT_TYPEDEF(struct transform_tag transform);
typedef struct aabb_tag  { 
	union {
		struct {
			vec3_t min,max; 
		};
		vec3_t	mpoints[2];
	};
}aabb_t;
typedef struct polar_tag { f32 r,theta,phi; }polar_t;
typedef struct polar polar;
// row major matrix, each col can be regarded as axis
#ifdef __cplusplus
}
#endif

#endif //