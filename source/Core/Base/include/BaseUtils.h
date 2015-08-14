
#ifndef NEXTAR_BASE_UTILS_H_
#define NEXTAR_BASE_UTILS_H_

#include <NexBase.h>

namespace nextar {

namespace Utils {

template <const uint32 N>
class first_set_bit_pos {

	template <const uint32 N, const uint32 level>
	class is_set {
	public:
		enum { value = (N & (1 << level)) };
	};

	template <const uint32 N, const bool isset, const uint32 level>
	class loop {
	public:
		enum { position = loop < N, (bool)is_set<N, level + 1>::value, level + 1>::position };
	};

	template <const uint32 N, const bool isset>
	class loop < N, isset, 32 > {
	public:
		enum { position = -1 };
	};

	template <const uint32 N, const uint32 level>
	class loop < N, true, level > {
	public:
		enum { position = level };
	};

public:
	enum { position = loop < N, (bool)is_set<N, 0>::value, 0 >::position };
};

}

}

#endif //NEXTAR_BASE_UTILS_H_
