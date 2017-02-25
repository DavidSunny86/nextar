//	
#ifndef NEXTAR_MEMORY_MANAGER_H__
#define NEXTAR_MEMORY_MANAGER_H__

#include <NexBase.h>
#include <Singleton.h>
#include <NexThread.h>
#include <MemoryPool.h>

namespace nextar {


class StaticMemoryPoolBase : public AllocGeneral {
public:

	StaticMemoryPoolBase() : _ref(0) {}
	virtual ~StaticMemoryPoolBase() {}

	virtual uint32 GetObjectSize() const = 0;
	virtual uint32 GetDefaultNumObjectsPerBlock() const = 0;
	virtual MemoryCategory GetMemoryCatagory() const = 0;

	virtual void Destroy() = 0;

protected:

	atomic_int _ref;
};

template <const MemoryCategory MemCat = MEMCAT_GENERAL>
class StaticMemoryPool : public StaticMemoryPoolBase {
public:
	typedef StaticMemoryPool<MemCat> Type;
	typedef MemoryPool<MemCat, Mutex> PoolType;

	class Ref {
	public:
		Ref(StaticMemoryPoolBase* rf) : _pointer(static_cast<Type*>(rf)) {
			_pointer->_ref.fetch_add(1);
			_pool = &_pointer->_pool;
		}

		Ref(const Ref& rf) : _pointer(rf._pointer) {
			_pointer->_ref.fetch_add(1);
			_pool = &_pointer->_pool;
		}

		Ref(Ref&& rf) : _pointer(rf._pointer) {
			rf._pointer = nullptr;
			rf._pool = nullptr;
			_pool = &_pointer->_pool;
		}

		~Ref() {
			if (_pointer && _pointer->_ref.fetch_sub(1) == 1)
				MemoryManager::Instance().ReleasePoolInstance(_pointer);
		}

		operator PoolType* () {
			return _pool;
		}

		PoolType* _pool;
		StaticMemoryPool<MemCat>* _pointer;
	};


	StaticMemoryPool(uint32 objectSize, uint32 defaultNumChunksPerBlock) : _pool(objectSize, defaultNumChunksPerBlock) {}

protected:
	friend class Ref;
	PoolType _pool;
};

_NexTemplateExtern template class _NexBaseAPI StaticMemoryPool<MEMCAT_GENERAL>;
_NexTemplateExtern template class _NexBaseAPI StaticMemoryPool<MEMCAT_CACHEALIGNED>;
_NexTemplateExtern template class _NexBaseAPI StaticMemoryPool<MEMCAT_SIMDALIGNED>;
_NexTemplateExtern template class _NexBaseAPI StaticMemoryPool<MEMCAT_MATH_CORE>;
_NexTemplateExtern template class _NexBaseAPI StaticMemoryPool<MEMCAT_BUFFER_DATA>;

class _NexBaseAPI MemoryManager : public Singleton<MemoryManager>,
public AllocGeneral {
public:

	MemoryManager();
	~MemoryManager();

	static uint64 GetPoolID(uint32 objectSize, uint32 numBlockSize, MemoryCategory cat) {
		return (uint64(objectSize) << 40) | (uint64(numBlockSize) << 10) | cat;
	}

	void ReleaseEverything();

	StaticMemoryPoolBase* AcquirePoolInstance(MemoryCategory memCat, uint32 objectSize, uint32 numPerBlock);
	void ReleasePoolInstance(StaticMemoryPoolBase* memPool);

protected:

	typedef unordered_map<uint64, StaticMemoryPoolBase*>::type MemoryPoolMap;
	
	Mutex contentLock;
	MemoryPoolMap memPools;
	
	static MemoryManager _instance;
};

}

#endif //NEXTAR_MEMORY_MANAGER_H__
