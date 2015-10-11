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
template<const size_t _NumChunkPerBlock, typename Allocator>
class Pool {

private:
	Pool(const Pool& p);

public:
	Pool(Pool&& p) {
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
		_ChunkSize = p._ChunkSize;
		_BlockSize = p._BlockSize;
		freeBlock = p.freeBlock;
		usedBlock = p.usedBlock;
		freeChunk = p.freeChunk;
		p._ChunkSize = 0;
		p._BlockSize = 0;
		p.freeBlock = 0;
		p.usedBlock = 0;
		p.freeChunk = 0;
	}
	/** */
	Pool(const size_t chunkSize, size_t numInitialBlocks = 0) :
			freeBlock(nullptr), usedBlock(nullptr), freeChunk(nullptr) {
#ifdef NEX_DEBUG
		_TotalBlockAllocations = 0;
		_TotalChunksAskedFor = 0;
		_TotalChunksFreed = 0;
		_CurrentTotalChunks = 0;
		_MaxActiveChunks = 0;
#endif
		_ChunkSize = chunkSize;
		_BlockSize = chunkSize * _NumChunkPerBlock;
		NEX_ASSERT(chunkSize >= sizeof(void*));
		for (size_t i = 0; i < numInitialBlocks; ++i) {
			_AddToFreeBlocks(_AllocBlock());
		}
	}

	~Pool() {
		FreePool();
	}

	inline size_t GetChunkSize() const {
		return _ChunkSize;
	}

	inline void* Alloc() {
		return _Alloc();
	}

	inline void* Alloc(size_t n) {
		return _Alloc(n);
	}

	inline void Free(void* ptr) {
		_Free(ptr);
	}

	inline void Free(void* ptr, size_t n) {
		_Free(ptr, n);
	}

	void FreePool() {
#ifdef NEX_DEBUG
		_PrintDebug();
#endif
		_FreeBlocks(freeBlock);
		_FreeBlocks(usedBlock);
		freeBlock = nullptr;
		usedBlock = nullptr;
	}

protected:

	void*& _NextBlock(void* const block) {
		void* _next =
				static_cast<void*>(static_cast<uint8*>(block) + _BlockSize);
		return *static_cast<void**>(_next);
	}

	void*& _Next(void* const chunk) {
		return *static_cast<void**>(chunk);
	}

	void*& _Last(void* const block) {
		void* _last = static_cast<void*>(static_cast<uint8*>(block) + _BlockSize
				- _ChunkSize);
		return *static_cast<void**>(_last);
	}

	void* _Alloc() {
		if (!freeChunk) {
			_AddFreeChunks();
		}
		NEX_ASSERT(freeChunk);
		void* v = freeChunk;
		freeChunk = _Next(freeChunk);
#ifdef NEX_DEBUG
		_CurrentTotalChunks++;
		_TotalChunksAskedFor++;
		if (_CurrentTotalChunks > _MaxActiveChunks)
			_MaxActiveChunks = _CurrentTotalChunks;
#endif
		return v;
	}

	void* _Alloc(size_t n) {
		NEX_ASSERT(_NumChunkPerBlock >= n);

		/** Add a new block and consider it as a series of new
		 * chunks. */
		_AddFreeChunks();
		void* v = static_cast<uint8*>(freeChunk) + _ChunkSize * n;
		freeChunk = v;
#ifdef NEX_DEBUG
		_CurrentTotalChunks+=n;
		_TotalChunksAskedFor+=n;
		if (_CurrentTotalChunks > _MaxActiveChunks)
			_MaxActiveChunks = _CurrentTotalChunks;
#endif
		return v;
	}

	void _Free(void* const chunk) {
		_Next(chunk) = freeChunk;
		NEX_ASSERT(_Next(chunk) != chunk); // cyclic
		freeChunk = chunk;
		_TotalChunksFreed++;
		_CurrentTotalChunks--;
	}

	void _Free(void* ptr, size_t n) {
		NEX_ASSERT(_NumChunkPerBlock >= n);
		for (size_t i = 0; i < n; ++i) {
			_Free(ptr);
			ptr = static_cast<uint8*>(ptr) + _ChunkSize;
		}
		_TotalChunksFreed+=n;
		_CurrentTotalChunks-=n;
	}

