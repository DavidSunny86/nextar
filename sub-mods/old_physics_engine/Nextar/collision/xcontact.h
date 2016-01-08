//@ Abhishek Dey
//@ Oct 24 2007
//@ Oct 25 2007

#ifndef XCONTACT_H
#define XCONTACT_H

#include <xmath_c.h>
// these are c like structures
#ifdef __cplusplus
extern "C" {
#endif

typedef struct tag_Feature {
	union {
		struct Edges {
			char inEdge1;
			char outEdge1;
			char inEdge2;
			char outEdge2;
		} e;
		s32 key;
	};
}Feature;

struct tag_cContact {
	vec4_t	pos;
	vec4_t	norm;	// from body1 to body2
	f32		depth;
//	Feature pair;
}XALIGN_STRUCT;
X_EFFICIENT_TYPEDEF(struct tag_cContact cContact);

#define N3DMaxContact 4

#ifdef __cplusplus
}
#endif

#endif //XCONTACT_H