#ifndef POOLALLOCATOR_H_
#define POOLALLOCATOR_H_

#include <NexBase.h>
#include <NexAlloc.h>
#include <NexThread.h>

namespace nextar {
/***
 * @todo Need more diagonistic infos regarding pool data, like average allocations for a specific type,
 * total number of allocations per run of experience.
 */
template<const MemoryCategory MemCat = MEMCAT_GENERAL>
class Pool {

private:
	Pool(const Pool& p);

	// A block is
	// [ Alignment Size | chunk 0 | chunk 1 | chunk n... ]
	// Alignment Size = [ block-size, next-block ... ]
	// block-size = n * chunk size
	// total-block-size = block-size + block-offset
	// Chunks store pointer-diff to next chunk
	// If the ptr-diff value is less than chunk-size, then it indicates two things:
	//  if value == chunk-size - 1
	//    next chunk holds the count of continuous block from this chunk
	// else if value == chunk-size - 2
	//    next chunk holds ptr-diff to another discontinuous chunk
	// else
	//    error
public:
	enum Constraints {
		MIN_OBJECT_SIZE = sizeof(ptrdiff_t),
		MIN_BUCKET_SIZE = 4,
	};

	typedef AllocatorBase<MemCat> Allocator;
	static const size_t BlockHeaderSize = sizeof(void*) + sizeof(size_t);
	static const size_t BlockOffset = AllocatorBase<MemCat>::Offset<BlockHeaderSize>::_value;
	Pool(Pool&& p) : 
		_ChunkSize(p._ChunkSize)
		, _NumChunkPerBlock(p._NumChunkPerBlock) {
	#ifdef NEX_DEBUG
		_TotalBlockAllocations = p._TotalBlockAllocations;
		_TotalChunksAskedFor = p._TotalChunksAskedFor;
		_TotalChunksFreed = p._TotalChunksFreed;
		_CurrentTotalChunks = p._CurrentTotalChunks;
		_MaxActiveChunks = p._MaxActiveChunks;
		p._TotalBlockAllocations = 0;
		p._TotalChunksAskedFor = 0;
		p._TotalChunksFreed = 0;
		p._CurrentTotalChunks = 0;
		p._MaxActiveChunks = 0;
	#endif
		freeBlock = p.freeBlock;
		usedBlock = p.usedBlock;
		freeChunk = p.freeChunk;
		p._ChunkSize = 0;
		p._NumChunkPerBlock = 0;
		p.freeBlock = 0;
		p.usedBlock = 0;
		p.freeChunk = 0;
	}
	/** */
	Pool(uint32 chunkSize, uint32 numChunks, uint32 numInitialBlocks = 0) :
		_ChunkSize(chunkSize)
		, _NumChunkPerBlock(numChunks)
		, freeBlock(nullptr)
		, usedBlock(nullptr)
		, freeChunk(nullptr) {
#ifdef NEX_DEBUG
		_TotalBlockAllocations = 0;
		_TotalChunksAskedFor = 0;
		_TotalChunksFreed = 0;
		_CurrentTotalChunks = 0;
		_MaxActiveChunks = 0;
#endif
		NEX_ASSERT(chunkSize >= MIN_OBJECT_SIZE);
		NEX_ASSERT(_NumChunkPerBlock >= MIN_BUCKET_SIZE);
		for (size_t i = 0; i < numInitialBlocks; ++i) {
			_AddToFreeBlocks(_AllocBlock(_NumChunkPerBlock));
		}
	}

	Pool() :
			_ChunkSize(0)
			, _NumChunkPerBlock(0)
			, freeBlock(nullptr)
			, usedBlock(nullptr)
			, freeChunk(nullptr) {
	#ifdef NEX_DEBUG
			_TotalBlockAllocations = 0;
			_TotalChunksAskedFor = 0;
			_TotalChunksFreed = 0;
			_CurrentTotalChunks = 0;
			_MaxActiveChunks = 0;
	#endif
	}

