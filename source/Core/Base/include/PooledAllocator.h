#ifndef MATRIXPOOL_H_
#define MATRIXPOOL_H_

#include <NexAlloc.h>
#include <MemoryManager.h>
#include <MemoryPool.h>
#include <NexThread.h>

namespace nextar {

template<const size_t ObjectSize, const uint32 NumPerBlock, enum MemoryCategory Catagory>
class SingletonPoolProvider {
public:
	NEX_STATIC_ASSERT(ObjectSize < 0xffffff);

	const static size_t ArrayHeader = (AllocatorBase<Catagory>::Offset<sizeof(uint32)>::_value / ObjectSize) + 1;

	typedef StaticMemoryPool<Catagory> Provider;
	typedef typename Provider::PoolType PoolType;
	typedef typename Provider::Ref RefType;
	typedef SingletonPoolProvider<ObjectSize, NumPerBlock, Catagory> Type;
	
	/* optimized */
	inline static void* AllocSingle() {
		return Instance()->Alloc();
	}

	inline static void FreeSingle(void* ptr) {
		Instance()->Free(ptr);
	}

	/* use base allocator */
	inline static void* AllocMultiple(uint32 amount) {
		void* ret = Instance()->Alloc(ArrayHeader + amount);
		*static_cast<uint32*>(ret) = amount;
		return static_cast<uint8*>(ret) + ArrayHeader;
	}

	inline static void* AllocMultipleKnown(uint32 amount) {
		return Instance()->Alloc(amount);
	}

	inline static void FreeMultiple(void* ptr) {
		void* item = static_cast<uint8*>(ptr) - ArrayHeader;
		uint32 amount = *static_cast<uint32*>(item);
		Instance()->Free(item, amount);
	}

	inline static void FreeMultipleKnown(void* ptr, uint32 amount) {
		Instance()->Free(ptr, amount);
	}

	inline static void FreePool() {
		Instance()->FreePool();
	}

protected:
	
	inline static PoolType* Instance() {
		static RefType _singletonPool(MemoryManager::Instance().AcquirePoolInstance(Catagory, ObjectSize, NumPerBlock));
		return _singletonPool;
	}
};

template<typename T, const size_t NumPerBlock, enum MemoryCategory Catagory = MemoryCategory::MEMCAT_GENERAL>
class PooledAllocator {
	typedef T ObjectType;
	typedef SingletonPoolProvider<sizeof(T), NumPerBlock, Catagory> ProviderSpecialization;
public:
	typedef ProviderSpecialization Provider;
	typedef PooledAllocator<T, NumPerBlock, Catagory> Type;
	typedef AllocObjectBase<Type> AllocObjectType;

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
			return ProviderSpecialization::AllocSingle();
		NEX_ASSERT( !(amount % sizeof(T)) );
		return ProviderSpecialization::AllocMultiple((uint32)(amount / sizeof(T)));
	}

	inline static void Free(void* ptr) {
		ProviderSpecialization::FreeSingle(ptr);
	}

	inline static void Free(void* ptr, size_t amount) {
		NEX_ASSERT(!(amount % sizeof(T)));
		ProviderSpecialization::FreeMultiple(ptr);
	}

	inline static void* AllocSingle(size_t size) {
		NEX_ASSERT(size == sizeof(T));
		return ProviderSpecialization::AllocSingle();
	}

	inline static void FreeSingle(void* ptr) {
		ProviderSpecialization::FreeSingle(ptr);
	}

	/* use base allocator */
	inline static void* AllocMultiple(size_t n) {
		return ProviderSpecialization::AllocMultiple((uint32)n);
	}

	inline static void FreeMultiple(void* ptr) {
		ProviderSpecialization::FreeMultiple(ptr);
	}
	/* use base allocator */
	inline static void* AllocMultipleKnown(size_t n) {
		return ProviderSpecialization::AllocMultipleKnown((uint32)n);
	}

	inline static void FreeMultipleKnown(void* ptr, size_t n) {
		ProviderSpecialization::FreeMultipleKnown(ptr, (uint32)n);
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
			return reinterpret_cast<pointer>(BaseAllocator::AllocMultipleKnown(cnt));
	}

	inline void deallocate(pointer p, size_type cnt) const {
		if (cnt == 1)
			BaseAllocator::FreeSingle(p);
		else
			BaseAllocator::FreeMultipleKnown(p, cnt);
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
