#ifndef MATRIXPOOL_H_
#define MATRIXPOOL_H_

#include <NexAlloc.h>
#include <MemoryPool.h>
#include <NexThread.h>

namespace nextar {

// Typically we want a single uniform pool for same sized objects.
// Current Implementation however uses multiple pools per object type,
// So we move the implementation of PooledAllocator to PoolProvider
template<const size_t ObjectSize, const size_t NumPerBlock, enum MemoryCategory Catagory>
class PoolProvider {
	typedef AllocatorBase<Catagory> BaseAllocator;
	typedef MemoryPool<NumPerBlock, Mutex, BaseAllocator> MPool;
	MPool pool;
public:
	typedef PoolProvider<ObjectSize, NumPerBlock, Catagory> Type;

	PoolProvider() :
		pool(ObjectSize) {
	}
	~PoolProvider() {
#ifdef NEX_DEBUG
		std::ostringstream info;
		info << "\n*********************************************"
			<< "\n  Pool Allocator: [" << ObjectSize << ", " << NumPerBlock << ", " << Catagory << "]"
			<< "\n*********************************************";
		Platform::OutputDebug(info.str().c_str());
#endif
	}

	/* optimized */
	inline static void* AllocSingle(size_t amount) {
		NEX_ASSERT(amount == ObjectSize);
		return Instance().pool.Alloc();
	}

	inline static void FreeSingle(void* ptr) {
		Instance().pool.Free(ptr);
	}

	/* use base allocator */
	inline static void* AllocMultiple(size_t amount) {
		return NEX_ALLOCATOR_ALLOC(amount, BaseAllocator);
	}

	inline static void FreeMultiple(void* ptr) {
		NEX_ALLOCATOR_FREE(ptr, BaseAllocator);
	}

	inline static void FreePool() {
		Instance().pool.FreePool();
	}

protected:

	static Type& Instance() {
		// @urgent The static initialization is potentially dangerous and unsafe.
		// May be we should use a registry to avoid instantiating large number of
		// memory pools.
		static Type _singletonPool;
		return _singletonPool;
	}
};

template<typename T, const size_t NumPerBlock, enum MemoryCategory Catagory>
class PooledAllocator {
	typedef T ObjectType;
	typedef PoolProvider<sizeof(T), NumPerBlock, Catagory> ProviderSpecialization;
public:
	typedef ProviderSpecialization Provider;
	typedef PooledAllocator<T, NumPerBlock, Catagory> Type;

	PooledAllocator() {
	}
	~PooledAllocator() {
	}
#ifdef NEX_EXTENSIVE_MEM_TRACKER
	static inline void* Alloc(size_t size,
			const char* func,
			const char* file,
			long line) {
		return Alloc(size);
	}
#endif
	/* generic */
	inline static void* Alloc(size_t amount) {
		if (amount == sizeof(T))
			return ProviderSpecialization::AllocSingle(amount);
		return ProviderSpecialization::AllocMultiple(amount);
	}

	inline static void Free(void* ptr) {
		ProviderSpecialization::FreeSingle(ptr);
	}

	inline static void Free(void* ptr, size_t totalSize) {
		ProviderSpecialization::FreeMultiple(ptr);
	}

	inline static void* AllocSingle(size_t amount) {
		return ProviderSpecialization::AllocSingle(amount);
	}

	inline static void FreeSingle(void* ptr) {
		ProviderSpecialization::FreeSingle(ptr);
	}

	/* use base allocator */
	inline static void* AllocMultiple(size_t amount) {
		return ProviderSpecialization::AllocMultiple(amount);
	}

	inline static void FreeMultiple(void* ptr) {
		ProviderSpecialization::FreeMultiple(ptr);
	}

protected:
};

template<typename T, const size_t NumPerBlock = (size_t)BaseConstants::NUM_EVENT_PER_BLOCK,
		enum MemoryCategory Catagory = MEMCAT_GENERAL>
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
		if( cnt == 1 )
			return reinterpret_cast<pointer>(BaseAllocator::AllocSingle(sizeof(T)));
		else
			return reinterpret_cast<pointer>(BaseAllocator::AllocMultiple(
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
