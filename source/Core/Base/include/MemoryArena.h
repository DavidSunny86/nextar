#ifndef NEXTAR_MEMORYARENA_H_
#define NEXTAR_MEMORYARENA_H_

#include <MemoryPool.h>
#include <NexThread.h>

namespace nextar {

// this is a non-threadsafe memory pool which can be used in a single context/
// operation for quick allocations.
template <typename T, const uint32 BlockSize = 32, typename Allocator = AllocatorGeneral>
class MemoryArena {

public:
  MemoryArena() {
    Push();
  }

  ~MemoryArena() {
    Pop();
  }

  inline void Push() {
    poolStack.emplace_back(sizeof(T));
  }

  inline void Pop() {
    poolStack.pop_back();
  }

  template<typename... _Args>
  T* Alloc(_Args&&... __args) {
  	return ConstructObject(_InternalAlloc(), std::forward<_Args>(__args)...);
  }

  void Free(T* p) {
    DestroyObject(p);
    _InternalFree(p);
  }

private:

  T* _InternalAlloc() {
    return reinterpret_cast<T*>(poolStack.back().Alloc());
  }

  void _InternalFree(T* p) {
    poolStack.back().Free(p);
  }
  
  typedef MemoryPool<BlockSize, NullMutex, Allocator> MemoryPoolType;
  typedef vector<MemoryPoolType>::type PoolStack;

  PoolStack poolStack;
};

}

#endif // NEXTAR_MEMORYARENA_H_