	inline void _AddFreeChunks() {

		if (!freeBlock) {
			void* block = _AllocBlock(nullptr);
			_AddToFreeBlocks(block);
		}

		if (freeBlock) {
			void* block = freeBlock;
			freeBlock = _NextBlock(freeBlock);
			_AddToUsedBlocks(block);
			_Last(block) = freeChunk;
			freeChunk = block;
		}
	}

	inline void* _AllocBlock(void* const end = nullptr) {
		void* block = NEX_ALLOCATOR_ALLOC(_BlockSize + sizeof(void*),
				Allocator);
		_UpdateFreeChunks(block, end);
		return block;
	}

	inline void _AddToFreeBlocks(void* const block) {
		_NextBlock(block) = freeBlock;
		freeBlock = block;
	}

	inline void _AddToUsedBlocks(void * const block) {
		_NextBlock(block) = usedBlock;
		usedBlock = block;
#ifdef NEX_DEBUG
		_TotalBlockAllocations++;
#endif
	}

	inline void _AddToFreeChunks(void* const chunk) {
		_Next(chunk) = freeChunk;
		NEX_ASSERT(_Next(chunk) != chunk); // cyclic
		freeChunk = chunk;
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

	void _UpdateFreeChunks(void * const block, void* const end) {
		uint8* last = (static_cast<uint8*>(block) + (_BlockSize - _ChunkSize));
		_Next(last) = end;
		NEX_ASSERT(_Next(last) != last); // cyclic
		/**todo Pointer comparison, might cause portability issue! */
		for (uint8* it = last - _ChunkSize; it >= block; last = it, it -=
			_ChunkSize) {
			_Next(it) = last;
			NEX_ASSERT(_Next(it) != it); // cyclic
		}
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

		size_t freeChunkCount = 0;
		size_t usedBlockCount = 0;
		size_t freeBlockCount = 0;

		size_t count = 0;
		void * chunk = freeChunk;
		while (chunk) {
			count++;
			chunk = _Next(chunk);
		}
		freeChunkCount = count;
		info << "\n Free chunks: " << count;

		count = 0;
		chunk = usedBlock;
		while (chunk) {
			count++;
			chunk = _NextBlock(chunk);
		}
		usedBlockCount = count;
		info << "\n Used blocks: " << count;

		count = 0;
		chunk = freeBlock;
		while (chunk) {
			count++;
			chunk = _NextBlock(chunk);
		}
		freeBlockCount = count;
		info << "\n Free blocks: " << count;

		size_t total = ((usedBlockCount * _NumChunkPerBlock)
				+ (freeBlockCount * _NumChunkPerBlock) - freeChunkCount);
		info << "\n Total objects not destroyed: " << total
			 << "\n*********************************************\n";
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
	size_t _BlockSize;
	size_t _ChunkSize;
	void* freeBlock;
	void* usedBlock;
	void* freeChunk;
};

#define NEX_ENABLE_MEMORY_POOLS 1

template<const size_t NumPerBlock, typename Mutex, typename Allocator>
class MemoryPool {
	typedef Pool<NumPerBlock, Allocator> TPool;

	struct PoolType: public TPool, public Mutex {
		PoolType(size_t objectSize) :
				TPool(objectSize, 0) {
		}
	};

#if NEX_ENABLE_MEMORY_POOLS
	PoolType pool;
#else
	size_t _objectSize;
#endif

	MemoryPool(const MemoryPool& p);

public:
	MemoryPool(MemoryPool&& p)
#if NEX_ENABLE_MEMORY_POOLS
		pool(std::move(p.pool))
#else
		_objectSize(p.objectSize)
#endif
	{
	}

	MemoryPool(size_t objectSize) :
#if NEX_ENABLE_MEMORY_POOLS
		pool(objectSize)
#else
		_objectSize(objectSize)
#endif
	{
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
		return pool.Alloc(numOfObject);
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
		pool.Free(o, numOfObject);
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

	inline size_t GetChunkSize() const {
#if NEX_ENABLE_MEMORY_POOLS
		return pool.GetChunkSize();
#else
		return _objectSize;
#endif
	}

};

}

#endif //POOLALLOCATOR_H_
