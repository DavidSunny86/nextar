//@ Abhishek Dey
//@ Oct 25 2007

#ifndef NEXTAR_XALLOCCACHE_H
#define NEXTAR_XALLOCCACHE_H

#ifdef __cplusplus
extern "C"
{
#endif

transform*	allocTransform();
void		freeTransform(transform* p);

#ifdef __cplusplus
}
#endif

#endif //NEXTAR_XALLOCCACHE_H