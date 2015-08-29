
#ifndef NEXTAR_AXISALIGNEDBOX_H
#define NEXTAR_AXISALIGNEDBOX_H

#include <MathBase.h>
#include <MathTypes.h>


namespace nextar {

inline bool AABoxIsValid(AABoxF box);
inline Vector3A AABoxGetCenter(AABoxF box);
inline Vector3A AABoxGetSize(AABoxF box);
inline Vector3A AABoxGetPoint(AABoxF box, unsigned int i);
inline AxisAlignedBox AABoxUnion(AABoxF box, Vector3A point);

}

#endif //NEXTAR_AXISALIGNEDBOX_H