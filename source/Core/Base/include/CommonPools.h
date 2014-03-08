
#ifndef MATRIXPOOL_H_
#define MATRIXPOOL_H_

#include "NexAlloc.h"
#include "MemPool.h"
#include "NexThread.h"

namespace nextar {

	template <typename T, const size_t NumPerBlock, enum MemoryCatagory Catagory>
	class PooledAllocator {

		typedef T ObjectType;
		typedef AllocatorBase<Catagory> BaseAllocator;
		typedef MemPool<NumPerBlock, mt::Mutex, Allocator> MPool;
		
		MPool pool;
	public:
		typedef PooledAllocator<T, NumPerBlock, Catagory> Type;

		PooledAllocator() : pool(sizeof(ObjectType)) {}
		~PooledAllocator() { }

		/* generic */
		inline static void* Alloc(size_t amount) {
			if (amount == Instance().pool.GetChunkSize())
				return AllocSingle(amount);
			return AllocMultiple(amount);
		}

		inline static void Free(void* ptr) {
			FreeSingle(ptr);
		}

		inline static void Free(void* ptr, size_t totalSize) {
			FreeMultiple(ptr);
		}

		/* optimized */
		inline static void* AllocSingle(size_t amount) {
			NEX_ASSERT(amount == Instance().pool.GetChunkSize());
			return Instance().pool.Alloc();
		}

		inline static void  FreeSingle(void* ptr) {
			return Instance().pool.Free(ptr);
		}

		/* use base allocator */
		inline static void* AllocMultiple(size_t amount) {
			return BaseAllocator::Alloc(amount);
		}

		inline static void  FreeMultiple(void* ptr) {
			BaseAllocator::Free(ptr);
		}

		inline static void FreePool() {
			Instance().pool.FreePool();
		}

	protected:

		static Type& Instance() {
			static Type _singletonPool;
			return _singletonPool;
		}
	};
}

#endif// MATRIXPOOL_H_
