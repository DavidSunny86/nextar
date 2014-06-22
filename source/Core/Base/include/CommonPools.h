#ifndef MATRIXPOOL_H_
#define MATRIXPOOL_H_

#include <NexAlloc.h>
#include <MemPool.h>
#include <NexThread.h>

namespace nextar {

template<typename T, const size_t NumPerBlock, enum MemoryCatagory Catagory>
class PooledAllocator {
	// todo write an usage reporting function that will automatically be invoked at the end and report
	// the total allocations, alive pointer avg, total blocks allocated, etc to get an idea of
	// item per block count is correct or not.
	typedef T ObjectType;
	typedef AllocatorBase<Catagory> BaseAllocator;
	typedef MemPool<NumPerBlock, mt::Mutex, Allocator> MPool;

	MPool pool;
public:
	typedef PooledAllocator<T, NumPerBlock, Catagory> Type;

	PooledAllocator() :
			pool(sizeof(ObjectType)) {
	}
	~PooledAllocator() {
	}

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

	inline static void FreeSingle(void* ptr) {
		Instance().pool.Free(ptr);
	}

	/* use base allocator */
	inline static void* AllocMultiple(size_t amount) {
		return BaseAllocator::Alloc(amount);
	}

	inline static void FreeMultiple(void* ptr) {
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

template<typename T, const size_t NumPerBlock, enum MemoryCatagory Catagory>
class STDPoolAllocator: public std::allocator<T> {
public:
	//    typedefs

	typedef typename std::allocator<T>::pointer pointer;
	typedef typename std::allocator<T>::size_type size_type;
	typedef PooledAllocator<T, NumPerBlock, Catagory> BaseAllocator;

public:
	//    convert an allocator<T> to allocator<U>

	template<typename U>
	struct rebind {
		typedef STDPoolAllocator<U, NumPerBlock, Catagory> other;
	};

public:

	inline explicit STDPoolAllocator() {
	}

	inline ~STDPoolAllocator() {
	}

	/*	inline explicit STLAllocator(STLAllocator const&) {}*/
	template<typename U>
	inline STDPoolAllocator(STDPoolAllocator<U, NumPerBlock, Catagory> const&) {
	}

	//    memory allocation
	inline pointer allocate(size_type cnt,
			typename std::allocator<void>::const_pointer = 0) const {
		return cnt == 1 ?
				reinterpret_cast<pointer>(BaseAllocator::AllocSingle(sizeof(T))) :
				reinterpret_cast<pointer>(BaseAllocator::AllocMultiple(
						sizeof(T) * cnt));
	}

	inline void deallocate(pointer p, size_type cnt) const {
		if (cnt == 1)
			BaseAllocator::FreeSingle(p);
		else
			BaseAllocator::FreeMultiple(p);
	}
	//    construction/destruction

	inline void construct(pointer p, const T& t) const {
		nextar::ConstructObject(p, t);
	}

	inline void destroy(pointer p) const {
		nextar::DestroyObject(p);
	}
};
}

#endif// MATRIXPOOL_H_
