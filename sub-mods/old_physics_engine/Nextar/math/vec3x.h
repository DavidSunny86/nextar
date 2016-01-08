//@ Abhishek Dey
//@ Oct 21 2007

#ifndef XVECTOR3_H
#define XVECTOR3_H

#include "xmath_c.h"

namespace nextar
{

struct vec3x : public vec3_t
{
	// adds more functionality to the basic structure
	vec3x(float x,float y,float z) { this->x = x; this->y = y; this->z = z; }
};

}

#endif //XVECTOR3_H