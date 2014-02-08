
#ifndef POOLALLOCATOR_H_
#define POOLALLOCATOR_H_

#include "NexTypes.h"
#include "NexAlloc.h"
#include "NexThread.h"

namespace nextar {

	/*** 
	 * @todo Need more diagonistic infos regarding pool data, like average allocations for a specific type,
	 * total number of allocations per run of experience.
	 */
	template <const size_t NumChunkPerBlock,  const size_t ChunkSize, typename Allocator>
	class Pool {

		enum : size_t {
			BLOCK_SIZE = NumChunkPerBlock * ChunkSize,
			FAT_BLOCK_SIZE = BLOCK_SIZE + sizeof(void*),
			CHUNK_SIZE = ChunkSize,
			NUM_MAX_CHUNKS = NumChunkPerBlock,
		};
		
	public:
 		NEX_COMPILE_ASSERT(CHUNK_SIZE >= sizeof(void*));
		/** */
		Pool(size_t numInitialBlocks = 0);
		~Pool();
				
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
			_FreeBlocks(freeBlock);
#ifdef NEX_DEBUG
			_PrintDebug();
#endif
			_FreeBlocks(usedBlock);
			freeBlock = nullptr;
			usedBlock = nullptr;
		}

	protected:
		
		void*& _NextBlock(void* const block) {
			void* _next = static_cast<void*>(static_cast<uint8*>(block) + BLOCK_SIZE);
			return *static_cast<void**>(_next);
		}

		void*& _Next(void* const chunk) {
			return *static_cast<void**>(chunk);
		}

		void*& _Last(void* const block) {
			void* _last = static_cast<void*>(static_cast<uint8*>(block) + BLOCK_SIZE - CHUNK_SIZE);
			return *static_cast<void**>(_last);
		}
		
		void*	_Alloc();
		void*	_Alloc(size_t n);
		void	_Free(void* const);
		void	_Free(void* const, size_t n);

		inline void  _AddFreeChunks();
		inline void* _AllocBlock(void* const end = nullptr);
		inline void  _AddToFreeBlocks(void* const block);
		inline void  _AddToUsedBlocks(void* const block);
		inline void  _AddToFreeChunks(void* const chunk);
		void _FreeBlocks(void *const block);
		void  _UpdateFreeChunks(void* const block, void* const end);
		
		
#ifdef NEX_DEBUG
		void _PrintDebug() {
			size_t freeChunkCount = 0;
			size_t usedBlockCount = 0;
			size_t freeBlockCount = 0;

			size_t count = 0;
			void * chunk = freeChunk;
			while(chunk) {
				count++;
				chunk = _Next(chunk);
			}
			freeChunkCount = count;
			std::cerr << "Free chunks: " << count << std::endl;
						
			count = 0;
			chunk = usedBlock;
			while(chunk) {
				count++;
				chunk = _NextBlock(chunk);
			}
			usedBlockCount = count;
			std::cerr << "Used blocks: " << count << std::endl;

			count = 0;
			chunk = freeBlock;
			while(chunk) {
				count++;
				chunk = _NextBlock(chunk);
			}
			freeBlockCount = count;
			std::cerr << "Free blocks: " << count << std::endl;

			size_t total = ((usedBlockCount * NUM_MAX_CHUNKS) + (freeBlockCount * NUM_MAX_CHUNKS) - freeChunkCount);
			std::cerr << "Total objects not destroyed: " << total << std::endl;
		}
#endif

