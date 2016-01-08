//@ Abhishek Dey
//@ Oct 24 2007
//@ Oct 25 2007

#ifndef X_COLLIDER_UTILITY_X_NEXTAR
#define X_COLLIDER_UTILITY_X_NEXTAR

#include <xmath_c.h>
#include <xcontact.h>

#ifdef __cplusplus
extern "C" 
{
#endif

s32 cBoxCollide( const f32* p1,const f32* r1,const f32* sz1, 
				 const f32* p2,const f32* r2,const f32* sz2,
				 cContact* c,f32* normal);


#ifdef __cplusplus
}
#endif

#endif //X_COLLIDER_UTILITY_X_NEXTAR