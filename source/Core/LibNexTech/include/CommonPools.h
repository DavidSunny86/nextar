
#ifndef MATRIXPOOL_H_
#define MATRIXPOOL_H_

#include "NexMath.h"
#include "NexAlloc.h"
#include "MemPool.h"
#include "NexThread.h"
#include "TransformData.h"
#include "BoundingVolume.h"

namespace nextar {

	template <typename T, size_t NumPerBlock, typename Allocator>
	class _NexExport SingletonPool : public AllocGeneral {

		typedef MemPool<T, NumPerBlock, mt::Mutex, Allocator> MPool;
		typedef SingletonPool<T, NumPerBlock, Allocator> Type;
		MPool pool;

	public:

		inline SingletonPool() {}
				
		static inline T* Alloc() {
			return Instance()._Alloc();
		}

		static inline T* Alloc(size_t n) {
			return Instance()._Alloc(n);
		}

		static inline void Free(T* m) {
			Instance()._Free(m);
		}

		static inline void Free(T* m, size_t n) {
			Instance()._Free(m, n);
		}

	protected:

		friend class PoolManager;
		
		inline T* _Alloc() {
			return pool.Alloc();
		}

		inline T* _Alloc(size_t n) {
			return pool.Alloc(n);
		}

		inline void _Free(T* m) {
			pool.Free(m);
		}

		inline void _Free(T* m, size_t n) {
			pool.Free(m, n);
		}
		
		inline void _FreePool() {
			pool.FreePool();
		}

		static inline void FreePool() {
			Instance()._FreePool();
		}

		static inline void AllocPool() {
		}

		static Type& Instance() {
			static Type pool;
			return pool;
		}

	};

	typedef SingletonPool<Matrix4x4, NEX_MATRIX_POOL_NUM_PER_BLOCK, AllocatorMathCore> Matrix4x4Pool;
	//extern template class SingletonPool<Matrix4x4, NEX_MATRIX_POOL_NUM_PER_BLOCK, AllocatorMathCore>;
	typedef SingletonPool<TransformData, NEX_MATRIX_POOL_NUM_PER_BLOCK, AllocatorMathCore> TransformDataPool;
	//extern template class SingletonPool<TransformData, NEX_MATRIX_POOL_NUM_PER_BLOCK, AllocatorMathCore>;
	//extern template class SingletonPool<Matrix4x4, NEX_MATRIX_POOL_NUM_PER_BLOCK, AllocatorMathCore>;
	typedef SingletonPool<BoundingBox, NEX_MATRIX_POOL_NUM_PER_BLOCK, AllocatorMathCore> BoundingBoxPool;

	class PoolManager {
	public:
		static void CreateCommonPools();
		static void DestroyCommonPools();
	};
}

#endif MATRIXPOOL_H_