		void* freeBlock;
		void* usedBlock;
		void* freeChunk;
	};

	template <const size_t BlockSize, const size_t ChunkSize, typename Allocator>
	inline Pool<BlockSize, ChunkSize, Allocator>::Pool(size_t initialSize) : freeBlock(nullptr),
		usedBlock(nullptr), freeChunk(nullptr) {
		for(size_t i = 0; i < initialSize; ++i) {
			_AddToFreeBlocks(_AllocBlock());
		}
	}

	template <const size_t BlockSize, const size_t ChunkSize, typename Allocator>
	inline void* Pool<BlockSize, ChunkSize, Allocator>::_AllocBlock(void* const end) {
		void* block = NEX_ALLOCATOR_ALLOC(FAT_BLOCK_SIZE, Allocator);
		_UpdateFreeChunks(block, end);
		return block;
	}

	template <const size_t BlockSize, const size_t ChunkSize, typename Allocator>
	inline void Pool<BlockSize, ChunkSize, Allocator>::_AddToFreeBlocks(void *const block) {
		_NextBlock(block) = freeBlock;
		freeBlock = block;	
	}

	template <const size_t BlockSize, const size_t ChunkSize, typename Allocator>
	inline void Pool<BlockSize, ChunkSize, Allocator>::_AddToFreeChunks(void *const chunk) {
		_Next(block) = freeBlock;
		freeBlock = block;	
	}

	template <const size_t BlockSize, const size_t ChunkSize, typename Allocator>
	inline void Pool<BlockSize, ChunkSize, Allocator>::_AddToUsedBlocks(void *const block) {
		_NextBlock(block) = usedBlock;
		usedBlock = block;	
	}

	template <const size_t BlockSize, const size_t ChunkSize, typename Allocator>
	void Pool<BlockSize, ChunkSize, Allocator>::_UpdateFreeChunks(void *const block, void* const end) {
		uint8* last = (static_cast<uint8*>(block) + (BLOCK_SIZE - CHUNK_SIZE));
		_Next(last) = end;
		/**@todo Pointer comparison, might cause portability issue! */
		for (uint8* it = last - CHUNK_SIZE; it >= block; last = it, it -= CHUNK_SIZE) 
			_Next(it) = last;
	}

	template <const size_t BlockSize, const size_t ChunkSize, typename Allocator>
	void Pool<BlockSize, ChunkSize, Allocator>::_AddFreeChunks() {
		
		if (!freeBlock) {
			void* block = _AllocBlock(nullptr);
			_AddToFreeBlocks(block);
		}

		if(freeBlock) {
			void* block = freeBlock;
			freeBlock = _NextBlock(freeBlock);
			_AddToUsedBlocks(block);
			_Last(block) = freeChunk;
			freeChunk = block;
		}
	}

	template <const size_t BlockSize, const size_t ChunkSize, typename Allocator>
	void* Pool<BlockSize, ChunkSize, Allocator>::_Alloc() {
		if (!freeChunk) {
			_AddFreeChunks();
		} 
		NEX_ASSERT(freeChunk);
		void* v = freeChunk;
		freeChunk = _Next(freeChunk);
		return v;
	}

	template <const size_t BlockSize, const size_t ChunkSize, typename Allocator>
	void* Pool<BlockSize, ChunkSize, Allocator>::_Alloc(size_t n) {
		NEX_ASSERT(NUM_MAX_CHUNKS >= n);

		/** Add a new block and consider it as a series of new
		 * chunks. */
		_AddFreeChunks();
		void* v = static_cast<uint8*>(freeChunk) + CHUNK_SIZE*n;
		freeChunk = v;
		return v;
	}

	
	template <const size_t BlockSize, const size_t ChunkSize, typename Allocator>
	void Pool<BlockSize, ChunkSize, Allocator>::_Free(void* ptr, size_t n) {
		NEX_ASSERT(NUM_MAX_CHUNKS >= n);
		for(size_t i = 0; i < n; ++i) {
			_Free(ptr);
			ptr = static_cast<uint8*>(ptr) + CHUNK_SIZE;
		}
	}

	template <const size_t BlockSize, const size_t ChunkSize, typename Allocator>
	void Pool<BlockSize, ChunkSize, Allocator>::_Free(void* const chunk) {
		_Next(chunk) = freeChunk;
		freeChunk = chunk;
	}

	template <const size_t BlockSize, const size_t ChunkSize, typename Allocator>
	Pool<BlockSize, ChunkSize, Allocator>::~Pool() {
		FreePool();
	}

	template <const size_t BlockSize, const size_t ChunkSize, typename Allocator>
	void Pool<BlockSize, ChunkSize, Allocator>::_FreeBlocks(void* const block) {
		size_t deleteCount = 0;
		void* it = block;
		while(it) {
			void* del = it;
			it = _NextBlock(it);
			NEX_ALLOCATOR_FREE(del, Allocator); 
			deleteCount++;
		}
#ifdef NEX_DEBUG
		std::cerr << "Deleted Blocks: " << deleteCount << std::endl;
#endif
	}

	template <typename T, const size_t NumPerBlock, typename Mutex, typename Allocator>
	class MemPool {
		typedef Pool<NumPerBlock, sizeof(T), Allocator> TPool;
		
		struct PoolType : public TPool, Mutex {
		};

		PoolType pool;

	public:
		MemPool() {}
		
		T* Alloc() {
			void* ptr;
			{
				NEX_THREAD_LOCK_GUARD_MUTEX_T(Mutex, pool);
				ptr = pool.Alloc();
			}
			return ConstructObject<T>(reinterpret_cast<T*>(ptr));
		}

		
		T* Alloc(size_t n) {
			void* ptr;
			{
				NEX_THREAD_LOCK_GUARD_MUTEX_T(Mutex, pool);
				ptr = pool.Alloc(n);
			}
			return ConstructObjects<T>(reinterpret_cast<T*>(ptr), (long)n);
		}

		void Free(T* o) {
			DestroyObject<T>(o);
			{
				NEX_THREAD_LOCK_GUARD_MUTEX_T(Mutex, pool);
				pool.Free(o);
			}
		}

		void Free(T* o, size_t n) {
			DestroyObjects<T>(o, n);
			{
				NEX_THREAD_LOCK_GUARD_MUTEX_T(Mutex, pool);
				pool.Free(o, n);
			}
		}

		void FreePool() {
			NEX_THREAD_LOCK_GUARD_MUTEX_T(Mutex, pool);
			pool.FreePool();
		}
	};

}

#endif //POOLALLOCATOR_H_