	~Pool() {
		FreePool();
	}

	inline void InitPool(uint32 chunkSize, uint32 numChunks) {
		NEX_ASSERT(chunkSize >= MIN_OBJECT_SIZE && !_ChunkSize);
		NEX_ASSERT(numChunks >= MIN_BUCKET_SIZE && !_NumChunkPerBlock);
		_NumChunkPerBlock = numChunks;
		_ChunkSize = chunkSize;

	}

	inline uint32 GetChunkSize() const {
		return _ChunkSize;
	}

	inline uint32 GetDefaultNumChunksPerBlock() const {
		return _NumChunkPerBlock;
	}

	inline void* Alloc() {
		return _Alloc();
	}

	inline void* Alloc(uint32 n) {
		return _Alloc(n);
	}

	inline void Free(void* ptr) {
		_Free(ptr);
	}

	inline void Free(void* ptr, uint32 n) {
		_Free(ptr, n);
	}

	void FreePool() {
#ifdef NEX_DEBUG
		if (freeBlock || usedBlock || freeChunk)
			_PrintDebug();
#endif
		_FreeBlocks(freeBlock);
		_FreeBlocks(usedBlock);
		freeBlock = nullptr;
		usedBlock = nullptr;
		freeChunk = nullptr;

	}

protected:

	inline void*& _NextBlock(void* const block) const {
		void* _next =
				static_cast<void*>(static_cast<uint8*>(block) + sizeof(size_t));
		return *static_cast<void**>(_next);
	}

	inline size_t& _BlockSize(void* const block) const {
		return *static_cast<size_t*>(block);
	}

	inline void* _FirstPtr(void* const block) const {
		return static_cast<void*>(static_cast<uint8*>(block) + BlockOffset);
	}

	inline void* _LastPtr(void* const block) const {
		size_t offset = _BlockSize(block) + BlockOffset
			- _ChunkSize;
		return static_cast<uint8*>(block) + offset;
	}
	
	inline ptrdiff_t _SentinelValCh() const {
		return _ChunkSize >> 1;
	}

	inline bool _IsSentinelCh(void* const chunk) const {
		if (_ValCh(chunk) == _SentinelValCh())
			return true;
		else {
			NEX_ASSERT(std::abs(*static_cast<ptrdiff_t*>((chunk))) >= _ChunkSize);
			return false;
		}
	}

	inline void* _NextPtrCh(void* const chunk) const {
		ptrdiff_t off = _ValCh(chunk);
		return static_cast<uint8*>(chunk) + off;
	}

	inline void _StoreNextCh(void* const chunk, void* next) const {
		_ValCh(chunk) = static_cast<uint8*>(next) - static_cast<uint8*>(chunk);
	}

	inline void* _NextCh(void* const chunk) const {
		return static_cast<uint8*>(chunk) + _ChunkSize;
	}

	inline ptrdiff_t _CountCh(void* const chunk) const {
		return (*reinterpret_cast<ptrdiff_t*>(static_cast<uint8*>(chunk) + _ChunkSize));
	}

	inline ptrdiff_t& _ValCh(void* const chunk) const {
		return (*static_cast<ptrdiff_t*>(chunk));
	}

	inline void _UpdateNextCh(void** store) const {
		void* chunk = *store;
		void* next = nullptr;
		if (_IsSentinelCh(chunk)) {
			next = static_cast<uint8*>(chunk) + _ChunkSize;
			ptrdiff_t newCount = _ValCh(next) - 1;
			if (newCount <= 2) {
				NEX_ASSERT(newCount == 2);
				_ValCh(next) = _ChunkSize;
			} else {
				_ValCh(next) = _SentinelValCh();
				_ValCh(_NextCh(next)) = newCount;
			}
		} else {
			next = _NextPtrCh(chunk);
		}
		*store = next;
	}
	
