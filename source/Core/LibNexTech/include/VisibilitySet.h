/*
 * VisibilitySet.h
 *
 *  Created on: Oct 5, 2011
 *      Author: obhi
 */

#ifndef VISIBILITYSET_H_
#define VISIBILITYSET_H_

#include "VisiblePrimitive.h"

namespace nextar {

	enum SortMask {
		MASK_LAYER =  		0xe0000000,
		MASK_TRANSLUCENCY =	0x18000000,
		MASK_SHADER_FIRST =	0x07ff0000,
		MASK_DEPTH_SECOND = 	0x0000ffff,
		MASK_SHADER_SECOND = 0x000007ff,
		MASK_DEPTH_FIRST = 	0x07fff800,
	};

	enum SortShift {
		SHIFT_LAYER = 29,
		SHIFT_TRANSLUCENCY = 27,
		SHIFT_SHADER_FIRST = 16,
		SHIFT_DEPTH_SECOND = 0,
		SHIFT_SHADER_SECOND = 0,
		SHIFT_DEPTH_FIRST = 11,
	};

	enum class RenderQueueFlags {
		TRANSLUCENCY = 1 << 0,
		DEFERRED = 1 << 1,
	};
	/**
	 * @remarks Sort key can be defined as or something similar:
	 *
	 * [3: layer | 2: translucency | 11: shader   | 16: depth  ]
	 * depth = front to back, so depth = distance
	 * [3: layer | 2: translucency | 16: depth    ] 11: shader ]
	 * * depth = back to front, so depth = MAX_DEPTH-distance
	 */
	/** Should do a radix sort */
	typedef std::pair<uint32, VisiblePrimitive*> KeyVisiblePrimitivePair;
	typedef vector<KeyVisiblePrimitivePair>::type VisibilityList;

	enum class RenderQueueName {
		Q_BACKGROUND,
		Q_OPAQUE,
		Q_TRANSLUCENT,
		Q_OVERLAY,
	};

	inline uint32 operator & (const KeyVisiblePrimitivePair& p, uint32 mask) {
		return p.first & mask;
	}

	inline bool operator < (const KeyVisiblePrimitivePair& first, const KeyVisiblePrimitivePair& second) {
		return first.first < second.first;
	}

	class SortKeyHelper {
	public:
		enum {
			SHADER_KEY_MASK = 0x7ff
		};

		static const uint16	MAX_DEPTH = 0xffff;

		static inline uint32 KeyTranslucent(uint8 layer, uint8 translucencyMask, uint16 depth, uint16 shader) {
			return (((uint32)layer) << SHIFT_LAYER) |
				( (((uint32)translucencyMask) << SHIFT_TRANSLUCENCY) & MASK_TRANSLUCENCY)|
				( (((uint32)depth) << SHIFT_DEPTH_FIRST) & MASK_DEPTH_FIRST) |
				( (((uint32)shader) << SHIFT_SHADER_SECOND) & MASK_SHADER_SECOND);
		}

		static inline uint32 KeyOpaque(uint8 layer, uint16 depth, uint16 shader) {
			return (((uint32)layer) << SHIFT_LAYER) |
				( (((uint32)0) << SHIFT_TRANSLUCENCY) & MASK_TRANSLUCENCY)|
				( (((uint32)shader) << SHIFT_SHADER_FIRST) & MASK_SHADER_FIRST )|
				( (((uint32)depth) << SHIFT_DEPTH_SECOND) & MASK_DEPTH_SECOND);
		}

		static inline uint32 Key(uint8 layer, uint8 translucencyMask, uint16 depth, uint16 shader) {
			return translucencyMask ? KeyTranslucent(layer, translucencyMask, depth, shader) :
					KeyOpaque(layer, depth, shader);
		}

		static inline uint32 KeyOpaque(uint32 key, uint16 newDepth) {
			/* extra masking at the end is redundant as its 16 bit already */
			return (key & (~MASK_DEPTH_SECOND))|
					( (((uint32)newDepth) << SHIFT_DEPTH_SECOND) /*& SortMask::MASK_SORT_INDEX*/);
		}

		static inline uint32 KeyTranslucent(uint32 key, uint16 newDepth) {
			/* extra masking at the end is redundant as its 16 bit already */
			return (key & (~MASK_DEPTH_FIRST))|
					( (((uint32)newDepth) << SHIFT_DEPTH_FIRST) /*& SortMask::MASK_SORT_INDEX*/);
		}

		static inline uint32 KeyOpaque(uint32 key, float normalizedDepth) {
			return KeyOpaque(key, static_cast<uint16>(normalizedDepth * MAX_DEPTH));
		}

		static inline uint32 KeyTranslucent(uint32 key, float normalizedDepth) {
			uint16 depth = MAX_DEPTH-static_cast<uint16>(normalizedDepth * MAX_DEPTH);
			return KeyTranslucent(key, depth);
		}

		static inline uint32 KeyUpdate(uint32 key, float normalizedDepth) {
			return key & MASK_TRANSLUCENCY ? KeyTranslucent(key, normalizedDepth) :
					KeyOpaque(key, normalizedDepth);
		}

		static inline uint32 KeyUpdate(uint32 key, uint16 depth) {
			return key & MASK_TRANSLUCENCY ? KeyTranslucent(key,
					static_cast<uint16>(MAX_DEPTH-depth)) :
					KeyOpaque(key, depth);
		}

		static inline uint16 Depth(float normalizedDepth) {
			return static_cast<uint16>(normalizedDepth * MAX_DEPTH);
		}

		friend inline bool operator < (const KeyVisiblePrimitivePair& kvp1, const KeyVisiblePrimitivePair& kvp2) {
			return kvp1.first < kvp2.first;
		}
	};

	class RenderQueue : public AllocScene {
	public:
		uint16 flags;
		uint16 priority;
		String name;
		VisibilityList visibles;

		friend inline bool operator < (const RenderQueue& rl1, const RenderQueue& rl2) {
			return rl1.priority < rl2.priority;
		}

		friend inline bool operator == (const RenderQueue& rl1, const RenderQueue& rl2) {
			return rl1.priority == rl2.priority;
		}

		inline VisibilityList& GetVisibilityList() {
			return visibles;
		}

		inline const VisibilityList& GetVisibilityList() const {
			return visibles;
		}

		inline void AddPrimitive(VisiblePrimitive* primitive, uint32 sortKey) {
			visibles.push_back(KeyVisiblePrimitivePair(sortKey, primitive));
		}

		void Sort();
	};

	typedef vector<RenderQueue>::type RenderQueueList;
	class VisibilitySet : public AllocScene {

	public:

		enum VisibilityMask {

			VM_BACKGROUND = 1 << 0,
			VM_OPAQUE = 1 << 1,
			VM_LIGHT = 1 << 2,
			VM_TRANSLUCENT = 1 << 3,
			VM_OVERLAY = 1 << 4,
			VM_SHADOW_CASTER = 1 << 5,
			VM_ALL = 0xffffffff
		};

		VisibilitySet();
		virtual ~VisibilitySet();

		void Prepare();
		inline void AddVisiblePrimitve(uint16 qu, VisiblePrimitive* primitive, uint32 sortKey) {
			queues[qu].AddPrimitive(primitive, sortKey);
		}
		void SortSet();

		RenderQueueList& GetRenderQueues() {
			return queues;
		}

		RenderQueue& GetQueue(uint32 i) {
			NEX_ASSERT(i < queues.size());
			return queues[i];
		}

	protected:

		uint32 visibleCount;
		RenderQueueList queues;
	};

} /* namespace nextar */
#endif /* VISIBILITYSET_H_ */
