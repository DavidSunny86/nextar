//@ Abhishek Dey
//@ Oct 21 2007

#ifndef XVECTOR2_H
#define XVECTOR2_H

#include "xmath_c.h"

namespace nextar
{

struct vec2x : public vec2_t
{
	// adds more functionality to the basic structure
	vec2x(float x,float y) { this->x = x; this->y = y; }
};

}

#endif //XVECTOR3_H