	bool _ConsequetiveCh(int32 n) const  {
		void* fc = freeChunk;
		return (freeChunk && _IsSentinelCh(freeChunk) && _ValCh(static_cast<uint8*>(freeChunk) + _ChunkSize) >= n) != 0;
	}

	inline void _UpdateAfterCh(void** store, uint32 n) const {
		void* chunk = *store;
		void* next = static_cast<uint8*>(chunk) + _ChunkSize;
		ptrdiff_t newCount = _ValCh(next) - static_cast<ptrdiff_t>(n);
		NEX_ASSERT(*static_cast<ptrdiff_t*>((chunk)) == _SentinelValCh());
		NEX_ASSERT(newCount >= 0);
		next = static_cast<uint8*>(chunk) + _ChunkSize*(n);
		if (!newCount) {
			next = _NextPtrCh(static_cast<uint8*>(next) - _ChunkSize);
		} else if (newCount == 1) {
		} else if (newCount == 2) {
			_ValCh(next) = _ChunkSize;
		} else if (newCount > 2) {
			_ValCh(next) = _SentinelValCh();
			_ValCh(_NextCh(next)) = newCount;
		}
		*store = next;
	}

	inline void _UpdateSentinelCh(void* const chunk, uint32 n, void* const end) const {
		void* last = static_cast<uint8*>(chunk) + (n - 1)*_ChunkSize;
		void* next = static_cast<uint8*>(chunk) + _ChunkSize;
		_ValCh(chunk) = _SentinelValCh();
		_ValCh(next) = static_cast<ptrdiff_t>(n);
		_StoreNextCh(last, end);
	}

	void* _Alloc() {
		if (!freeChunk) {
			_AddFreeChunks(_NumChunkPerBlock);
		}
		NEX_ASSERT(freeChunk);
		void* v = freeChunk;
		_UpdateNextCh(&freeChunk);
#ifdef NEX_DEBUG
		_CurrentTotalChunks++;
		_TotalChunksAskedFor++;
		if (_CurrentTotalChunks > _MaxActiveChunks)
			_MaxActiveChunks = _CurrentTotalChunks;
#endif
		return v;
	}
	
	void* _Alloc(uint32 n) {
		/** Add a new block and consider it as a series of new
		 * chunks. */
		if (!_ConsequetiveCh(n))
			_AddFreeChunks(std::max(n, _NumChunkPerBlock));
		void* v = freeChunk;
		_UpdateAfterCh(&freeChunk, n);
#ifdef NEX_DEBUG
		_CurrentTotalChunks+=n;
		_TotalChunksAskedFor+=n;
		if (_CurrentTotalChunks > _MaxActiveChunks)
			_MaxActiveChunks = _CurrentTotalChunks;
#endif
		return v;
	}

	void _Free(void* const chunk) {
		// if current freeChunk is a sentinel, lets not ruin it
		// and we push this chunk after the sentinel, otherwise
		// just replace
		if (freeChunk && _IsSentinelCh(freeChunk)) {
			uint8* p = static_cast<uint8*>(freeChunk) + (_CountCh(freeChunk)-1) * _ChunkSize;
			_StoreNextCh(chunk, _NextPtrCh(p));
			_StoreNextCh(p, chunk);
		} else {
			_StoreNextCh(chunk, freeChunk);
			freeChunk = chunk;
		}
#ifdef NEX_DEBUG
		_TotalChunksFreed++;
		_CurrentTotalChunks--;
#endif
	}

	void _Free(void* ptr, uint32 n) {
#ifdef NEX_DEBUG
		_TotalChunksFreed += n;
		_CurrentTotalChunks -= n;
#endif
		if (n >= _NumChunkPerBlock) {
			// a free block is available
			uint8* block = static_cast<uint8*>(ptr) - BlockOffset;
			if (_BlockSize(block) == (n * _ChunkSize)) {
				_RemoveFromUsedBlocks(block);
				_UpdateFreeChunks(block, n, nullptr);
				_AddToFreeBlocks(block);
				//or 
				// NEX_ALLOCATOR_FREE(block, Allocator);
				return;
			}
		} 
		if (n < 3) {
			for (uint32 i = 0; i < n; ++i) {
				_Free(ptr);
				ptr = static_cast<uint8*>(ptr) + _ChunkSize;
			}
		} else {
			uint8* chunk = static_cast<uint8*>(ptr);
			_UpdateSentinelCh(chunk, n, freeChunk);
			freeChunk = chunk;
		}
	}

