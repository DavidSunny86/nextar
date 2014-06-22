/*
 * SortKeyHelper.h
 *
 *  Created on: 23-Mar-2014
 *      Author: obhi
 */

#ifndef SORTKEYHELPER_H_
#define SORTKEYHELPER_H_

#include <NexBase.h>

namespace nextar {

enum SortMask {
	MASK_LAYER = 0xe0000000,
	MASK_TRANSLUCENCY = 0x18000000,
	MASK_SHADER_FIRST = 0x07ff0000,
	MASK_DEPTH_SECOND = 0x0000ffff,
	MASK_SHADER_SECOND = 0x000007ff,
	MASK_DEPTH_FIRST = 0x07fff800,
};

enum SortShift {
	SHIFT_LAYER = 29,
	SHIFT_TRANSLUCENCY = 27,
	SHIFT_SHADER_FIRST = 16,
	SHIFT_DEPTH_SECOND = 0,
	SHIFT_SHADER_SECOND = 0,
	SHIFT_DEPTH_FIRST = 11,
};

class SortKeyHelper {
public:
	enum {
		SHADER_KEY_MASK = 0x7ff
	};

	static const uint16 MAX_DEPTH = 0xffff;

	static inline uint32 KeyTranslucent(uint8 layer, uint8 translucencyMask,
			uint16 depth, uint16 shader) {
		return (((uint32) layer) << SHIFT_LAYER)
				| ((((uint32) translucencyMask) << SHIFT_TRANSLUCENCY)
						& MASK_TRANSLUCENCY)
				| ((((uint32) depth) << SHIFT_DEPTH_FIRST) & MASK_DEPTH_FIRST)
				| ((((uint32) shader) << SHIFT_SHADER_SECOND)
						& MASK_SHADER_SECOND);
	}

	static inline uint32 KeyOpaque(uint8 layer, uint16 depth, uint16 shader) {
		return (((uint32) layer) << SHIFT_LAYER)
				| ((((uint32) 0) << SHIFT_TRANSLUCENCY) & MASK_TRANSLUCENCY)
				| ((((uint32) shader) << SHIFT_SHADER_FIRST) & MASK_SHADER_FIRST)
				| ((((uint32) depth) << SHIFT_DEPTH_SECOND) & MASK_DEPTH_SECOND);
	}

	static inline uint32 Key(uint8 layer, uint8 translucencyMask, uint16 depth,
			uint16 shader) {
		return translucencyMask ?
				KeyTranslucent(layer, translucencyMask, depth, shader) :
				KeyOpaque(layer, depth, shader);
	}

	static inline uint32 KeyOpaque(uint32 key, uint16 newDepth) {
		/* extra masking at the end is redundant as its 16 bit already */
		return (key & (~MASK_DEPTH_SECOND))
				| ((((uint32) newDepth) << SHIFT_DEPTH_SECOND) /*& SortMask::MASK_SORT_INDEX*/);
	}

	static inline uint32 KeyTranslucent(uint32 key, uint16 newDepth) {
		/* extra masking at the end is redundant as its 16 bit already */
		return (key & (~MASK_DEPTH_FIRST))
				| ((((uint32) newDepth) << SHIFT_DEPTH_FIRST) /*& SortMask::MASK_SORT_INDEX*/);
	}

	static inline uint32 KeyOpaque(uint32 key, float normalizedDepth) {
		return KeyOpaque(key, static_cast<uint16>(normalizedDepth * MAX_DEPTH));
	}

	static inline uint32 KeyTranslucent(uint32 key, float normalizedDepth) {
		uint16 depth = MAX_DEPTH
				- static_cast<uint16>(normalizedDepth * MAX_DEPTH);
		return KeyTranslucent(key, depth);
	}

	static inline uint32 KeyUpdate(uint32 key, float normalizedDepth) {
		return key & MASK_TRANSLUCENCY ?
				KeyTranslucent(key, normalizedDepth) :
				KeyOpaque(key, normalizedDepth);
	}

	static inline uint32 KeyUpdate(uint32 key, uint16 depth) {
		return key & MASK_TRANSLUCENCY ?
				KeyTranslucent(key, static_cast<uint16>(MAX_DEPTH - depth)) :
				KeyOpaque(key, depth);
	}

	static inline uint16 Depth(float normalizedDepth) {
		return static_cast<uint16>(normalizedDepth * MAX_DEPTH);
	}
};
}

#endif /* SORTKEYHELPER_H_ */
