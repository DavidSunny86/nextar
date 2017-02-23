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
public:
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
		NEX_ASSERT(chunkSize >= sizeof(void*));
		for (size_t i = 0; i < numInitialBlocks; ++i) {
			_AddToFreeBlocks(_AllocBlock(_ChunkSize*_NumChunkPerBlock));
		}
	}

	~Pool() {
		FreePool();
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
		_PrintDebug();
#endif
		_FreeBlocks(freeBlock);
		_FreeBlocks(usedBlock);
		freeBlock = nullptr;
		usedBlock = nullptr;
	}

protected:

	inline void*& _NextBlock(void* const block) {
		void* _next =
				static_cast<void*>(static_cast<uint8*>(block) + sizeof(size_t));
		return *static_cast<void**>(_next);
	}

	inline void*& _Next(void* const chunk) {
		return *static_cast<void**>(chunk);
	}

	inline size_t& _BlockSize(void* const block) {
		return *static_cast<size_t*>(block);
	}

	inline void*& _First(void* const block) {
		void* _first = static_cast<void*>(static_cast<uint8*>(block) + BlockOffset);
		return *static_cast<void**>(_first);
	}

	inline void*& _Last(void* const block) {
		size_t offset = _BlockSize(block) + BlockOffset
			- _ChunkSize;
		void* _last = static_cast<uint8*>(block) + offset;
		return *static_cast<void**>(_last);
	}

	void* _Alloc() {
		if (!freeChunk) {
			_AddFreeChunks(_NumChunkPerBlock);
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

	void* _Alloc(uint32 n) {
		/** Add a new block and consider it as a series of new
		 * chunks. */
		_AddFreeChunks(std::max(n, _NumChunkPerBlock));
		void* v = static_cast<uint8*>(freeChunk) + static_cast<size_t>(_ChunkSize) * n;
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
#ifdef NEX_DEBUG
		_TotalChunksFreed++;
		_CurrentTotalChunks--;
#endif
	}

	void _Free(void* ptr, uint32 n) {
		if (n >= _NumChunkPerBlock) {
			// a free block is available
			uint8* block = static_cast<uint8*>(ptr) - BlockOffset;
			_RemoveFromUsedBlocks(block);
			_UpdateFreeChunks(block, nullptr);
			_AddToFreeBlocks(block);
		} else {
			for (uint32 i = 0; i < n; ++i) {
				_Free(ptr);
				ptr = static_cast<uint8*>(ptr) + _ChunkSize;
			}
		}
#ifdef NEX_DEBUG
		_TotalChunksFreed+=n;
		_CurrentTotalChunks-=n;
#endif
	}

	inline void _AddFreeChunks(uint32 n) {
		if (!freeBlock) {
			void* block = _AllocBlock(n*_ChunkSize, nullptr);
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

	inline void* _AllocBlock(size_t blockSize, void* const end = nullptr) {
		void* block = NEX_ALLOCATOR_ALLOC(blockSize + BlockOffset,
				Allocator);
		_BlockSize(block) = blockSize;
		_UpdateFreeChunks(block, end);
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
		uint8* last = static_cast<uint8*>(_Last(block));
		uint8* first = static_cast<uint8*>(_First(block));

		_Next(last) = end;
		NEX_ASSERT(_Next(last) != last); // cyclic
		/**todo Pointer comparison, might cause portability issue! */
		for (uint8* it = last - _ChunkSize; it >= first; last = it, it -=
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
	const uint32 _NumChunkPerBlock;
	const uint32 _ChunkSize;

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