	inline void _AddFreeChunks(uint32 n) {
		if (!freeBlock) {
			void* block = _AllocBlock(n, nullptr);
			_AddToFreeBlocks(block);
		}

		if (freeBlock) {
			void* block = freeBlock;
			freeBlock = _NextBlock(freeBlock);
			_AddToUsedBlocks(block);
			_StoreNextCh(_LastPtr(block), freeChunk);
			freeChunk = _FirstPtr(block);
		}
	}

	inline void* _AllocBlock(uint32 n, void* const end = nullptr) {
		size_t blockSize = (n*_ChunkSize);
		void* block = NEX_ALLOCATOR_ALLOC(blockSize + BlockOffset,
				Allocator);
		_BlockSize(block) = blockSize;
		_UpdateFreeChunks(block, n, end);
#ifdef NEX_DEBUG
		_TotalBlockAllocations++;
#endif
		return block;
	}

	inline void _AddToFreeBlocks(void* const block) {
		_NextBlock(block) = freeBlock;
		freeBlock = block;
	}

	inline void _AddToUsedBlocks(void * const block) {
		_NextBlock(block) = usedBlock;
		usedBlock = block;
	}

	inline void _RemoveFromUsedBlocks(void * const block) {
		void **prev = &usedBlock;
		void *cur = usedBlock;
		while (cur) {
			if (cur == block) {
				*prev = _NextBlock(block);
				return;
			}
			prev = &_NextBlock(cur);
			cur = _NextBlock(cur);
		}
	}
	
	void _FreeBlocks(void* const block) {
		size_t deleteCount = 0;
		void* it = block;
		while (it) {
			void* del = it;
			it = _NextBlock(it);
			NEX_ALLOCATOR_FREE(del, Allocator);
			deleteCount++;
		}
	}

	void _UpdateFreeChunks(void * const block, uint32 n, void* const end) {
		uint8* first = static_cast<uint8*>(_FirstPtr(block));
		_UpdateSentinelCh(first, n, end);
	}

#ifdef NEX_DEBUG
	void _PrintDebug() {
		std::ostringstream info;
		info << "\n*********************************************"
			 << "\n Pool Info"
			 << "\n ChunkSize: " << _ChunkSize << " NumChunkPerBlock: " << _NumChunkPerBlock
			 << "\n _TotalBlockAllocations: " << _TotalBlockAllocations
			 << "\n _TotalChunksAskedFor: " << _TotalChunksAskedFor
			 << "\n _TotalChunksFreed: " << _TotalChunksFreed
			 << "\n _MaxActiveChunks: " << _MaxActiveChunks
			 << "\n*********************************************";
		Platform::OutputDebug(info.str().c_str());
	}
#endif
#ifdef NEX_DEBUG
	size_t _TotalBlockAllocations;
	size_t _TotalChunksAskedFor;
	size_t _TotalChunksFreed;
	size_t _CurrentTotalChunks;
	size_t _MaxActiveChunks;
#endif
	uint32 _NumChunkPerBlock;
	uint32 _ChunkSize;

	void* freeBlock;
	void* usedBlock;
	void* freeChunk;
};

#define NEX_ENABLE_MEMORY_POOLS 1

template<const MemoryCategory MemCat, typename Mutex>
class MemoryPool {
	typedef Pool<MemCat> TPool;

	struct PoolType: public TPool, public Mutex {
		PoolType(uint32 objectSize, uint32 numObj) :
				TPool(objectSize, numObj, 0) {
		}
	};

#if NEX_ENABLE_MEMORY_POOLS
	PoolType pool;
#else
	size_t _objectSize;
#endif

