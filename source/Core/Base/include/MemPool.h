
#ifndef POOLALLOCATOR_H_
#define POOLALLOCATOR_H_

#include <NexBase.h>
#include <NexAlloc.h>
#include <NexThread.h>

namespace nextar {

	/*** 
	* todo Need more diagonistic infos regarding pool data, like average allocations for a specific type,
	* total number of allocations per run of experience.
	*/
	template <const size_t NumChunkPerBlock, typename Allocator>
	class Pool {

		enum : size_t {
			NUM_MAX_CHUNKS = NumChunkPerBlock,
		};


	public:
		
		/** */
		Pool(const size_t chunkSize, size_t numInitialBlocks = 0) : 
			CHUNK_SIZE(chunkSize),
			BLOCK_SIZE(NUM_MAX_CHUNKS*CHUNK_SIZE),
			freeBlock(nullptr),
			usedBlock(nullptr), 
			freeChunk(nullptr) {
			NEX_ASSERT(CHUNK_SIZE >= sizeof(void*));
			for(size_t i = 0; i < numInitialBlocks; ++i) {
				_AddToFreeBlocks(_AllocBlock());
			}
		}

		~Pool();

		inline size_t GetChunkSize() const {
			return CHUNK_SIZE;
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

		void*	_Alloc() {
			if (!freeChunk) {
				_AddFreeChunks();
			} 
			NEX_ASSERT(freeChunk);
			void* v = freeChunk;
			freeChunk = _Next(freeChunk);
			return v;
		}

		void*	_Alloc(size_t n) {
			NEX_ASSERT(NUM_MAX_CHUNKS >= n);

			/** Add a new block and consider it as a series of new
			* chunks. */
			_AddFreeChunks();
			void* v = static_cast<uint8*>(freeChunk) + CHUNK_SIZE*n;
			freeChunk = v;
			return v;
		}

		void	_Free(void* const chunk) {
			_Next(chunk) = freeChunk;
			freeChunk = chunk;
		}

		void	_Free(void* ptr, size_t n) {
			NEX_ASSERT(NUM_MAX_CHUNKS >= n);
			for(size_t i = 0; i < n; ++i) {
				_Free(ptr);
				ptr = static_cast<uint8*>(ptr) + CHUNK_SIZE;
			}
		}


		inline void  _AddFreeChunks() {

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

		inline void* _AllocBlock(void* const end = nullptr) {
			void* block = NEX_ALLOCATOR_ALLOC(BLOCK_SIZE + sizeof(void*), Allocator);
			_UpdateFreeChunks(block, end);
			return block;
		}

		inline void  _AddToFreeBlocks(void* const block) {
			_NextBlock(block) = freeBlock;
			freeBlock = block;	
		}

		inline void  _AddToUsedBlocks(void *const block) {
			_NextBlock(block) = usedBlock;
			usedBlock = block;	
		}

		inline void  _AddToFreeChunks(void* const chunk) {
			_Next(chunk) = freeChunk;
			freeChunk = chunk;	
		}

		void _FreeBlocks(void* const block) {
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

		void  _UpdateFreeChunks(void *const block, void* const end) {
			uint8* last = (static_cast<uint8*>(block) + (BLOCK_SIZE - CHUNK_SIZE));
			_Next(last) = end;
			/**todo Pointer comparison, might cause portability issue! */
			for (uint8* it = last - CHUNK_SIZE; it >= block; last = it, it -= CHUNK_SIZE) 
				_Next(it) = last;
		}


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
		const size_t BLOCK_SIZE;
		const size_t CHUNK_SIZE;
		void* freeBlock;
		void* usedBlock;
		void* freeChunk;
	};

	template <const size_t NumPerBlock, typename Mutex, typename Allocator>
	class MemPool {
		typedef Pool<NumPerBlock, Allocator> TPool;

		struct PoolType : public TPool, public Mutex {
			PoolType(size_t objectSize) : TPool(objectSize, 0) {}
		};

		PoolType pool;

	public:
		MemPool(size_t objectSize) : pool(objectSize) {}

		void* Alloc() {
			NEX_THREAD_LOCK_GUARD_MUTEX_T(PoolType, pool);
			return pool.Alloc();
		}


		void* Alloc(size_t numOfObject) {
			NEX_THREAD_LOCK_GUARD_MUTEX_T(PoolType, pool);
			return pool.Alloc(numOfObject);
		}

		void Free(void* o) {
			NEX_THREAD_LOCK_GUARD_MUTEX_T(PoolType, pool);
			pool.Free(o);
		}

		void Free(void* o, size_t numOfObject) {
			NEX_THREAD_LOCK_GUARD_MUTEX_T(PoolType, pool);
			pool.Free(o, numOfObject);
		}

		void FreePool() {
			NEX_THREAD_LOCK_GUARD_MUTEX_T(PoolType, pool);
			pool.FreePool();
		}

		inline size_t GetChunkSize() const {
			return pool.GetChunkSize();
		}

	};

}

#endif //POOLALLOCATOR_H_
