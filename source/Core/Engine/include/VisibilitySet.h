/*
 * VisibilitySet.h
 *
 *  Created on: Oct 5, 2011
 *      Author: obhi
 */

#ifndef VISIBILITYSET_H_
#define VISIBILITYSET_H_

#include <SortKeyHelper.h>
#include <VisiblePrimitive.h>
#include <RadixSort.h>

namespace nextar {

enum class RenderQueueFlags
	: uint16 {
		TRANSLUCENCY = 1 << 0,
	DEFERRED = 1 << 1,
	SORT_ENABLED = 1 << 2,
	DEFAULT_FLAGS = SORT_ENABLED | DEFERRED,
};

NEX_ENUM_FLAGS(RenderQueueFlags, uint16);
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

inline uint32 operator &(const KeyVisiblePrimitivePair& p, uint32 mask) {
	return p.first & mask;
}

inline bool operator <(const KeyVisiblePrimitivePair& first,
		const KeyVisiblePrimitivePair& second) {
	return first.first < second.first;
}

enum class RenderQueueName {
	Q_BACKGROUND, Q_OPAQUE, Q_TRANSLUCENT, Q_OVERLAY,
};

struct RenderQueueDesc {
	RenderQueueFlags flags;
	uint16 priority;
	String name;

	friend inline bool operator <(const RenderQueueDesc& rl1,
			const RenderQueueDesc& rl2) {
		return rl1.priority < rl2.priority;
	}

	friend inline bool operator ==(const RenderQueueDesc& rl1,
			const RenderQueueDesc& rl2) {
		return rl1.name == rl2.name;
	}
};

typedef vector<RenderQueueDesc>::type RenderQueueDescList;
class RenderQueue: public AllocScene {
public:

	RenderQueueFlags flags;
	VisibilityList visibles;
	const RenderQueueDesc* descriptor;

	RenderQueue();

	inline void SetDesc(const RenderQueueDesc& desc) {
		flags = desc.flags;
		descriptor = &desc;
	}

	inline VisibilityList& GetVisibilityList() {
		return visibles;
	}

	inline const VisibilityList& GetVisibilityList() const {
		return visibles;
	}

	inline bool IsSorted() const {
		return Test(flags & RenderQueueFlags::SORT_ENABLED);
	}

	inline void AddPrimitive(VisiblePrimitive* primitive, uint32 sortKey) {
		visibles.push_back(KeyVisiblePrimitivePair(sortKey, primitive));
	}

	inline void Sort() {
		if (visibles.size() > 32)
			RadixSortInt32(visibles.data(), (uint32) visibles.size());
		else
			std::sort(visibles.begin(), visibles.end());
	}

	inline void Prepare() {
		visibles.clear();
	}
};

typedef vector<RenderQueue>::type RenderQueueList;
class VisibilitySet: public AllocScene {

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

	inline void Prepare() {
		for (auto &q : queues) {
			if (q.IsSorted())
				q.Prepare();
		}
	}

	inline void AddVisiblePrimitve(uint16 qu, VisiblePrimitive* primitive,
			uint32 sortKey) {
		queues[qu].AddPrimitive(primitive, sortKey);
	}

	inline void SortSet() {
		for (auto &q : queues) {
			if (q.IsSorted())
				q.Sort();
		}
	}

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
