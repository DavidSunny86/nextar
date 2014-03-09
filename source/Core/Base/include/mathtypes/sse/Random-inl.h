
/**
 * @remarks SSE Randomizer
 *	    Code from intel website: Fast random number generator
 *	    http://software.intel.com/en-us/articles/fast-random-number-generator-on-the-intel-pentiumr-4-processor/
 **/
#define NEX_MATH_RANDOMIZER_DEFINED
#define NEX_MATH_RANDOM_SEED _NexBaseExport NEX_EFFICIENT_ALIGN(__m128i nextar::MathRandomizer::curSeed);

namespace nextar {

	class MathRandomizer {
		_NexBaseExport static __m128i curSeed;
	public:

		static inline void Seed(unsigned int seed) {
			curSeed = _mm_set_epi32(seed, seed + 1, seed, seed + 1);
		}

		static inline void RandSSE(unsigned int* result) {
			NEX_EFFICIENT_ALIGN(__m128i curSeedSplit);
			NEX_EFFICIENT_ALIGN(__m128i multiplier);
			NEX_EFFICIENT_ALIGN(__m128i adder);
			NEX_EFFICIENT_ALIGN(__m128i modMask);
			NEX_EFFICIENT_ALIGN(__m128i sraMask);

			NEX_EFFICIENT_ALIGN(static const unsigned int mult[4]) = {214013, 17405, 214013, 69069};
			NEX_EFFICIENT_ALIGN(static const unsigned int gadd[4]) = {2531011, 10395331, 13737667, 1};
			NEX_EFFICIENT_ALIGN(static const unsigned int mask[4]) = {0xFFFFFFFF, 0, 0xFFFFFFFF, 0};
			NEX_EFFICIENT_ALIGN(static const unsigned int masklo[4]) = {0x00007FFF, 0x00007FFF, 0x00007FFF, 0x00007FFF};
			adder = _mm_load_si128((__m128i*) gadd);
			multiplier = _mm_load_si128((__m128i*) mult);
			modMask = _mm_load_si128((__m128i*) mask);
			sraMask = _mm_load_si128((__m128i*) masklo);
			curSeedSplit = _mm_shuffle_epi32(curSeed, _MM_SHUFFLE(2, 3, 0, 1));
			curSeed = _mm_mul_epu32(curSeed, multiplier);
			multiplier = _mm_shuffle_epi32(multiplier, _MM_SHUFFLE(2, 3, 0, 1));
			curSeedSplit = _mm_mul_epu32(curSeedSplit, multiplier);
			curSeed = _mm_and_si128(curSeed, modMask);
			curSeedSplit = _mm_and_si128(curSeedSplit, modMask);
			curSeedSplit = _mm_shuffle_epi32(curSeedSplit, _MM_SHUFFLE(2, 3, 0, 1));
			curSeed = _mm_or_si128(curSeed, curSeedSplit);
			curSeed = _mm_add_epi32(curSeed, adder);

	#ifdef NEX_C_RAND_COMPATABILITY// Add the lines below if you wish to reduce your results to 16-bit vals...
			NEX_EFFICIENT_ALIGN(__m128i sseresult);
			sseresult = _mm_srai_epi32(curSeed, 16);
			sseresult = _mm_and_si128(sseresult, sraMask);
			_mm_storeu_si128((__m128i*) result, sseresult);
			return;
	#endif
			_mm_storeu_si128((__m128i*) result, curSeed);
			return;
		}
	};

}