	MemoryPool(const MemoryPool& p);

public:
	MemoryPool(MemoryPool&& p) :
#if NEX_ENABLE_MEMORY_POOLS
		pool(std::move(p.pool))
#else
		_objectSize(p.objectSize)
#endif
	{
	}

	MemoryPool(uint32 objectSize, uint32 numObjectInArr) :
#if NEX_ENABLE_MEMORY_POOLS
		pool(objectSize, numObjectInArr)
#else
		_objectSize(objectSize)
#endif
	{
	}

	MemoryPool() :
#if NEX_ENABLE_MEMORY_POOLS
		pool()
#else
		_objectSize(0)
#endif
	{
	}

	inline void InitPool(uint32 objSize, uint32 numObjInArr) {
#if NEX_ENABLE_MEMORY_POOLS
		pool.InitPool(objSize, numObjInArr)
#else
		NEX_ASSERT(!_objectSize);
		_objectSize = objSize;
#endif
	}

	void* Alloc() {
#if NEX_ENABLE_MEMORY_POOLS
		NEX_THREAD_LOCK_GUARD_MUTEX_T(PoolType, pool);
		return pool.Alloc();
#else
	return NEX_ALLOCATOR_ALLOC(_objectSize,
				Allocator);
#endif
	}

	void* Alloc(size_t numOfObject) {
#if NEX_ENABLE_MEMORY_POOLS
		NEX_THREAD_LOCK_GUARD_MUTEX_T(PoolType, pool);
		return pool.Alloc((uint32)numOfObject);
#else
	return NEX_ALLOCATOR_ALLOC(_objectSize*numOfObject,
				Allocator);
#endif
	}

	void Free(void* o) {
#if NEX_ENABLE_MEMORY_POOLS
		NEX_THREAD_LOCK_GUARD_MUTEX_T(PoolType, pool);
		pool.Free(o);
#else
		NEX_ALLOCATOR_FREE(o, Allocator);
#endif
	}

	void Free(void* o, size_t numOfObject) {
#if NEX_ENABLE_MEMORY_POOLS
		NEX_THREAD_LOCK_GUARD_MUTEX_T(PoolType, pool);
		pool.Free(o, (uint32)numOfObject);
#else
		NEX_ALLOCATOR_FREE(o, Allocator);
#endif
	}

	void FreePool() {
#if NEX_ENABLE_MEMORY_POOLS
		NEX_THREAD_LOCK_GUARD_MUTEX_T(PoolType, pool);
		pool.FreePool();
#endif
	}

	inline uint32 GetChunkSize() const {
#if NEX_ENABLE_MEMORY_POOLS
		return pool.GetChunkSize();
#else
		return _objectSize;
#endif
	}

	inline uint32 GetDefaultNumChunksPerBlock() const {
#if NEX_ENABLE_MEMORY_POOLS
		return pool.GetDefaultNumChunksPerBlock();
#else
		return 0;
#endif
	}

};

template <typename T, const MemoryCategory MemCat, typename Mutex>
class TypedMemoryPool {
	// deleted
	TypedMemoryPool(const TypedMemoryPool&);
public:
	typedef MemoryPool<MemCat, Mutex> MemoryPoolType;

protected:

	MemoryPoolType impl;

public:
	TypedMemoryPool(uint32 numPerBlock) : impl(sizeof(T), numPerBlock) {}
	TypedMemoryPool(TypedMemoryPool&& other) : impl(std::move(other.impl)) {}

	T* AllocType() {
		return reinterpret_cast<T*>(impl.Alloc());
	}

	void FreeType(T* t) {
		impl.Free(t);
	}


	T* AllocType(uint32 n) {
		return reinterpret_cast<T*>(impl.Alloc(n));
	}

	void FreeType(T* t, uint32 n) {
		impl.Free(t, n);
	}
};

}

#endif //POOLALLOCATOR_H_
