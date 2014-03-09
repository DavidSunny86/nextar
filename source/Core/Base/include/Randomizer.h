#ifndef NEXTAR_RANDOMIZER_H
#define NEXTAR_RANDOMIZER_H

#include <NexBase.h>

namespace nextar {

	enum RandClass {
		RANDCLASS_C, RANDCLASS_FAST, RANDCLASS_CORE,
	};

	class RandomizerSeed {
	protected:
		_NexBaseExport
		static uint32 seed;
	};

	template<const RandClass randClass = RANDCLASS_FAST>
	class RandomizerBase: public RandomizerSeed {

	public:
		enum {
			MAX_VAL = 0x7fff
		};

		static _NexInline void Seed(uint32 s) {
			seed = s;
		}

		static _NexInline uint32 Rand() {
			seed = (214013 * seed + 2531011);
			return (seed >> 16) & 0x7FFF;
		}
		;
	};

	template<const RandClass randClass = RANDCLASS_FAST>
	class Random {
		typedef RandomizerBase<randClass> RandBase;
	public:

		static _NexInline void Seed(uint32 val) {
			RandBase::Seed(val);
		}

		static _NexInline uint32 ULong() {
			return RandBase::Rand();
		}

		static _NexInline int32 Long() {
			return (int32) RandBase::Rand();
		}

		static _NexInline float Normalized() {
			return (float) RandBase::Rand() / (float) RandBase::MAX_VAL;
		}

		/** @param length Should be equal to end-start **/
		static _NexInline int32 RangeLong(int32 start, uint32 length) {
			NEX_ASSERT(length);
			return start + (int32) (Normalized() * length);
		}

		/** @param length Should be equal to end-start **/
		static _NexInline float RangeFloat(float start, float length) {
			return start + (float) (Normalized() * length);
		}

	};

	typedef Random<RANDCLASS_C> RandomGen; // rand from C
	typedef Random<RANDCLASS_FAST> RandomFast; // randfast
	typedef Random<RANDCLASS_CORE> RandomCore; // from nextar

	/** Specialization for randomizers **/
	template<> class RandomizerBase<RANDCLASS_C> {
	public:

		enum {
			MAX_VAL = RAND_MAX
		};

		static _NexInline void Seed(uint32 s) {
			std::srand(s);
		}

		static _NexInline uint32 Rand() {
			return std::rand();
		}
		;
	};
}

#endif //NEXTAR_RANDOMIZER_H
