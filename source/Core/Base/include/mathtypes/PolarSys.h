#ifndef NEXTAR_POLARSYS_H
#define NEXTAR_POLARSYS_H

//# generic polar sys corrdinate, this class is not aligned.

class _NexBaseAPI PolarSys : public nextar::AllocGeneral {
public:

	union {
		float r; // radius
		float length;// radius
	};

	union {
		float theta; // horizontal angle
		float horz;// horizontal angle
	};

	union {
		float phi; // vertical angle
		float vert;// vertical angle
	};
};

#endif //NEXTAR_POLARSYS_H
